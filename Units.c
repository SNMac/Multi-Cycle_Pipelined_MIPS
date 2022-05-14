//
// Created by SNMac on 2022/05/09.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "Units.h"
#include "main.h"

PROGRAM_COUNTER PC;
IFID ifid[2];
IDEX idex[2];
EXMEM exmem[2];
MEMWB memwb[2];
CONTROL_SIGNAL ctrlSig;
ALU_CONTROL_SIGNAL ALUctrlSig;
BRANCH_PREDICT BranchPred;
FORWARD_SIGNAL fwrdSig;
HAZARD_DETECTION_SIGNAL hzrddetectSig;
ID_FORWARD_SIGNAL idfwrdSig;

// from main.c
extern uint32_t Memory[0x400000];
extern uint32_t R[32];
extern COUNTING counting;

/*============================Data units============================*/

// [Instruction memory]
uint32_t InstMem(uint32_t Readaddr) {
    uint32_t Instruction = Memory[Readaddr / 4];
    return Instruction;
}

// [Registers]
uint32_t* RegsRead(uint8_t Readreg1, uint8_t Readreg2) {
    static uint32_t Readdata[2];
    Readdata[0] = R[Readreg1];
    Readdata[1] = R[Readreg2];
    return Readdata;
}
void RegsWrite(uint8_t Writereg, uint32_t Writedata, bool RegWrite) {
    if (RegWrite) {  // RegWrite asserted
        if (Writereg == 0 && Writedata != 0) {
            fprintf(stderr, "WARNING: Cannot change value at $0\n");
            return;
        }
        R[Writereg] = Writedata;  // GPR write enabled
        return;
    }
    else {  // RegWrite De-asserted
        return;  // GPR write disabled
    }
}

// [Data memory]
uint32_t DataMem(uint32_t Addr, uint32_t Writedata, bool MemRead, bool MemWrite) {
    uint32_t Readdata = 0;
    if (MemRead == 1 && MemWrite == 0) {  // MemRead asserted, MemWrite De-asserted
        if (Addr > 0x1000000) {  // loading outside of memory
            fprintf(stderr, "ERROR: Accessing outside of memory\n");
            exit(EXIT_FAILURE);
        }
        Readdata = Memory[Addr / 4];  // Memory read port return load value
        printf("Memory[0x%08x] load -> 0x%x (%d)\n", Addr, Memory[Addr / 4], Memory[Addr / 4]);
        counting.Memcount++;
    }
    else if (MemRead == 0 && MemWrite == 1) {  // MemRead De-asserted, MemWrite asserted
        if (Addr > 0x1000000) {  // Writing outside of memory
            fprintf(stderr, "ERROR: Accessing outside of memory\n");
            exit(EXIT_FAILURE);
        }
        Memory[Addr / 4] = Writedata;  // Memory write enabled
        printf("Memory[0x%08x] <- store 0x%x (%d)\n", Addr, Memory[Addr / 4], Memory[Addr / 4]);
        counting.Memcount++;
    }
    else {  // MemRead, MemWrite De-asserted
        printf("There is no memory access\n");
    }
    return Readdata;
}

// [Branch predictor]
void CheckBranch(uint32_t PCvalue) {  // Check if PC is branch instruction
    BranchPred.instPC[0] = PCvalue;
    if (BranchPred.BTBsize == 0) {  // BTB is empty
        BranchPred.AddressHit[0] = 0;  // Branch not predicted
        BranchPred.Predict[0] = 0;  // Predict branch not taken
        printf("<BTB hasn't PC value.>\n");
        return;
    }
    // Find PC in BTB
    for (BranchPred.BTBindex[0] = 0; BranchPred.BTBindex[0] < BTBMAX; BranchPred.BTBindex[0]++) {
        if (BranchPred.BTB[BranchPred.BTBindex[0]][0] == PCvalue) {  // PC found in BTB
            BranchPred.AddressHit[0] = 1;  // Branch predicted
            break;  // Send out predicted PC
        }
    }
    // PC not found in BTB
    if (BranchPred.BTBindex[0] == BTBMAX) {
        BranchPred.AddressHit[0] = 0;  // Branch not predicted
        BranchPred.Predict[0] = 0;  // Predict branch not taken
        printf("<BTB hasn't PC value.>\n");
        return;
    }
    // PC found in BTB
    if (BranchPred.BTB[BranchPred.BTBindex[0]][2] == 2 || BranchPred.BTB[BranchPred.BTBindex[0]][2] == 3)  {
        BranchPred.Predict[0] = 1;  // Predict branch taken
        printf("<BTB has PC value. Predict that branch is taken.>\n");
    }
    else {
        BranchPred.Predict[0] = 0;  // Predict branch not taken
        printf("<BTB has PC value. Predict that branch is not taken.>\n");
    }
    BranchPred.BTB[BranchPred.BTBindex[0]][3]++;
    return;
}

void UpdateBranchBuffer(bool Branch, bool PCBranch, uint32_t BranchAddr) {
    if (Branch) {  // beq, bne
        if (BranchPred.AddressHit[1]) {  // PC found in BTB
            if (PCBranch) {  // Branch taken
                printf("Branch taken, ");
                PBtaken(BranchPred.BTB[BranchPred.BTBindex[1]][2]);
                counting.takenBranch++;
                if (BranchPred.Predict[1]) {  // Predict branch taken, HIT
                    printf("predicted branch taken.\nBranch prediction HIT.\n");
                    counting.PredictHitCount++;
                }
                else {  // Predict branch not taken
                    printf("predicted branch not taken.\nBranch prediction not HIT. Flushing IF instruction.\n");
                    ctrlSig.IFFlush = 1;
                }
            }

            else {  // Branch not taken
                printf("Branch not taken, ");
                PBnottaken(BranchPred.BTB[BranchPred.BTBindex[1]][2]);
                counting.nottakenBranch++;
                if (BranchPred.Predict[1]) {  // Predict branch taken
                    printf("predicted branch taken.\nBranch prediction not HIT. Flushing IF instruction.\n");
                    ctrlSig.IFFlush = 1;
                }
                else {  // Predict branch not taken, HIT
                    printf("predicted branch not taken.\nBranch prediction HIT.\n");
                    counting.PredictHitCount++;
                }
            }
        }
        else {  // PC not found in BTB, Predicted PC + 4
            BranchBufferWrite(BranchPred.instPC[1], BranchAddr);
            printf("## Write PC to BTB ##\n");
            if (PCBranch) {  // Branch taken
                PBtaken(BranchPred.BTB[BranchPred.BTBindex[1]][2]);
                printf("Instruction is branch. FLushing IF instruction.\n");
                counting.takenBranch++;
                ctrlSig.IFFlush = 1;
            }
            else {  // Branch not taken
                counting.nottakenBranch++;
            }
        }
        if (ctrlSig.IFFlush) {  // Flushing IF instruction
            ifid[0].instruction = 0;
        }
    }
}

void BranchBufferWrite(uint32_t WritePC, uint32_t Address) {  // Write PC and BranchAddr to BTB
    if (BranchPred.BTBsize >= BTBMAX) {  // BTB has no space
        uint32_t min = BranchPred.BTB[0][3];
        int minindex;
        for (BranchPred.BTBindex[1] = 0; BranchPred.BTBindex[1] < BTBMAX; BranchPred.BTBindex[1]++) {
            if (min > BranchPred.BTB[BranchPred.BTBindex[1]][3]) {
                min = BranchPred.BTB[BranchPred.BTBindex[1]][3];
                minindex = BranchPred.BTBindex[1];
            }
        }
        // Substitute low frequency used branch
        BranchPred.BTB[minindex][0] = WritePC;
        BranchPred.BTB[minindex][1] = Address;
        BranchPred.BTB[minindex][2] = 1;
        BranchPred.BTB[minindex][3] = 0;
        BranchPred.BTBindex[1] = minindex;
        return;
    }
    BranchPred.BTB[BranchPred.BTBsize][0] = WritePC;
    BranchPred.BTB[BranchPred.BTBsize][1] = Address;
    BranchPred.BTBsize++;
    return;
}

void PBtaken(uint8_t Predbit) {  // Update prediction bits to taken
    if (Predbit == 1 || Predbit == 2 || Predbit == 3) {  // PB == 01 or 10 or 11
        printf("<Update PB to 3>\n");
        BranchPred.BTB[BranchPred.BTBindex[1]][2] =  3;  // PB = 11
    }
    else if (Predbit == 0) {  // PB == 00
        printf("<Update PB to 1>\n");
        BranchPred.BTB[BranchPred.BTBindex[1]][2] = 1;  // PB = 01
    }
    else {
        fprintf(stderr, "ERROR: Prediction bit is wrong.\n");
        exit(EXIT_FAILURE);
    }
    return;
}

void PBnottaken(uint8_t Predbit) {  // Update prediction bits to not taken
    if (Predbit == 0 || Predbit == 1 || Predbit == 2) {  // PB == 00 or 01 or 10
        printf("<Update PB to 0>\n");
        BranchPred.BTB[BranchPred.BTBindex[1]][2] =  0;  // PB = 00
    }
    else if (Predbit == 3) {  // PB == 11
        printf("<Update PB to 2>\n");
        BranchPred.BTB[BranchPred.BTBindex[1]][2] =  2;  // PB = 10
    }
    else {
        fprintf(stderr, "ERROR: Prediction bit is wrong.\n");
        exit(EXIT_FAILURE);
    }
    return;
}

// [ALU]
uint32_t ALU(uint32_t input1, uint32_t input2, char ALUSig) {
    uint32_t ALUresult = 0;
    switch (ALUSig) {
        case '+' :  // add
            ALUresult = input1 + input2;
            break;

        case '-' :  // subtract
            ALUresult = input1 - input2;
            break;

        case '&' :  // AND
            ALUresult = input1 & input2;
            break;

        case '|' :  // OR
            ALUresult = input1 | input2;
            break;

        case '~' :  // NOR
            ALUresult = ~(input1 | input2);
            break;
        
        case '<' :  // set less than signed
            if ( (input1 & 0x80000000) == (input2 & 0x80000000) ) {  // same sign
                if (input1 < input2) {
                    ALUresult = 1;
                }
                else {
                    ALUresult = 0;
                }
            }
            else if ( ((input1 & 0x80000000) == 0x80000000) && ((input2 & 0x80000000) == 0x00000000) ) {  // input1 < 0. input2 > 0
                ALUresult = 1;
            }
            else {
                ALUresult = 0;
            }
            break;

        case '>' :  // set less than unsigned
            if ((input1 & 0x7fffffff) < (input2 & 0x7fffffff)) {
                ALUresult = 1;
            }
            else {
                ALUresult = 0;
            }
            break;

        case '{' :  // sll
            ALUresult = input2 << input1;
            break;

        case '}' :  // srl
            ALUresult = input2 >> input1;
            break;

        case '\0' :  // nop
            break;

        default :
            fprintf(stderr, "ERROR: Instruction has wrong opcode or funct.\n");
            exit(EXIT_FAILURE);

    }
    if ( ((input1 & 0x80000000) == (input2 & 0x80000000)) && ((ALUresult & 0x80000000) != (input1 & 0x80000000)) ) {
        // same sign input = different sign output
        ALUctrlSig.ArthOvfl = 1;
    }
    return ALUresult;
}

// [MUX with 2 input]
uint32_t MUX(uint32_t input1, uint32_t input2, bool signal) {
    if (signal) {
        return input2;
    }
    else {
        return input1;
    }
}

// [MUX with 3 input]
uint32_t MUX_3(uint32_t input1, uint32_t input2, uint32_t input3, const bool signal[]) {
    if (signal[1] == 0 && signal[0] == 0) {
        return input1;
    }
    else if (signal[1] == 0 && signal[0] == 1) {
        return input2;
    }
    else {
        return input3;
    }
}

// [MUX with 4 input]
uint32_t MUX_4(uint32_t input1, uint32_t input2, uint32_t input3, uint32_t input4, const bool signal[]) {
    if (signal[1] == 0 && signal[0] == 0) {
        return input1;
    }
    else if (signal[1] == 0 && signal[0] == 1) {
        return input2;
    }
    else if (signal[1] == 1 && signal[0] == 0){
        return input3;
    }
    else {
        return input4;
    }
}

/*============================Control units============================*/

// (Control unit)
void CtrlUnit(uint8_t opcode, uint8_t funct) {
    memset(&ctrlSig, 0, sizeof(CONTROL_SIGNAL));
    switch (opcode) {  // considered don't care as 0 or not written
        case 0x0 :  // R-format
            counting.format = 'R';
            printf("name : R-format\n");
            ctrlSig.RegDst[1] = 0; ctrlSig.RegDst[0] = 1;  // Write register = rd
            ctrlSig.ALUSrc = 0;  // ALU input2 = rt
            ctrlSig.MemtoReg[1] = 0; ctrlSig.MemtoReg[0] = 0;  // Write data = ALU result
            ctrlSig.RegWrite = 1;  // GPR write enabled
            ctrlSig.MemRead = 0;  // Memory read disabled
            ctrlSig.MemWrite = 0;  // Memory write enabled
            ctrlSig.BNE = 0;  // opcode != bne
            ctrlSig.BEQ = 0;  // opcode != beq
            ctrlSig.ALUOp = '0';  // select operation according to funct
            ctrlSig.Jump[1] = 0; ctrlSig.Jump[0] = 0;
            if (funct == 0x08) {  // jr
                ctrlSig.RegWrite = 0;
                ctrlSig.Jump[1] = 1; ctrlSig.Jump[0] = 0;
            }
            else if (funct == 0x9) {  // jalr
                ctrlSig.MemtoReg[1] = 1; ctrlSig.MemtoReg[0] = 0;  // Write data = PC + 8
                ctrlSig.Jump[1] = 1; ctrlSig.Jump[0] = 0;
            }
            else if (funct == 0x00 || funct == 0x02) {  // sll, srl
                ctrlSig.Shift = 1;  // ALU input1 = shamt
            }
            break;

        case 0x8 :  // addi
            counting.format = 'I';
            printf("name : addi\n");
            ctrlSig.RegDst[1] = 0; ctrlSig.RegDst[0] = 0;
            ctrlSig.ALUSrc = 1;
            ctrlSig.MemtoReg[1] = 0; ctrlSig.MemtoReg[0] = 0;
            ctrlSig.RegWrite = 1;
            ctrlSig.MemRead = 0;
            ctrlSig.MemWrite = 0;
            ctrlSig.SignZero = 0;
            ctrlSig.BNE = 0;
            ctrlSig.BEQ = 0;
            ctrlSig.ALUOp = '+';
            ctrlSig.Jump[1] = 0; ctrlSig.Jump[0] = 0;
            break;

        case 0x9 :  // addiu
            counting.format = 'I';
            printf("name : addiu\n");
            ctrlSig.RegDst[1] = 0; ctrlSig.RegDst[0] = 0;
            ctrlSig.ALUSrc = 1;
            ctrlSig.MemtoReg[1] = 0; ctrlSig.MemtoReg[0] = 0;
            ctrlSig.RegWrite = 1;
            ctrlSig.MemRead = 0;
            ctrlSig.MemWrite = 0;
            ctrlSig.SignZero = 0;
            ctrlSig.BNE = 0;
            ctrlSig.BEQ = 0;
            ctrlSig.ALUOp = '+';
            ctrlSig.Jump[1] = 0; ctrlSig.Jump[0] = 0;
            break;

        case 0xc :  // andi
            counting.format = 'I';
            printf("name : andi\n");
            ctrlSig.RegDst[1] = 0; ctrlSig.RegDst[0] = 0;
            ctrlSig.ALUSrc = 1;
            ctrlSig.MemtoReg[1] = 0; ctrlSig.MemtoReg[0] = 0;
            ctrlSig.RegWrite = 1;
            ctrlSig.MemRead = 0;
            ctrlSig.MemWrite = 0;
            ctrlSig.SignZero = 1;
            ctrlSig.BNE = 0;
            ctrlSig.BEQ = 0;
            ctrlSig.ALUOp = '&';
            ctrlSig.Jump[1] = 0; ctrlSig.Jump[0] = 0;
            break;

        case 0x4 :  // beq
            counting.format = 'I';
            printf("name : beq\n");
            ctrlSig.ALUSrc = 0;
            ctrlSig.RegWrite = 0;
            ctrlSig.MemRead = 0;
            ctrlSig.MemWrite = 0;
            ctrlSig.BNE = 0;
            ctrlSig.BEQ = 1;
            ctrlSig.ALUOp = 'B';
            ctrlSig.Jump[1] = 0; ctrlSig.Jump[0] = 0;
            break;

        case 0x5 :  // bne
            counting.format = 'I';
            printf("name : bne\n");
            ctrlSig.ALUSrc = 0;
            ctrlSig.RegWrite = 0;
            ctrlSig.MemRead = 0;
            ctrlSig.MemWrite = 0;
            ctrlSig.BNE = 1;
            ctrlSig.BEQ = 0;
            ctrlSig.ALUOp = 'B';
            ctrlSig.Jump[1] = 0; ctrlSig.Jump[0] = 0;
            break;

        case 0x2 :  // j
            counting.format = 'J';
            printf("name : j\n");
            ctrlSig.RegWrite = 0;
            ctrlSig.MemRead = 0;
            ctrlSig.MemWrite = 0;
            ctrlSig.BNE = 0;
            ctrlSig.BEQ = 0;
            ctrlSig.ALUOp = '+';
            ctrlSig.Jump[1] = 0; ctrlSig.Jump[0] = 1;
            break;

        case 0x3 :  // jal
            counting.format = 'J';
            printf("name : jal\n");
            counting.format = 'J';
            ctrlSig.RegWrite = 1;
            ctrlSig.RegDst[1] = 1; ctrlSig.RegDst[0] = 0;
            ctrlSig.MemtoReg[1] = 1; ctrlSig.MemtoReg[0] = 0;
            ctrlSig.ALUOp = '+';
            ctrlSig.Jump[1] = 0; ctrlSig.Jump[0] = 1;
            break;
        
        case 0xf :  // lui
            counting.format = 'I';
            printf("name : lui\n");
            counting.Icount++;
            ctrlSig.RegDst[1] = 0; ctrlSig.RegDst[0] = 0;
            ctrlSig.ALUSrc = 1;
            ctrlSig.MemtoReg[1] = 1; ctrlSig.MemtoReg[0] = 1;
            ctrlSig.RegWrite = 1;
            ctrlSig.MemRead = 0;
            ctrlSig.MemWrite = 0;
            ctrlSig.BNE = 0;
            ctrlSig.BEQ = 0;
            ctrlSig.ALUOp = '+';
            ctrlSig.Jump[1] = 0; ctrlSig.Jump[0] = 0;
            break;

        case 0x23 :  // lw
            counting.format = 'I';
            printf("name : lw\n");
            counting.Icount++;
            ctrlSig.RegDst[1] = 0; ctrlSig.RegDst[0] = 0;
            ctrlSig.ALUSrc = 1;
            ctrlSig.MemtoReg[1] = 0; ctrlSig.MemtoReg[0] = 1;
            ctrlSig.RegWrite = 1;
            ctrlSig.MemRead = 1;
            ctrlSig.MemWrite = 0;
            ctrlSig.SignZero = 0;
            ctrlSig.BNE = 0;
            ctrlSig.BEQ = 0;
            ctrlSig.ALUOp = '+';
            ctrlSig.Jump[1] = 0; ctrlSig.Jump[0] = 0;
            break;

        case 0xd :  // ori
            counting.format = 'I';
            printf("name : ori\n");
            counting.Icount++;
            ctrlSig.RegDst[1] = 0; ctrlSig.RegDst[0] = 0;
            ctrlSig.ALUSrc = 1;
            ctrlSig.MemtoReg[1] = 0; ctrlSig.MemtoReg[0] = 0;
            ctrlSig.RegWrite = 1;
            ctrlSig.MemRead = 0;
            ctrlSig.MemWrite = 0;
            ctrlSig.SignZero = 1;
            ctrlSig.BNE = 0;
            ctrlSig.BEQ = 0;
            ctrlSig.ALUOp = '|';
            ctrlSig.Jump[1] = 0; ctrlSig.Jump[0] = 0;
            break;

        case 0xa :  // slti
            counting.format = 'I';
            printf("name : slti\n");
            counting.Icount++;
            ctrlSig.RegDst[1] = 0; ctrlSig.RegDst[0] = 0;
            ctrlSig.ALUSrc = 1;
            ctrlSig.MemtoReg[1] = 0; ctrlSig.MemtoReg[0] = 0;
            ctrlSig.RegWrite = 1;
            ctrlSig.MemRead = 0;
            ctrlSig.MemWrite = 0;
            ctrlSig.SignZero = 0;
            ctrlSig.BNE = 0;
            ctrlSig.BEQ = 0;
            ctrlSig.ALUOp = '<';
            ctrlSig.Jump[1] = 0; ctrlSig.Jump[0] = 0;
            break;

        case 0xb :  // sltiu
            counting.format = 'I';
            printf("name : sltiu\n");
            counting.Icount++;
            ctrlSig.RegDst[1] = 0; ctrlSig.RegDst[0] = 0;
            ctrlSig.ALUSrc = 1;
            ctrlSig.MemtoReg[1] = 0; ctrlSig.MemtoReg[0] = 0;
            ctrlSig.RegWrite = 1;
            ctrlSig.MemRead = 0;
            ctrlSig.MemWrite = 0;
            ctrlSig.SignZero = 0;
            ctrlSig.BNE = 0;
            ctrlSig.BEQ = 0;
            ctrlSig.ALUOp = '>';
            ctrlSig.Jump[1] = 0; ctrlSig.Jump[0] = 0;
            break;

        case 0x2B :  // sw
            counting.format = 'I';
            printf("name : sw\n");
            counting.Icount++;
            ctrlSig.ALUSrc = 1;
            ctrlSig.RegWrite = 0;
            ctrlSig.MemRead = 0;
            ctrlSig.MemWrite = 1;
            ctrlSig.SignZero = 0;
            ctrlSig.BNE = 0;
            ctrlSig.BEQ = 0;
            ctrlSig.ALUOp = '+';
            ctrlSig.Jump[1] = 0; ctrlSig.Jump[0] = 0;
            break;

        default :
            fprintf(stderr, "ERROR: Instruction has wrong opcode.\n");
            exit(EXIT_FAILURE);
    }
    return;
}

// (ALU control unit)
void ALUCtrlUnit(uint8_t funct, char ALUOp) {
    memset(&ALUctrlSig, 0, sizeof(ALU_CONTROL_SIGNAL));
    switch (ALUOp) {  // 0, +, &, B, |, <
        case '0' :  // select operation according to funct
            ALUctrlSig.ALUSig = Rformat(funct);
            return;

        case '+' :
            ALUctrlSig.ALUSig = '+';
            return;

        case '&' :
            ALUctrlSig.ALUSig = '&';
            return;

        case 'B' :  // select bcond generation function
            ALUctrlSig.ALUSig = '-';
            return;

        case '|' :
            ALUctrlSig.ALUSig = '|';
            return;

        case '<' :
            ALUctrlSig.ALUSig = '<';
            return;

        case '\0' :  // nop
            return;

        default :
            fprintf(stderr, "ERROR: Instruction has wrong funct.\n");
            exit(EXIT_FAILURE);
    }
}

// (Fowarding Unit)
void ForwardUnit(uint8_t IDEXrt, uint8_t IDEXrs, uint8_t EXMEMWritereg, uint8_t MEMWBWritereg, bool EXMEMRegWrite, bool MEMWBRegWrite) {
    memset(&fwrdSig, 0, sizeof(FORWARD_SIGNAL));
    // EX hazard
    if (EXMEMRegWrite && (EXMEMWritereg != 0) && (EXMEMWritereg == IDEXrs)) {
        fwrdSig.ForwardA[1] = 1; fwrdSig.ForwardA[0] = 0;  // ForwardA = 10
        printf("<ALU input1 forwarded from EX/MEM pipeline>\n");
    }
    if (EXMEMRegWrite && (EXMEMWritereg != 0) && (EXMEMWritereg == IDEXrt)) {
        fwrdSig.ForwardB[1] = 1; fwrdSig.ForwardB[0] = 0;  // ForwardB = 10
        printf("<ALU input2 forwarded from EX/MEM pipeline>\n");
    }

    // MEM hazard
    if (MEMWBRegWrite && (MEMWBWritereg != 0) &&
        !( EXMEMRegWrite && (EXMEMWritereg != 0) && (EXMEMWritereg == IDEXrs) ) &&
        (MEMWBWritereg == IDEXrs)) {
        fwrdSig.ForwardA[1] = 0; fwrdSig.ForwardA[0] = 1;  // ForwardA = 01
        printf("<ALU input1 forwarded from MEM/WB pipeline>\n");
    }
    if (MEMWBRegWrite && (MEMWBWritereg != 0) &&
        !( EXMEMRegWrite && (EXMEMWritereg != 0) && (EXMEMWritereg == IDEXrt) ) &&
        (MEMWBWritereg == IDEXrt)) {
        fwrdSig.ForwardB[1] = 0; fwrdSig.ForwardB[0] = 1;  // ForwardB = 01
        printf("<ALU input2 forwarded from MEM/WB pipeline>\n");
    }
    return;
}

void IDForwardUnit(uint8_t IFIDrt, uint8_t IFIDrs, uint8_t IDEXWritereg, uint8_t EXMEMWritereg, uint8_t MEMWBWritereg,
                   bool IDEXRegWrite, bool EXMEMRegWrite, bool MEMWBRegWrite) {
    memset(&idfwrdSig, 0, sizeof(ID_FORWARD_SIGNAL));
    // ID hazard
    if (IDEXRegWrite && (IDEXWritereg != 0) && (IDEXWritereg == IFIDrs)) {
        idfwrdSig.IDForwardA[1] = 1; idfwrdSig.IDForwardA[1] = 1;  // IDForwardA = 11
        printf("<Register Read data1 forwarded from ID/EX pipeline>\n");
    }
    if (IDEXRegWrite && (IDEXWritereg != 0) && (IDEXWritereg == IFIDrt)) {
        idfwrdSig.IDForwardB[1] = 1; idfwrdSig.IDForwardB[1] = 1;  // IDForwardB = 11
        printf("<Register Read data2 forwarded from ID/EX pipeline>\n");
    }

    // EX hazard
    if (EXMEMRegWrite && (EXMEMWritereg != 0) &&
        !( IDEXRegWrite && (IDEXWritereg != 0) && (IDEXWritereg == IFIDrs) ) &&
        (EXMEMWritereg == IFIDrs)) {
        idfwrdSig.IDForwardA[1] = 1; idfwrdSig.IDForwardA[0] = 0;  // IDForwardA = 10
        printf("<Register Read data1 forwarded from EX/MEM pipeline>\n");
    }
    if (EXMEMRegWrite && (EXMEMWritereg != 0) &&
        !( IDEXRegWrite && (IDEXWritereg != 0) && (IDEXWritereg == IFIDrt) ) &&
        (EXMEMWritereg == IFIDrt)) {
        idfwrdSig.IDForwardB[1] = 1; idfwrdSig.IDForwardB[0] = 0;  // IDForwardB = 10
        printf("<Register Read data2 forwarded from EX/MEM pipeline>\n");
    }

    // MEM hazard
    if (MEMWBRegWrite && (MEMWBWritereg != 0) &&
        !( EXMEMRegWrite && (EXMEMWritereg != 0) &&
        !( IDEXRegWrite && (IDEXWritereg != 0) && (IDEXWritereg == IFIDrs) ) && (EXMEMWritereg == IFIDrs) )
        && (MEMWBWritereg == IFIDrs)) {
        idfwrdSig.IDForwardA[1] = 0; idfwrdSig.IDForwardA[0] = 1;  // IDForwardA = 01
        printf("<Register Read data1 forwarded from MEM/WB pipeline>\n");
    }
    if (MEMWBRegWrite && (MEMWBWritereg != 0) &&
        !( EXMEMRegWrite && (EXMEMWritereg != 0) &&
           !( IDEXRegWrite && (IDEXWritereg != 0) && (IDEXWritereg == IFIDrt) ) && (EXMEMWritereg == IFIDrt) )
        && (MEMWBWritereg == IFIDrt)) {
        idfwrdSig.IDForwardB[1] = 0; idfwrdSig.IDForwardB[0] = 1;  // IDForwardB = 01
        printf("<Register Read data2 forwarded from MEM/WB pipeline>\n");
    }
}

// (Hazard detection unit)
void HazardDetectUnit(uint8_t IFIDrs, uint8_t IFIDrt, uint8_t IDEXrt, uint8_t IDEXWritereg, uint8_t EXMEMWritereg,
                    bool IDEXMemRead, bool IDEXRegWrite, bool EXMEMMemRead, bool BEQ, bool BNE, bool Jump) {
    memset(&hzrddetectSig, 0, sizeof(HAZARD_DETECTION_SIGNAL));
    if (IDEXMemRead && ((IDEXrt == IFIDrs) || (IDEXrt == IFIDrt))) {
        printf("<<Load-use hazard detected. Adding nop.>>\n");
        hzrddetectSig.PCnotWrite = 1;
        hzrddetectSig.IFIDnotWrite = 1;
        memset(&ctrlSig, 0, sizeof(CONTROL_SIGNAL));
    }
    if (IDEXRegWrite && (BEQ | BNE | Jump) && ((IDEXWritereg == IFIDrs) || (IDEXWritereg == IFIDrt))) {
        printf("<<Register Read data hazard detected from IDEX. Adding nop.>>\n");
        hzrddetectSig.PCnotWrite = 1;
        hzrddetectSig.IFIDnotWrite = 1;
        hzrddetectSig.BTBnotWrite = 1;
        memset(&ctrlSig, 0, sizeof(CONTROL_SIGNAL));
    }
    if (EXMEMMemRead && (BEQ | BNE | Jump) && ((EXMEMWritereg == IFIDrs) || (EXMEMWritereg == IFIDrt))) {
        printf("<<Register Read data hazard detected from EXMEM. Adding nop.>>\n");
        hzrddetectSig.PCnotWrite = 1;
        hzrddetectSig.IFIDnotWrite = 1;
        hzrddetectSig.BTBnotWrite = 1;
        memset(&ctrlSig, 0, sizeof(CONTROL_SIGNAL));
    }
}


/*============================Select ALU operation============================*/

char Rformat(uint8_t funct) {  // select ALU operation by funct (R-format)
    switch (funct) {
        case 0x20 :  // add
            printf ("name : add\n");
            return '+';

        case 0x21 :  // addu
            printf("name : addu\n");
            return '+';

        case 0x24 :  // and
            printf("name : and\n");
            return '&';

        case 0x08 :  // jr
            printf("name : jr\n");
            return '+';
        
        case 0x9 :  // jalr
            printf("name : jalr\n");
            return '+';

        case 0x27 :  // nor
            printf("name : nor\n");
            return '~';

        case 0x25 :  // or
            printf("name : or\n");
            return '|';

        case 0x2a :  // slt
            printf("name : slt\n");
            return '<';

        case 0x2b :  // sltu
            printf("name : sltu\n");
            return '>';

        case 0x00 :  // sll
            printf("name : sll\n");
            return '{';

        case 0x02 :  // srl
            printf("name : srl\n");
            return '}';

        case 0x22 :  // sub
            printf("name : sub\n");
            return '-';

        case 0x23 :  // subu
            printf("name : subu\n");
            return '-';

        default:
            fprintf(stderr, "ERROR: Instruction has wrong funct.\n");
            exit(EXIT_FAILURE);
    }
}

/*============================Exception============================*/

void OverflowException() {  // check overflow in signed instruction
    fprintf(stderr, "ERROR: Arithmetic overflow occured.\n");
    exit(EXIT_FAILURE);
}
