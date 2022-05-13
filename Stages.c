//
// Created by SNMac on 2022/05/09.
//

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "Stages.h"
#include "main.h"
#include "Units.h"
#include "Debug.h"

uint32_t MemtoRegMUX;

// from main.c
extern uint32_t Memory[0x400000];
extern uint32_t R[32];
extern COUNTING counting;

// from Units.c
extern PROGRAM_COUNTER PC;
extern IFID ifid[2];
extern IDEX idex[2];
extern EXMEM exmem[2];
extern MEMWB memwb[2];
extern CONTROL_SIGNAL ctrlSig;
extern ALU_CONTROL_SIGNAL ALUctrlSig;
extern BRANCH_PREDICT BranchPred;
extern FORWARD_SIGNAL fwrdSig;
extern HAZARD_DETECTION_SIGNAL hzrddetectSig;
extern ID_FORWARD_SIGNAL idfwrdSig;

// from Debug.h
extern DEBUGID debugid[2];
extern DEBUGEX debugex[2];
extern DEBUGMEM debugmem[2];
extern DEBUGWB debugwb[2];

/*============================Stages============================*/

// Instruction Fetch
void IF(void) {
    printf("\n<<<<<<<<<<<<<<<<<<<<<IF>>>>>>>>>>>>>>>>>>>>>\n");
    if (PC.PC == 0xffffffff) {
        ifid[0].valid = 0;
        PC.valid = 0;
        printf("End of program\n");
        printf("Wait for finishing other stage\n");
        printf("**********************************************\n");
        return;
    }

    // Fetch instruction
    uint32_t instruction = InstMem(PC.PC);

    // Print information of IF
    printf("PC : 0x%08x\n", PC.PC);
    printf("Fetch instruction : 0x%08x\n", instruction);

    // Check if PC is branch instruction
    CheckBranch(PC.PC);

    // Save data to pipeline
    if (!hzrddetectSig.IFIDnotWrite){
        ifid[0].instruction = instruction; ifid[0].PCadd4 = PC.PC + 4;

        // For visible state
        debugid[0].IDPC = PC.PC; debugid[0].IDinst = instruction;
    }

    printf("**********************************************\n");

    return;
}

// Instruction Decode
void ID(void) {
    idex[0].valid = ifid[1].valid;
    printf("\n<<<<<<<<<<<<<<<<<<<<<ID>>>>>>>>>>>>>>>>>>>>>\n");

    INSTRUCTION inst;
    inst.address = ifid[1].instruction & 0x3ffffff;  // for J-format
    inst.opcode = (ifid[1].instruction & 0xfc000000) >> 26;
    inst.rs = (ifid[1].instruction & 0x03e00000) >> 21;
    inst.rt = (ifid[1].instruction & 0x001f0000) >> 16;
    inst.rd = (ifid[1].instruction & 0x0000f800) >> 11;
    inst.shamt = (ifid[1].instruction & 0x000007c0) >> 6;  // for sll, srl
    inst.imm = ifid[1].instruction & 0x0000ffff;  // for I-format
    inst.funct = inst.imm & 0x003f;  // for R-format

    // Set control signals
    CtrlUnit(inst.opcode, inst.funct);
    uint8_t IDEXWritereg = MUX_3(idex[1].rt, idex[1].rd, 31, idex[1].RegDst);

    // Load-use, branch hazard detect
    HazardDetectUnit(inst.rs, inst.rt, idex[1].rt, IDEXWritereg,
                     idex[1].MemRead, idex[1].RegWrite, ctrlSig.BEQ, ctrlSig.BNE, ctrlSig.Jump[1]);

    // Avoid ID-WB hazard
    MemtoRegMUX = MUX_4(memwb[1].ALUresult, memwb[1].Readdata, memwb[1].PCadd8, memwb[1].upperimm, memwb[1].MemtoReg);
    RegsWrite(memwb[1].Writereg, MemtoRegMUX, memwb[1].RegWrite);

    // Register fetch
    uint32_t* Regs_return = RegsRead(inst.rs, inst.rt);

    // Check branch forwarding
    IDForwardUnit(inst.rt, inst.rs, IDEXWritereg,
                      exmem[1].Writereg, memwb[1].Writereg);
    uint32_t IDForwardAMUX= MUX_4(Regs_return[0], MemtoRegMUX ,exmem[1].ALUresult, idex[1].Rrs, idfwrdSig.IDForwardA);
    uint32_t IDForwardBMUX= MUX_4(Regs_return[1], MemtoRegMUX ,exmem[1].ALUresult, idex[1].Rrt, idfwrdSig.IDForwardB);

    // Comparate two operands
    bool Equal = 0;
    if (IDForwardAMUX == IDForwardBMUX) {
        Equal = 1;
    }
    printf("IDForwardAMUX = %u\nIDForwardBMUX = %u\n", IDForwardAMUX, IDForwardBMUX);
    bool Branch = ctrlSig.BEQ | ctrlSig.BNE;
    bool PCBranch = (ctrlSig.BNE & !Equal) | (ctrlSig.BEQ & Equal);

    // Extending immediate
    int32_t signimm = (int16_t) inst.imm;
    uint32_t zeroimm = (uint16_t) inst.imm;
    uint32_t extimm = MUX(signimm, zeroimm, ctrlSig.SignZero);

    // Address calculate
    uint32_t BranchAddr = ifid[1].PCadd4 + (signimm << 2);
    uint32_t JumpAddr = (ifid[1].PCadd4 & 0xf0000000) | (inst.address << 2);

    // Update branch result to BTB
    UpdateBranchBuffer(Branch, PCBranch, BranchAddr);

    // Select PC address
    bool PCtarget = BranchPred.AddressHit[0] & BranchPred.Predict[0];
    bool BranchHit = (BranchPred.Predict[1] == PCBranch) ? 1 : 0;  // !(BranchPred.Predict[1] ^ PCBranch);
    bool IDBrPC = !BranchPred.Predict[1] & PCBranch;
    uint32_t IFIDPCMUX = MUX(ifid[1].PCadd4, PC.PC + 4, BranchHit);
    uint32_t PCBranchMUX = MUX(IFIDPCMUX, BranchAddr, IDBrPC);
    uint32_t JumpMUX = MUX_3(PCBranchMUX, JumpAddr, IDForwardAMUX, ctrlSig.Jump);
    uint32_t PredictMUX = MUX(JumpMUX, BranchPred.BTB[BranchPred.BTBindex[0]][1], PCtarget);
    if (PC.valid & !(hzrddetectSig.PCnotWrite)) {  // PC write disabled
        PC.PC = PredictMUX;
    }

    if (!(ifid[1].valid)) {  // Pipeline is invalid
        printf("IF/ID pipeline is invalid\n");
        printf("**********************************************\n");
        return;
    }

    printf("Processing PC : 0x%08x\n", debugid[1].IDPC);

    // Print information of ID
    printf("Decode instruction : 0x%08x\n", ifid[1].instruction);
    printf("opcode : 0x%x\n", inst.opcode);
    printf("rs : %u, rt : %u, rd : %u,\n", inst.rs, inst.rt, inst.rd);
    printf("imm : 0x%08x (%u), address : 0x%08x\n", inst.imm, inst.imm, inst.address);
    printf("shamt : 0x%x, funct : 0x%x\n", inst.shamt, inst.funct);

    // Save data to pipeline
    idex[0].PCadd8 = ifid[1].PCadd4 + 4;
    idex[0].Rrs = Regs_return[0]; idex[0].Rrt = Regs_return[1];
    idex[0].extimm = extimm; idex[0].upperimm = zeroimm << 16;
    idex[0].funct = inst.funct; idex[0].shamt = inst.shamt;
    idex[0].rs = inst.rs; idex[0].rt = inst.rt; idex[0].rd = inst.rd;

    // Save control signals to pipeline
    idex[0].Shift = ctrlSig.Shift; idex[0].ALUSrc = ctrlSig.ALUSrc;
    idex[0].RegDst[0] = ctrlSig.RegDst[0]; idex[0].RegDst[1] = ctrlSig.RegDst[1];
    idex[0].MemWrite = ctrlSig.MemWrite; idex[0].MemRead = ctrlSig.MemRead;
    idex[0].MemtoReg[0] = ctrlSig.MemtoReg[0]; idex[0].MemtoReg[1] = ctrlSig.MemtoReg[1];
    idex[0].RegWrite = ctrlSig.RegWrite; idex[0].ALUOp = ctrlSig.ALUOp;

    if (!hzrddetectSig.BTBnotWrite) {
        BranchPred.Predict[1] = BranchPred.Predict[0];
        BranchPred.AddressHit[1] = BranchPred.AddressHit[0];
        BranchPred.BTBindex[1] = BranchPred.BTBindex[0];
        BranchPred.instPC[1] = BranchPred.instPC[0];
    }

    printf("**********************************************\n");

    // For visible state
    debugex[0].EXPC = debugid[1].IDPC; debugex[0].EXinst = debugid[1].IDinst;
    return;
}

// Execute
void EX(void) {
    exmem[0].valid = idex[1].valid;
    printf("\n<<<<<<<<<<<<<<<<<<<<<EX>>>>>>>>>>>>>>>>>>>>>\n");
    if (!(idex[1].valid)) {
        printf("ID/EX pipeline is invalid\n");
        printf("**********************************************\n");
        return;
    }

    printf("Processing PC : 0x%08x\n", debugex[1].EXPC);
    printf("Processing instruction : 0x%08x\n", debugex[1].EXinst);

    // Set ALU operation
    ALUCtrlUnit(idex[1].funct, idex[1].ALUOp);

    // Forwarding
    ForwardUnit(idex[1].rt, idex[1].rs, exmem[1].Writereg, memwb[1].Writereg);
    uint32_t ForwardAMUX = MUX_3(idex[1].Rrs, MemtoRegMUX, exmem[1].ALUresult, fwrdSig.ForwardA);
    uint32_t ForwardBMUX = MUX_3(idex[1].Rrt, MemtoRegMUX, exmem[1].ALUresult, fwrdSig.ForwardB);

    // Execute operation
    uint32_t ALUinput1 = MUX(ForwardAMUX, idex[1].shamt, idex[1].Shift);
    uint32_t ALUinput2 = MUX(ForwardBMUX, idex[1].extimm, idex[1].ALUSrc);
    uint32_t ALUresult = ALU(ALUinput1, ALUinput2, ALUctrlSig.ALUSig);

    // Select write register
    uint8_t Writereg = MUX_3(idex[1].rt, idex[1].rd, 31, idex[1].RegDst);

    // Print information of EX
    printf("ALU input1 : 0x%08x (%u)\nALU input2 : 0x%08x (%u)\n", ALUinput1, ALUinput1, ALUinput2, ALUinput2);
    printf("ALU result : 0x%08x (%u)\n", ALUresult, ALUresult);

    // Save data to pipeline
    exmem[0].PCadd8 = idex[1].PCadd8; exmem[0].upperimm = idex[1].upperimm;
    exmem[0].ForwardBMUX = ForwardBMUX; exmem[0].ALUresult = ALUresult;
    exmem[0].Writereg = Writereg;

    // Save control signals to pipeline
    exmem[0].MemWrite = idex[1].MemWrite; exmem[0].MemRead = idex[1].MemRead;
    exmem[0].MemtoReg[0] = idex[1].MemtoReg[0]; exmem[0].MemtoReg[1] = idex[1].MemtoReg[1];
    exmem[0].RegWrite = idex[1].RegWrite;

    printf("**********************************************\n");

    // For visible state
    debugmem[0].MEMPC = debugex[1].EXPC; debugmem[0].MEMinst = debugex[1].EXinst;
    return;
}

// Memory Access
void MEM(void) {
    memwb[0].valid = exmem[1].valid;
    printf("\n<<<<<<<<<<<<<<<<<<<<<MEM>>>>>>>>>>>>>>>>>>>>>\n");
    if (!(exmem[1].valid)) {
        printf("EX/MEM pipeline is invalid\n");
        printf("**********************************************\n");
        return;
    }

    printf("Processing PC : 0x%08x\n", debugmem[1].MEMPC);
    printf("Processing instruction : 0x%08x\n", debugmem[1].MEMinst);

    // Memory access
    uint32_t Readdata = DataMem(exmem[1].ALUresult, exmem[1].ForwardBMUX,
                                exmem[1].MemRead, exmem[1].MemWrite);

    // Save data to pipeline
    memwb[0].PCadd8 = exmem[1].PCadd8; memwb[0].ALUresult = exmem[1].ALUresult;
    memwb[0].upperimm = exmem[1].upperimm; memwb[0].Writereg = exmem[1].Writereg;
    memwb[0].Readdata = Readdata;

    // Save control signals to pipeline
    memwb[0].MemtoReg[0] = exmem[1].MemtoReg[0]; memwb[0].MemtoReg[1] = exmem[1].MemtoReg[1];
    memwb[0].RegWrite = exmem[1].RegWrite;

    printf("**********************************************\n");

    // For visible state
    debugwb[0].WBPC = debugmem[1].MEMPC; debugwb[0].WBinst = debugmem[1].MEMinst;
    return;
}

// Write Back
void WB(void) {
    printf("\n<<<<<<<<<<<<<<<<<<<<<WB>>>>>>>>>>>>>>>>>>>>>\n");
    if (!(memwb[1].valid)) {
        printf("MEM/WB pipeline is invalid\n");
        printf("**********************************************\n");
        return;
    }

    printf("Processing PC : 0x%08x\n", debugwb[1].WBPC);
    printf("Processing instruction : 0x%08x\n", debugwb[1].WBinst);

    if (memwb[1].RegWrite) {
        printf("R[%d] = 0x%x (%d)\n", memwb[1].Writereg, R[memwb[1].Writereg], R[memwb[1].Writereg]);
    }
    else {
        printf("There is no register write\n");
    }

    printf("**********************************************\n");
    return;
}
