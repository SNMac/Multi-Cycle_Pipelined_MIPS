#include "header.h"

CONTROL_SIGNAL ctrlSig;
ALU_CONTROL_SIGNAL ALUctrlSig;

// from main.c
extern uint32_t Memory[0x400000];
extern uint32_t PC; extern bool PCvalid;  // PC valid bit
extern uint32_t R[32];
extern IFID ifid[2];
extern IDEX idex[2];
extern EXMEM exmem[2];
extern MEMWB memwb[2];
extern BRANCH_PREDICT BranchPred;
extern COUNTING counting;
extern DEBUGID debugid[2];
extern DEBUGEX debugex[2];
extern DEBUGMEM debugmem[2];
extern DEBUGWB debugwb[2];


// from Units.c
extern FORWARD_SIGNAL fwrdSig;
extern HAZARD_DETECTION_SIGNAL hzrddetectSig;
extern ID_FORWARD_SIGNAL idfwrdSig;

uint32_t MemtoRegMUX;


/*============================Stages============================*/

// Instruction Fetch
void IF(void) {
    printf("\n<<<<<<<<<<<<<<<<<<<<<IF>>>>>>>>>>>>>>>>>>>>>\n");
    if (PC == 0xffffffff) {
        ifid[0].valid = 0;
        PCvalid = 0;
        printf("End of program\n");
        printf("Wait for finishing other stage\n");
        printf("**********************************************\n");
        return;
    }

    // Fetch instruction
    uint32_t instruction = InstMem(PC);

    // Print information of IF
    printf("PC : 0x%08x\n", PC);
    printf("Fetch instruction : 0x%08x\n", instruction);

    // Check if PC is branch instruction
    CheckBranch(PC);

    // Save data to pipeline
    if (!hzrddetectSig.IFIDnotWrite){
        ifid[0].instruction = instruction; ifid[0].PCadd4 = PC + 4;

        // For visible state
        debugid[0].IDPC = PC; debugid[0].IDinst = instruction;
    }

    if (ctrlSig.IFFlush) {
        ifid[0].instruction = 0;
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
    HazardDetectUnit(inst.rs, inst.rt, idex[1].rt, IDEXWritereg);

    // Avoid ID-WB hazard
    MemtoRegMUX = MUX_4(memwb[1].ALUresult, memwb[1].Readdata, memwb[1].PCadd8, memwb[1].upperimm, memwb[1].MemtoReg);
    RegsWrite(memwb[1].Writereg, MemtoRegMUX);

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
    printf("BranchForwardAMUX = %u\n BranchForwardBMUX = %u\n", IDForwardAMUX, IDForwardBMUX);
    bool PCBranch = (ctrlSig.BNE & !Equal) | (ctrlSig.BEQ & Equal);

    // Extending immediate
    int32_t signimm = (int16_t) inst.imm;
    uint32_t zeroimm = inst.imm;
    uint32_t extimm = MUX(signimm, zeroimm, ctrlSig.SignZero);

    // Address calculate
    uint32_t BranchAddr = ifid[1].PCadd4 + (signimm << 2);
    uint32_t JumpAddr = (ifid[1].PCadd4 & 0xf0000000) | (inst.address << 2);

    // Update branch result to BTB
    if ((ctrlSig.BEQ | ctrlSig.BNE)) {  // beq, bne
        if (BranchPred.Predict[1]) {  // PC found in BTB
            UpdatePredictBits(PCBranch);
            if (PCBranch) {  // Branch taken
                printf("Branch taken, ");
                counting.takenBranch++;
                if (BranchPred.AddressHit[1]) {  // Predict branch taken, HIT
                    printf("predicted branch taken.\nBranch prediction HIT.\n");
                    counting.PredictHitCount++;
                }
                else {  // Predict branch not taken
                    printf("predicted branch not taken.\nBranch prediction not HIT. Flushing IF instruction.\n");
                    ctrlSig.IFFlush = 1;
                    ctrlSig.IFIDPC = 0;
                }
            }

            else {  // Branch not taken
                printf("Branch not taken, ");
                counting.nottakenBranch++;
                if (BranchPred.AddressHit[1]) {  // Predict branch taken
                    printf("predicted branch taken.\nBranch prediction not HIT. Flushing IF instruction.\n");
                    ctrlSig.IFFlush = 1;
                    ctrlSig.IFIDPC = 1;
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
                UpdatePredictBits(PCBranch);
                printf("Instruction is branch. FLushing IF instruction.\n");
                counting.takenBranch++;
                ctrlSig.IFFlush = 1;
                ctrlSig.IFIDPC = 0;
            }
            else {  // Branch not taken
                counting.nottakenBranch++;
            }
        }
    }

    // Select PC address
    bool PCtarget = BranchPred.Predict[0] & BranchPred.AddressHit[0];
    uint32_t IFIDPCMUX = MUX(PC + 4, ifid[1].PCadd4, ctrlSig.IFIDPC);
    uint32_t PCBranchMUX = MUX(IFIDPCMUX, BranchAddr, PCBranch);
    uint32_t JumpMUX = MUX_3(PCBranchMUX, JumpAddr, IDForwardAMUX, ctrlSig.Jump);
    uint32_t PredictMUX = MUX(JumpMUX, BranchPred.BTB[BranchPred.BTBindex[0]][1], PCtarget);
    if (PCvalid & !(hzrddetectSig.PCnotWrite)) {  // PC write disabled
        PC = PredictMUX;
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
    ALUCtrlUnit(idex[1].funct);

    // Forwarding
    ForwardUnit(idex[1].rt, idex[1].rs, exmem[1].Writereg, memwb[1].Writereg);
    uint32_t ForwardAMUX = MUX_3(idex[1].Rrs, MemtoRegMUX, exmem[1].ALUresult, fwrdSig.ForwardA);
    uint32_t ForwardBMUX = MUX_3(idex[1].Rrt, MemtoRegMUX, exmem[1].ALUresult, fwrdSig.ForwardB);

    // Execute operation
    uint32_t ALUinput1 = MUX(ForwardAMUX, idex[1].shamt, idex[1].Shift);
    uint32_t ALUinput2 = MUX(ForwardBMUX, idex[1].extimm, idex[1].ALUSrc);
    uint32_t ALUresult = ALU(ALUinput1, ALUinput2);

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
    uint32_t Readdata = DataMem(exmem[1].ALUresult, exmem[1].ForwardBMUX);

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

    printf("**********************************************\n");
    return;
}


/* Debugging */
void IFIDDebug(void) {
    printf("\n//////////////// IF/ID pipeline ////////////////\n");
    printf("valid)       IFID[0] : %d,  IFID[1] : %d\n", ifid[0].valid, ifid[1].valid);
    printf("instruction) IFID[0] : 0x%08x, IFID[1] : 0x%08x\n", ifid[0].instruction, ifid[1].instruction);
    printf("PCadd4)      IFID[0] : 0x%08x, IFID[1] : 0x%08x\n", ifid[0].PCadd4, ifid[1].PCadd4);
    printf("////////////////////////////////////////////////\n");
}

void IDEXDebug(void) {
    printf("\n//////////////// ID/EX pipeline ////////////////\n");
    printf("valid)    IDEX[0] : %d,          IDEX[1] : %d\n", idex[0].valid, idex[1].valid);
    printf("PCadd8)   IDEX[0] : 0x%08x, IDEX[1] : 0x%08x\n", idex[0].PCadd8, idex[1].PCadd8);
    printf("shamt)    IDEX[0] : %u,          IDEX[1] : %u\n", idex[0].shamt, idex[1].shamt);
    printf("funct)    IDEX[0] : 0x%x,        IDEX[1] : 0x%x\n", idex[0].funct, idex[1].funct);
    printf("R[rs])    IDEX[0] : 0x%08x (%u), IDEX[1] : 0x%08x (%u)\n", idex[0].Rrs, idex[0].Rrs, idex[1].Rrs, idex[1].Rrs);
    printf("R[rt])    IDEX[0] : 0x%08x (%u), IDEX[1] : 0x%08x (%u)\n", idex[0].Rrt, idex[0].Rrt, idex[1].Rrt, idex[1].Rrt);
    printf("extimm)   IDEX[0] : 0x%08x (%u), IDEX[1] : 0x%08x (%u)\n", idex[0].extimm, idex[0].extimm, idex[1].extimm, idex[1].extimm);
    printf("upperimm) IDEX[0] : 0x%08x (%u), IDEX[1] : 0x%08x (%u)\n", idex[0].upperimm, idex[0].upperimm, idex[1].upperimm, idex[1].upperimm);
    printf("rs)       IDEX[0] : %u,          IDEX[1] : %u\n", idex[0].rs, idex[1].rs);
    printf("rt)       IDEX[0] : %u,          IDEX[1] : %u\n", idex[0].rt, idex[1].rt);
    printf("rd)       IDEX[0] : %u,          IDEX[1] : %u\n", idex[0].rd, idex[1].rd);
    printf("Shift)    IDEX[0] : %d,          IDEX[1] : %d\n", idex[0].Shift, idex[1].Shift);
    printf("ALUSrc)   IDEX[0] : %d,          IDEX[1] : %d\n", idex[0].ALUSrc, idex[1].ALUSrc);
    printf("RegDst)   IDEX[0] : %d%d,        IDEX[1] : %d%d\n", idex[0].RegDst[1], idex[0].RegDst[0], idex[1].RegDst[1], idex[1].RegDst[0]);
    printf("MemWrite) IDEX[0] : %d,          IDEX[1] : %d\n", idex[0].MemWrite, idex[1].MemWrite);
    printf("MemRead)  IDEX[0] : %d,          IDEX[1] : %d\n", idex[0].MemRead, idex[1].MemRead);
    printf("MemtoReg) IDEX[0] : %d%d,        IDEX[1] : %d%d\n", idex[0].MemtoReg[1], idex[0].MemtoReg[0], idex[1].MemtoReg[1], idex[1].MemtoReg[0]);
    printf("RegWrite) IDEX[0] : %d,          IDEX[1] : %d\n", idex[0].RegWrite, idex[1].RegWrite);
    printf("////////////////////////////////////////////////\n");
}

void EXMEMDebug(void) {
    printf("\n//////////////// EX/MEM pipeline ////////////////\n");
    printf("valid)     EXMEM[0] : %d,          EXMEM[1] : %d\n", exmem[0].valid, exmem[1].valid);
    printf("PCadd8)    EXMEM[0] : 0x%08x,      EXMEM[1] : 0x%08x\n", exmem[0].PCadd8, exmem[1].PCadd8);
    printf("ALUresult) EXMEM[0] : 0x%08x (%u), EXMEM[1] : 0x%08x (%u)\n", exmem[0].ALUresult, exmem[0].ALUresult, exmem[1].ALUresult, exmem[1].ALUresult);
    printf("upperimm)  EXMEM[0] : 0x%08x (%u), EXMEM[1] : 0x%08x (%u)\n", exmem[0].upperimm, exmem[0].upperimm, exmem[1].upperimm, exmem[1].upperimm);
    printf("Writereg)  EXMEM[0] : %u,          EXMEM[1] : %u\n", exmem[0].Writereg, exmem[1].Writereg);
    printf("MemWrite)  EXMEM[0] : %d,          EXMEM[1] : %d\n", exmem[0].MemWrite, exmem[1].MemWrite);
    printf("MemRead)   EXMEM[0] : %d,          EXMEM[1] : %d\n", exmem[0].MemRead, exmem[1].MemRead);
    printf("MemtoReg)  EXMEM[0] : %d%d,        EXMEM[1] : %d%d\n", exmem[0].MemtoReg[1], exmem[0].MemtoReg[0], exmem[1].MemtoReg[1], exmem[1].MemtoReg[0]);
    printf("RegWrite)  EXMEM[0] : %d,          EXMEM[1] : %d\n", exmem[0].RegWrite, exmem[1].RegWrite);
    printf("/////////////////////////////////////////////////\n");
}

void MEMWBDebug(void) {
    printf("\n//////////////// MEM/WB pipeline ////////////////\n");
    printf("valid)     MEMWB[0] : %d,          MEMWB[1] : %d\n", memwb[0].valid, memwb[1].valid);
    printf("PCadd8)    MEMWB[0] : 0x%08x,      MEMWB[1] : 0x%08x\n", memwb[0].PCadd8, memwb[1].PCadd8);
    printf("Readdata)  MEMWB[0] : 0x%08x (%u), MEMWB[1] : 0x%08x (%u)\n", memwb[0].Readdata, memwb[0].Readdata, memwb[1].Readdata, memwb[1].Readdata);
    printf("ALUresult) MEMWB[0] : 0x%08x (%u), MEMWB[1] : 0x%08x (%u)\n", memwb[0].ALUresult, memwb[0].ALUresult, memwb[1].ALUresult, memwb[1].ALUresult);
    printf("upperimm)  MEMWB[0] : 0x%08x (%u), MEMWB[1] : 0x%08x (%u)\n", memwb[0].upperimm, memwb[0].upperimm, memwb[1].upperimm, memwb[1].upperimm);
    printf("Writereg)  MEMWB[0] : %u,          MEMWB[1] : %u\n", memwb[0].Writereg, memwb[1].Writereg);
    printf("MemtoReg)  MEMWB[0] : %d%d,        MEMWB[1] : %d%d\n", memwb[0].MemtoReg[1], memwb[0].MemtoReg[0], memwb[1].MemtoReg[1], memwb[1].MemtoReg[0]);
    printf("RegWrite)  MEMWB[0] : %d,          MEMWB[1] : %d\n", memwb[0].RegWrite, memwb[1].RegWrite);
    printf("/////////////////////////////////////////////////\n");
}

//
// Created by SNMac on 2022/05/09.
//
