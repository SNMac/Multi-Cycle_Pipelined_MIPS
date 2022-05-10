#include "header.h"

CONTROL_SIGNAL ctrlSig;
ALU_CONTROL_SIGNAL ALUctrlSig;

// from main.c
extern IFID ifid[2];
extern IDEX idex[2];
extern EXMEM exmem[2];
extern MEMWB memwb[2];
extern uint32_t Memory[0x400000];
extern uint32_t PC;
extern uint32_t R[32];
extern int takenBranch;  // taken Branch count
extern int nottakenBranch;  // not taken Branch count

// from Units.c
extern FORWARD_SIGNAL fwrdSig;
extern HAZARD_DETECTION_SIGNAL hzrddetectSig;
extern BRANCH_PREDICT BranchPred;

uint32_t MemtoRegMUX;

/*============================Stages============================*/

// Instruction Fetch
void IF() {
    printf("\n<<<<<<<<<<<<<IF>>>>>>>>>>>>>\n");
    if (PC == 0xffffffff) {
        ifid[0].valid = 0;
        printf("PC = 0xFFFFFFFF\n");
        printf("****************************\n");
        return;
    }
    printf("PC : 0x%08x\n", PC);
    BranchPred.instPC = PC;
    CheckBranch();

    printf("Fetch instruction : 0x%08x\n", ifid[0].instruction);
    printf("****************************\n");

    memset(&ifid, 0, sizeof(IFID));
    // save data to pipeline
    ifid[0].instruction = InstMem(PC); ifid[0].PCadd4 = PC + 4;
    if (ctrlSig.IFIDFlush) {
        memset(&ifid, 0, sizeof(IFID));
    }
    return;
}
// Instruction Decode
void ID() {
    printf("\n<<<<<<<<<<<<<ID>>>>>>>>>>>>>\n");
    if (!(ifid[1].valid)) {
        printf("IF/ID pipeline is invalid\n");
        printf("****************************\n");
        return;
    }
    printf("Instruction : 0x%08x\n", ifid[1].instruction);

    INSTRUCTION inst;
    memset(&inst, 0, sizeof(INSTRUCTION));
    inst.address = ifid[1].instruction & 0x3ffffff;  // for J-format
    inst.opcode = (ifid[1].instruction & 0xfc000000) >> 26;
    inst.rs = (ifid[1].instruction & 0x03e00000) >> 21;
    inst.rt = (ifid[1].instruction & 0x001f0000) >> 16;
    inst.rd = (ifid[1].instruction & 0x0000f800) >> 11;
    inst.shamt = (ifid[1].instruction & 0x000007c0) >> 6;  // for sll, srl
    inst.imm = ifid[1].instruction & 0x0000ffff;  // for I-format
    inst.funct = inst.imm & 0x003f;  // for R-format

    // Load-use hazard detect
    HazardDetectUnit(inst.rs, inst.rt, idex[1].rt);

    // Set control signals
    CtrlUnit(inst.opcode, inst.funct);

    // Register fetch
    uint32_t* Regs_return = RegsRead(inst.rs, inst.rt);
    if (Regs_return[0] == Regs_return[1]) {  // Comparator
        ctrlSig.Equal = 1;
    }
    ctrlSig.PCBranch = (ctrlSig.BNE & !ctrlSig.Equal) | (ctrlSig.BEQ & ctrlSig.Equal);

    // Extending immediate
    int32_t signimm = (int16_t) inst.imm;
    uint32_t zeroimm = inst.imm;

    // Address calculate
    uint32_t BranchAddr = ifid[1].PCadd4 + (signimm << 2);
    uint32_t JumpAddr = (ifid[1].PCadd4 & 0xf0000000) | (inst.address << 2);

    // Update jump result
    if (ctrlSig.Jump[0] == 1) {  // j, jal
        if(BranchPred.predict) {
            BranchBufferWrite(JumpAddr, 1);
        }
    }
    // Update branch result
    if (ctrlSig.BEQ | ctrlSig.BNE) {  // Instruction is beq or bne
        if (BranchPred.predict) {
            UpdatePredictBits();
        }
        else {
            BranchBufferWrite(BranchAddr, 0);
        }
    }

    if (ctrlSig.PCBranch) {
        takenBranch++;
    }
    else {
        nottakenBranch++;
    }

    // TODO
    //  adjust PCbranchMUX
    //  uint32_t PCBranchMUX = MUX(PC + 4, BranchAddr, ctrlSig.PCBranch);
    uint32_t JumpMUX = MUX_3(PCBranchMUX, JumpAddr, Regs_return[0], ctrlSig.Jump);
    PC = JumpMUX;

    if (!(idex[0].valid)) {
        return;
    }
    memset(&idex, 0, sizeof(IDEX));
    // save data to pipeline
    idex[0].PCadd8 = ifid[1].PCadd4 + 4; idex[0].shamt = inst.shamt;
    idex[0].Rrs = Regs_return[0]; idex[0].Rrt = Regs_return[1];
    idex[0].extimm = MUX(signimm, zeroimm, ctrlSig.SignZero);
    idex[0].upperimm = zeroimm << 16; idex[0].funct = inst.funct;
    idex[0].rs = inst.rs; idex[0].rt = inst.rt; idex[0].rd = inst.rd;

    // save control signals to pipeline
    idex[0].Shift = ctrlSig.Shift; idex[0].ALUSrc = ctrlSig.ALUSrc;
    idex[0].RegDst[0] = ctrlSig.RegDst[0]; idex[0].RegDst[1] = ctrlSig.RegDst[1];
    idex[0].MemWrite = ctrlSig.MemWrite; idex[0].MemRead = ctrlSig.MemRead;
    idex[0].MemtoReg[0] = ctrlSig.MemtoReg[0]; idex[0].MemtoReg[1] = ctrlSig.MemtoReg[1];
    idex[0].RegWrite = ctrlSig.RegWrite; idex[0].ALUOp = ctrlSig.ALUOp;

    printf("****************************\n");
    return;
}

// Execute
void EX() {
    printf("\n<<<<<<<<<<<<<EX>>>>>>>>>>>>>\n");
    if (!(idex[1].valid)) {
        printf("ID/EX pipeline is invalid\n");
        printf("****************************\n");
        return;
    }

    // set ALU operation
    ALUCtrlUnit(idex[1].funct);

    // Forwarding
    ForwardUnit(idex[1].rt, idex[1].rs, exmem[1].Writereg, memwb[1].Writereg);
    uint32_t ForwardAMUX = MUX_3(idex[1].Rrs, MemtoRegMUX, exmem[1].ALUresult, fwrdSig.ForwardA);
    uint32_t ForwardBMUX = MUX_3(idex[1].Rrt, MemtoRegMUX, exmem[1].ALUresult, fwrdSig.ForwardB);

    // Execute operation
    uint32_t ALUinput1 = MUX(ForwardAMUX, idex[1].shamt, idex[1].Shift);
    uint32_t ALUinput2 = MUX(ForwardBMUX, idex[1].extimm, idex[1].ALUSrc);

    if (!(exmem[0].valid)) {
        return;
    }
    memset(&exmem, 0, sizeof(EXMEM));
    // save data to pipeline
    exmem[0].ForwardBMUX = ForwardBMUX;
    exmem[0].ALUresult = ALU(ALUinput1, ALUinput2);
    exmem[0].PCadd8 = idex[1].PCadd8; exmem[0].upperimm = idex[1].upperimm;
    // select write register
    exmem[0].Writereg = MUX_3(idex[1].rt, idex[1].rd, 31, idex[1].RegDst);
    // save control signals to pipeline
    exmem[0].MemWrite = idex[1].MemWrite; exmem[0].MemRead = idex[1].MemRead;
    exmem[0].MemtoReg[0] = idex[1].MemtoReg[0]; exmem[0].MemtoReg[1] = idex[1].MemtoReg[1];
    exmem[0].RegWrite = idex[1].RegWrite;

    printf("****************************\n");
    return;
}

// Memory Access
void MEM() {
    printf("\n<<<<<<<<<<<<<MEM>>>>>>>>>>>>>\n");
    if (!(exmem[1].valid)) {
        printf("EX/MEM pipeline is invalid\n");
        printf("****************************\n");
        return;
    }

    if (!(memwb[0].valid)) {
        return;
    }
    memset(&memwb, 0, sizeof(MEMWB));
    // save data to pipeline
    memwb[0].PCadd8 = exmem[1].PCadd8; memwb[0].ALUresult = exmem[1].ALUresult;
    memwb[0].upperimm = exmem[1].upperimm; memwb[0].Writereg = exmem[1].Writereg;
    // Memory access
    memwb[0].Readdata = DataMem(exmem[1].ALUresult, exmem[1].ForwardBMUX);
    // save control signals to pipeline
    memwb[0].MemtoReg[0] = exmem[1].MemtoReg[0]; memwb[0].MemtoReg[1] = exmem[1].MemtoReg[1];
    memwb[0].RegWrite = exmem[1].RegWrite;

    printf("****************************\n");
    return;
}

// Write Back
void WB() {
    printf("\n<<<<<<<<<<<<<WB>>>>>>>>>>>>>\n");
    if (!(memwb[1].valid)) {
        printf("MEM/WB pipeline is invalid\n");
        printf("****************************\n");
        return;
    }

    // select Write data
    MemtoRegMUX = MUX_4(memwb[1].ALUresult, memwb[1].Readdata, memwb[1].PCadd8, memwb[1].upperimm, memwb[1].MemtoReg);

    // update register
    RegsWrite(memwb[1].Writereg, MemtoRegMUX);

    printf("****************************\n");
    return;
}
//
// Created by SNMac on 2022/05/09.
//
