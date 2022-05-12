#include "header.h"

// from main.c
extern IFID ifid[2];
extern IDEX idex[2];
extern EXMEM exmem[2];
extern MEMWB memwb[2];
extern uint32_t Memory[0x400000];
extern uint32_t PC;
extern uint32_t R[32];
extern COUNTING counting;
extern BRANCH_PREDICT BranchPred;

// from Stages.c
extern CONTROL_SIGNAL ctrlSig;
extern ALU_CONTROL_SIGNAL ALUctrlSig;

FORWARD_SIGNAL fwrdSig;
ID_FORWARD_SIGNAL idfwrdSig;
extern HAZARD_DETECTION_SIGNAL hzrddetectSig;



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
void RegsWrite(uint8_t Writereg, uint32_t Writedata) {
    if (memwb[1].RegWrite) {  // RegWrite asserted
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
uint32_t DataMem(uint32_t Addr, uint32_t Writedata) {
    uint32_t Readdata = 0;
    if (exmem[1].MemRead == 1 && exmem[1].MemWrite == 0) {  // MemRead asserted, MemWrite De-asserted
        if (Addr > 0x1000000) {  // loading outside of memory
            fprintf(stderr, "ERROR: Accessing outside of memory\n");
            exit(EXIT_FAILURE);
        }
        Readdata = Memory[Addr / 4];  // Memory read port return load value
        printf("Memory[0x%08x] load -> 0x%x (%d)\n", Addr, Memory[Addr / 4], Memory[Addr / 4]);
        counting.Memcount++;
    }
    else if (exmem[1].MemRead == 0 && exmem[1].MemWrite == 1) {  // MemRead De-asserted, MemWrite asserted
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
        BranchPred.Predict[0] = 0;  // Branch not predicted
        BranchPred.AddressHit[0] = 0;  // Predict branch not taken
        printf("<BTB hasn't PC value.>\n");
        return;
    }
    // Find PC in BTB
    for (BranchPred.BTBindex[0] = 0; BranchPred.BTBindex[0] < BTBMAX; BranchPred.BTBindex[0]++) {
        if (BranchPred.BTB[BranchPred.BTBindex[0]][0] == PCvalue) {  // PC found in BTB
            BranchPred.Predict[0] = 1;  // Branch predicted
            break;  // Send out predicted PC
        }
    }
    // PC not found in BTB
    if (BranchPred.BTBindex[0] == BTBMAX) {
        BranchPred.Predict[0] = 0;  // Branch not predicted
        BranchPred.AddressHit[0] = 0;  // Predict branch not taken
        printf("<BTB hasn't PC value.>\n");
        return;
    }
    // PC found in BTB
    if (BranchPred.BTB[BranchPred.BTBindex[0]][2] == 2 || BranchPred.BTB[BranchPred.BTBindex[0]][2] == 3)  {
        BranchPred.AddressHit[0] = 1;  // Predict branch taken
        printf("<BTB has PC value. Predict that branch is taken.>\n");
    }
    else {
        BranchPred.AddressHit[0] = 0;  // Predict branch not taken
        printf("<BTB has PC value. Predict that branch is not taken.>\n");
    }
    BranchPred.BTB[BranchPred.BTBindex[0]][3]++;
    return;
}

void BranchBufferWrite(uint32_t WritePC, uint32_t Address) {  // Write PC and BranchAddr to BTB
    // brjp == 0) beq or bne, 1) j or jal
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

void UpdatePredictBits(bool PCBranch) {  // Update predict bits
    uint8_t PB = BranchPred.BTB[BranchPred.BTBindex[1]][2];  // Prediction Bits
    if (PCBranch) {  // branch taken
        BranchPred.BTB[BranchPred.BTBindex[1]][2] = PBtaken(PB);
    }
    else {  // branch not taken
        BranchPred.BTB[BranchPred.BTBindex[1]][2] = PBnottaken(PB);
    }
    return;
}

// [ALU]
uint32_t ALU(uint32_t input1, uint32_t input2) {
    uint32_t ALUresult = 0;
    switch (ALUctrlSig.ALUSig) {
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
            counting.Rcount++;
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
            printf("name : addi\n");
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
            ctrlSig.ALUOp = '+';
            ctrlSig.Jump[1] = 0; ctrlSig.Jump[0] = 0;
            break;

        case 0x9 :  // addiu
            printf("name : addiu\n");
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
            ctrlSig.ALUOp = '+';
            ctrlSig.Jump[1] = 0; ctrlSig.Jump[0] = 0;
            break;

        case 0xc :  // andi
            printf("name : andi\n");
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
            ctrlSig.ALUOp = '&';
            ctrlSig.Jump[1] = 0; ctrlSig.Jump[0] = 0;
            break;

        case 0x4 :  // beq
            printf("name : beq\n");
            counting.Icount++;
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
            printf("name : bne\n");
            counting.Icount++;
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
            printf("name : j\n");
            counting.Jcount++;
            ctrlSig.RegWrite = 0;
            ctrlSig.MemRead = 0;
            ctrlSig.MemWrite = 0;
            ctrlSig.BNE = 0;
            ctrlSig.BEQ = 0;
            ctrlSig.ALUOp = '+';
            ctrlSig.Jump[1] = 0; ctrlSig.Jump[0] = 1;
            break;

        case 0x3 :  // jal
            printf("name : jal\n");
            counting.Jcount++;
            ctrlSig.RegWrite = 1;
            ctrlSig.RegDst[1] = 1; ctrlSig.RegDst[0] = 0;
            ctrlSig.MemtoReg[1] = 1; ctrlSig.MemtoReg[0] = 0;
            ctrlSig.ALUOp = '+';
            ctrlSig.Jump[1] = 0; ctrlSig.Jump[0] = 1;
            break;
        
        case 0xf :  // lui
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
void ALUCtrlUnit(uint8_t funct) {
    memset(&ALUctrlSig, 0, sizeof(ALU_CONTROL_SIGNAL));
    switch (idex[1].ALUOp) {  // 0, +, &, B, |, <
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
            fprintf(stderr, "ERROR: Instruction has wrong funct\n");
            exit(EXIT_FAILURE);
    }
}

// (Fowarding Unit)
void ForwardUnit(uint8_t IDEXrt, uint8_t IDEXrs, uint8_t EXMEMWritereg, uint8_t MEMWBWritereg) {
    memset(&fwrdSig, 0, sizeof(FORWARD_SIGNAL));
    // EX hazard
    if (exmem[1].RegWrite && (EXMEMWritereg != 0) && (EXMEMWritereg == IDEXrs)) {
        fwrdSig.ForwardA[1] = 1; fwrdSig.ForwardA[0] = 0;  // ForwardA = 10
        printf("<ALU input1 forwarded from EX/MEM pipeline>\n");
    }
    if (exmem[1].RegWrite && (EXMEMWritereg != 0) && (EXMEMWritereg == IDEXrt)) {
        fwrdSig.ForwardB[1] = 1; fwrdSig.ForwardB[0] = 0;  // ForwardB = 10
        printf("<ALU input2 forwarded from EX/MEM pipeline>\n");
    }

    // MEM hazard
    if (memwb[1].RegWrite && (MEMWBWritereg != 0) &&
        !( exmem[1].RegWrite && (EXMEMWritereg != 0) && (EXMEMWritereg == IDEXrs) ) &&
        (MEMWBWritereg == IDEXrs)) {
        fwrdSig.ForwardA[1] = 0; fwrdSig.ForwardA[0] = 1;  // ForwardA = 01
        printf("<ALU input1 forwarded from MEM/WB pipeline>\n");
    }
    if (memwb[1].RegWrite && (MEMWBWritereg != 0) &&
        !( exmem[1].RegWrite && (EXMEMWritereg != 0) && (EXMEMWritereg == IDEXrt) ) &&
        (MEMWBWritereg == IDEXrt)) {
        fwrdSig.ForwardB[1] = 0; fwrdSig.ForwardB[0] = 1;  // ForwardB = 01
        printf("<ALU input2 forwarded from MEM/WB pipeline>\n");
    }
    return;
}

void IDForwardUnit(uint8_t IFIDrt, uint8_t IFIDrs, uint8_t IDEXWritereg, uint8_t EXMEMWritereg, uint8_t MEMWBWritereg) {
    memset(&idfwrdSig, 0, sizeof(ID_FORWARD_SIGNAL));
    // ID hazard
    if (idex[1].RegWrite && (IDEXWritereg != 0) && (IDEXWritereg == IFIDrs)) {
        idfwrdSig.IDForwardA[1] = 1; idfwrdSig.IDForwardA[1] = 1;  // IDForwardA = 11
        printf("<Register Read data1 forwarded from ID/EX pipeline>\n");
    }
    if (idex[1].RegWrite && (IDEXWritereg != 0) && (IDEXWritereg == IFIDrt)) {
        idfwrdSig.IDForwardB[1] = 1; idfwrdSig.IDForwardB[1] = 1;  // IDForwardB = 11
        printf("<Register Read data2 forwarded from ID/EX pipeline>\n");
    }

    // EX hazard
    if (exmem[1].RegWrite && (EXMEMWritereg != 0) &&
        !( idex[1].RegWrite && (IDEXWritereg != 0) && (IDEXWritereg == IFIDrs) ) &&
        (EXMEMWritereg == IFIDrs)) {
        idfwrdSig.IDForwardA[1] = 1; idfwrdSig.IDForwardA[0] = 0;  // IDForwardA = 10
        printf("<Register Read data1 forwarded from EX/MEM pipeline>\n");
    }
    if (exmem[1].RegWrite && (EXMEMWritereg != 0) &&
        !( idex[1].RegWrite && (IDEXWritereg != 0) && (IDEXWritereg == IFIDrt) ) &&
        (EXMEMWritereg == IFIDrt)) {
        idfwrdSig.IDForwardB[1] = 1; idfwrdSig.IDForwardB[0] = 0;  // IDForwardB = 10
        printf("<Register Read data2 forwarded from EX/MEM pipeline>\n");
    }

    // MEM hazard
    if (memwb[1].RegWrite && (MEMWBWritereg != 0) &&
        !( exmem[1].RegWrite && (EXMEMWritereg != 0) &&
        !( idex[1].RegWrite && (IDEXWritereg != 0) && (IDEXWritereg == IFIDrs) ) && (EXMEMWritereg == IFIDrs) )
        && (MEMWBWritereg == IFIDrs)) {
        idfwrdSig.IDForwardA[1] = 0; idfwrdSig.IDForwardA[0] = 1;  // IDForwardA = 01
        printf("<Register Read data1 forwarded from MEM/WB pipeline>\n");
    }
    if (memwb[1].RegWrite && (MEMWBWritereg != 0) &&
        !( exmem[1].RegWrite && (EXMEMWritereg != 0) &&
           !( idex[1].RegWrite && (IDEXWritereg != 0) && (IDEXWritereg == IFIDrt) ) && (EXMEMWritereg == IFIDrt) )
        && (MEMWBWritereg == IFIDrt)) {
        idfwrdSig.IDForwardB[1] = 0; idfwrdSig.IDForwardB[0] = 1;  // IDForwardB = 01
        printf("<Register Read data2 forwarded from MEM/WB pipeline>\n");
    }
}

// (Hazard detection unit)
void HazardDetectUnit(uint8_t IFIDrs, uint8_t IFIDrt, uint8_t IDEXrt, uint8_t IDEXWritereg) {
    memset(&hzrddetectSig, 0, sizeof(HAZARD_DETECTION_SIGNAL));
    if (idex[1].MemRead && ((IDEXrt == IFIDrs) || (IDEXrt == IFIDrt))) {
        printf("<<Load-use hazard detected. Adding nop.>>\n");
        hzrddetectSig.PCnotWrite = 1;
        hzrddetectSig.IFIDnotWrite = 1;
        memset(&ctrlSig, 0, sizeof(CONTROL_SIGNAL));
    }
    if (idex[1].RegWrite && (ctrlSig.BEQ | ctrlSig.BNE | ctrlSig.Jump[1]) && ((IDEXWritereg == IFIDrs) || (IDEXWritereg == IFIDrt))) {
        printf("<<Register Read data hazard detected. Adding nop.>>\n");
        hzrddetectSig.PCnotWrite = 1;
        hzrddetectSig.IFIDnotWrite = 1;
        hzrddetectSig.BTBnotWrite = 1;
        memset(&ctrlSig, 0, sizeof(CONTROL_SIGNAL));
    }
}


/*============================Select ALU operation============================*/

char Rformat(uint8_t funct) {  // select ALU operation by funct (R-format)
    printf("name : ");
    switch (funct) {
        case 0x20 :  // add
            printf ("add\n");
            return '+';

        case 0x21 :  // addu
            printf("addu\n");
            return '+';

        case 0x24 :  // and
            printf("and\n");
            return '&';

        case 0x08 :  // jr
            printf("jr\n");
            return '+';
        
        case 0x9 :  // jalr
            printf("jalr\n");
            return '+';

        case 0x27 :  // nor
            printf("nor\n");
            return '~';

        case 0x25 :  // or
            printf("or\n");
            return '|';

        case 0x2a :  // slt
            printf("slt\n");
            return '<';

        case 0x2b :  // sltu
            printf("sltu\n");
            return '>';

        case 0x00 :  // sll
            printf("sll\n");
            return '{';

        case 0x02 :  // srl
            printf("srl\n");
            return '}';

        case 0x22 :  // sub
            printf("sub\n");
            return '-';

        case 0x23 :  // subu
            printf("subu\n");
            return '-';

        default:
            fprintf(stderr, "ERROR: Instruction has wrong funct\n");
            exit(EXIT_FAILURE);
    }
}

/*============================Update prediction bits============================*/

uint8_t PBtaken(uint8_t Predbit) {
    if (Predbit == 1 || Predbit == 2 || Predbit == 3) {  // PB == 01 or 10 or 11
        printf("<Update PB to 3>\n");
        return 3;  // PB = 11
    }
    else if (Predbit == 0) {  // PB == 00
        printf("<Update PB to 1>\n");
        return 1;  // PB = 01
    }
}

uint8_t PBnottaken(uint8_t Predbit) {
    if (Predbit == 0 || Predbit == 1 || Predbit == 2) {  // PB == 00 or 01 or 10
        printf("<Update PB to 1>\n");
        return 0;  // PB = 00
    }
    else if (Predbit == 3) {  // PB == 11
        printf("<Update PB to 2>\n");
        return 2;  // PB = 10
    }
}

/*============================Exception============================*/

// Overflow exception
void OverflowException() {  // check overflow in signed instruction
    fprintf(stderr, "ERROR: Arithmetic overflow occured\n");
    exit(EXIT_FAILURE);
}
