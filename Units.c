#include "header.h"

CONTROL_SIGNAL ctrlSig;
ALU_CONTROL_SIGNAL ALUctrlSig;

extern IFID ifid[2];
extern IDEX idex[2];
extern EXMEM exmem[2];
extern MEMWB memwb[2];

extern uint32_t Memory[0x400000];
extern uint32_t PC;
extern int32_t R[32];
extern int Rcount;  // R-format instruction count
extern int Icount;  // I-format instruction count
extern int Jcount;  // J-format instruction count
extern int Memcount;  // Memory access instruction count
extern int Branchcount;  // taken Branch count

uint32_t MemtoRegMUX;

/*============================Stages============================*/

// Instruction Fetch
void IF() {
    printf("\n<<<<<<<<<<<<<IF>>>>>>>>>>>>>\n");
    printf("PC : 0x%08x\n", PC);

    // save data to pipeline
    ifid[0].instruction = InstMem(PC); ifid[0].PCadd4 = PC + 4;

    printf("Fetch instruction : 0x%08x\n", ifid[0].instruction);
    printf("****************************\n");
    return;
}
// Instruction Decode
void ID() {
    printf("\n<<<<<<<<<<<<<ID>>>>>>>>>>>>>\n");
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

    // Register fetch
    uint32_t* Regs_return = RegsRead(inst.rs, inst.rt);

    // Extending immediate
    int32_t signimm = (int16_t) inst.imm;
    uint32_t zeroimm = inst.imm;

    // Set control signals
    CtrlUnit(inst.opcode, inst.funct);

    uint32_t BranchAddr = ifid[1].PCadd4 + (signimm << 2);
    uint32_t JumpAddr = (ifid[1].PCadd4 & 0xf0000000) | (inst.address << 2);

//    ctrlSig.PCBranch = (ctrlSig.BranchNot & !ctrlSig.Zero) | (ctrlSig.Branch & ctrlSig.Zero);

    uint32_t PCBranchMUX = MUX(PC + 4, BranchAddr, ctrlSig.PCBranch);
    uint32_t JumpMUX = MUX_3(PCBranchMUX, JumpAddr, Regs_return[0], ctrlSig.Jump);
    PC = JumpMUX;
    if (ctrlSig.PCBranch == 1) {
        Branchcount++;
    }

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

    // set ALU operation
    ALUCtrlUnit(idex[1].funct);

    // Forwarding
    ForwardUnit(idex[1].rt, idex[1].rs, exmem[1].Writereg, memwb[1].Writereg);
    uint32_t ForwardAMUX = MUX_3(idex[1].Rrs, MemtoRegMUX, exmem[1].ALUresult, ctrlSig.ForwardA);
    uint32_t ForwardBMUX = MUX_3(idex[1].Rrt, MemtoRegMUX, exmem[1].ALUresult, ctrlSig.ForwardB);

    // Execute operation
    uint32_t ALUinput1 = MUX(ForwardAMUX, idex[1].shamt, idex[1].Shift);
    uint32_t ALUinput2 = MUX(ForwardBMUX, idex[1].extimm, idex[1].ALUSrc);

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

    // select Write data
    MemtoRegMUX = MUX_4(memwb[1].ALUresult, memwb[1].Readdata, memwb[1].PCadd8, memwb[1].upperimm, memwb[1].MemtoReg);

    // update register
    RegsWrite(memwb[1].Writereg, MemtoRegMUX);

    printf("****************************\n");
    return;
}

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
    if (memwb[1].RegWrite == 1) {  // RegWrite asserted
        if (Writereg == 0 && Writedata != 0) {
            fprintf(stderr, "WARNING: Cannot change value at $0\n");
            return;
        }
        R[Writereg] = Writedata;  // GPR write enabled
        printf("R[%d] = 0x%x (%d)\n", Writereg, R[Writereg], R[Writereg]);
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
        Memcount++;
    }
    else if (exmem[1].MemRead == 0 && exmem[1].MemWrite == 1) {  // MemRead De-asserted, MemWrite asserted
        if (Addr > 0x1000000) {  // loading outside of memory
            fprintf(stderr, "ERROR: Accessing outside of memory\n");
            exit(EXIT_FAILURE);
        }
        Memory[Addr / 4] = Writedata;  // Memory write enabled
        printf("Memory[0x%08x] <- store 0x%x (%d)\n", Addr, Memory[Addr / 4], Memory[Addr / 4]);
        Memcount++;
    }
    return Readdata;
}

// [Fowarding Unit]
void ForwardUnit(uint8_t IDEXrt, uint8_t IDEXrs, uint8_t EXMEMWritereg, uint8_t MEMWBWritereg) {
    exmem[1].RegWrite;
    memwb[1].RegWrite;
    // EX hazard
    if (exmem[1].RegWrite && (EXMEMWritereg != 0) && (EXMEMWritereg == IDEXrs)) {
        ctrlSig.ForwardA[1] = 1; ctrlSig.ForwardA[0] = 0;  // ForwardA = 10
    }
    if (exmem[1].RegWrite && (EXMEMWritereg != 0) && (EXMEMWritereg == IDEXrt)) {
        ctrlSig.ForwardB[1] = 1; ctrlSig.ForwardB[0] = 0;  // ForwardB = 10
    }
    // MEM hazard
    if (memwb[1].RegWrite && (MEMWBWritereg != 0) &&
        !( exmem[1].RegWrite && (EXMEMWritereg != 0) && (EXMEMWritereg != IDEXrs) ) &&
        (MEMWBWritereg == IDEXrs)) {
        ctrlSig.ForwardA[1] = 0; ctrlSig.ForwardA[0] = 1;  // ForwardA = 01
    }
    if (memwb[1].RegWrite && (MEMWBWritereg != 0) &&
        !( exmem[1].RegWrite && (EXMEMWritereg != 0) && (EXMEMWritereg != IDEXrs) ) &&
        (MEMWBWritereg == IDEXrt)) {
        ctrlSig.ForwardB[1] = 0; ctrlSig.ForwardB[0] = 1;  // ForwardB = 01
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
            if (ALUresult == 0) {
                ctrlSig.Zero = 1;
            }
            else {
                ctrlSig.Zero = 0;
            }
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
            uint32_t uinput1 = input1 & 0x7fffffff;
            uint32_t uinput2 = input2 & 0x7fffffff;
            if (uinput1 < uinput2) {
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
    }
    if ( ((input1 & 0x80000000) == (input2 & 0x80000000)) && ((ALUresult & 0x80000000) != (input1 & 0x80000000)) ) {
        // same sign input = different sign output
        ALUctrlSig.ArthOvfl = 1;
    }
    return ALUresult;
}

// [MUX with 2 input]
uint32_t MUX(uint32_t input1, uint32_t input2, bool signal) {
    if (signal == 0) {
        return input1;
    }
    else {
        return input2;
    }
}

// [MUX with 3 input]
uint32_t MUX_3(uint32_t input1, uint32_t input2, uint32_t input3, bool signal[]) {
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
uint32_t MUX_4(uint32_t input1, uint32_t input2, uint32_t input3, uint32_t input4, bool signal[]) {
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
    switch (opcode) {  // considered don't care as 0
        case 0x0 :  // R-format
            Rcount++;
            ctrlSig.RegDst[1] = 0; ctrlSig.RegDst[0] = 1;  // Write register = rd
            ctrlSig.ALUSrc = 0;  // ALU input2 = rt
            ctrlSig.MemtoReg[1] = 0; ctrlSig.MemtoReg[0] = 0;  // Write data = ALU result
            ctrlSig.RegWrite = 1;  // GPR write enabled
            ctrlSig.MemRead = 0;  // Memory read disabled
            ctrlSig.MemWrite = 0;  // Memory write enabled
            ctrlSig.BranchNot = 0;  // opcode != bne
            ctrlSig.Branch = 0;  // opcode != beq
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
            Icount++;
            printf("format : I   |   addi $%d, $%d, 0x%x\n", inst.rt, inst.rs, inst.imm);
            ctrlSig.RegDst[1] = 0; ctrlSig.RegDst[0] = 0;
            ctrlSig.ALUSrc = 1;
            ctrlSig.MemtoReg[1] = 0; ctrlSig.MemtoReg[0] = 0;
            ctrlSig.RegWrite = 1;
            ctrlSig.MemRead = 0;
            ctrlSig.MemWrite = 0;
            ctrlSig.SignZero = 0;
            ctrlSig.BranchNot = 0;
            ctrlSig.Branch = 0;
            ctrlSig.ALUOp = '+';
            ctrlSig.Jump[1] = 0; ctrlSig.Jump[0] = 0;
            break;

        case 0x9 :  // addiu
            Icount++;
            printf("format : I   |   addiu $%d, $%d, 0x%x\n", inst.rt, inst.rs, inst.imm);
            ctrlSig.RegDst[1] = 0; ctrlSig.RegDst[0] = 0;
            ctrlSig.ALUSrc = 1;
            ctrlSig.MemtoReg[1] = 0; ctrlSig.MemtoReg[0] = 0;
            ctrlSig.RegWrite = 1;
            ctrlSig.MemRead = 0;
            ctrlSig.MemWrite = 0;
            ctrlSig.SignZero = 0;
            ctrlSig.BranchNot = 0;
            ctrlSig.Branch = 0;
            ctrlSig.ALUOp = '+';
            ctrlSig.Jump[1] = 0; ctrlSig.Jump[0] = 0;
            break;

        case 0xc :  // andi
            Icount++;
            printf("format : I   |   andi $%d, $%d, 0x%x\n", inst.rt, inst.rs, inst.imm);
            ctrlSig.RegDst[1] = 0; ctrlSig.RegDst[0] = 0;
            ctrlSig.ALUSrc = 1;
            ctrlSig.MemtoReg[1] = 0; ctrlSig.MemtoReg[0] = 0;
            ctrlSig.RegWrite = 1;
            ctrlSig.MemRead = 0;
            ctrlSig.MemWrite = 0;
            ctrlSig.SignZero = 1;
            ctrlSig.BranchNot = 0;
            ctrlSig.Branch = 0;
            ctrlSig.ALUOp = '&';
            ctrlSig.Jump[1] = 0; ctrlSig.Jump[0] = 0;
            break;

        case 0x4 :  // beq
            Icount++;
            printf("format : I   |   beq $%d, $%d, 0x%x\n", inst.rt, inst.rs, inst.imm);
            ctrlSig.ALUSrc = 0;
            ctrlSig.RegWrite = 0;
            ctrlSig.MemRead = 0;
            ctrlSig.MemWrite = 0;
            ctrlSig.BranchNot = 0;
            ctrlSig.Branch = 1;
            ctrlSig.ALUOp = 'B';
            ctrlSig.Jump[1] = 0; ctrlSig.Jump[0] = 0;
            break;

        case 0x5 :  // bne
            Icount++;
            printf("format : I   |   bne $%d, $%d, 0x%x\n", inst.rt, inst.rs, inst.imm);
            ctrlSig.ALUSrc = 0;
            ctrlSig.RegWrite = 0;
            ctrlSig.MemRead = 0;
            ctrlSig.MemWrite = 0;
            ctrlSig.BranchNot = 1;
            ctrlSig.Branch = 0;
            ctrlSig.ALUOp = 'B';
            ctrlSig.Jump[1] = 0; ctrlSig.Jump[0] = 0;
            break;

        case 0x2 :  // j
            Jcount++;
            printf("format : J   |   j 0x%08x\n", inst.address);
            ctrlSig.RegWrite = 0;
            ctrlSig.MemRead = 0;
            ctrlSig.MemWrite = 0;
            ctrlSig.BranchNot = 0;
            ctrlSig.Branch = 0;
            ctrlSig.ALUOp = '+';
            ctrlSig.Jump[1] = 0; ctrlSig.Jump[0] = 1;
            break;

        case 0x3 :  // jal
            Jcount++;
            printf("format : J   |   jal 0x%08x\n", inst.address);
            ctrlSig.RegWrite = 1;
            ctrlSig.RegDst[1] = 1; ctrlSig.RegDst[0] = 0;
            ctrlSig.MemtoReg[1] = 1; ctrlSig.MemtoReg[0] = 0;
            ctrlSig.ALUOp = '+';
            ctrlSig.Jump[1] = 0; ctrlSig.Jump[0] = 1;
            break;
        
        case 0xf :  // lui
            Icount++;
            printf("format : I   |   lui $%d, 0x%x\n", inst.rt, inst.imm);
            ctrlSig.RegDst[1] = 0; ctrlSig.RegDst[0] = 0;
            ctrlSig.ALUSrc = 1;
            ctrlSig.MemtoReg[1] = 1; ctrlSig.MemtoReg[0] = 1;
            ctrlSig.RegWrite = 1;
            ctrlSig.MemRead = 0;
            ctrlSig.MemWrite = 0;
            ctrlSig.BranchNot = 0;
            ctrlSig.Branch = 0;
            ctrlSig.ALUOp = '+';
            ctrlSig.Jump[1] = 0; ctrlSig.Jump[0] = 0;
            break;

        case 0x23 :  // lw
            Icount++;
            printf("format : I   |   lw $%d, 0x%x($%d)\n", inst.rt, inst.imm, inst.rs);
            ctrlSig.RegDst[1] = 0; ctrlSig.RegDst[0] = 0;
            ctrlSig.ALUSrc = 1;
            ctrlSig.MemtoReg[1] = 0; ctrlSig.MemtoReg[0] = 1;
            ctrlSig.RegWrite = 1;
            ctrlSig.MemRead = 1;
            ctrlSig.MemWrite = 0;
            ctrlSig.SignZero = 0;
            ctrlSig.BranchNot = 0;
            ctrlSig.Branch = 0;
            ctrlSig.ALUOp = '+';
            ctrlSig.Jump[1] = 0; ctrlSig.Jump[0] = 0;
            break;

        case 0xd :  // ori
            Icount++;
            printf("format : I   |   ori $%d, $%d, 0x%x\n", inst.rt, inst.rs, inst.imm);
            ctrlSig.RegDst[1] = 0; ctrlSig.RegDst[0] = 0;
            ctrlSig.ALUSrc = 1;
            ctrlSig.MemtoReg[1] = 0; ctrlSig.MemtoReg[0] = 0;
            ctrlSig.RegWrite = 1;
            ctrlSig.MemRead = 0;
            ctrlSig.MemWrite = 0;
            ctrlSig.SignZero = 1;
            ctrlSig.BranchNot = 0;
            ctrlSig.Branch = 0;
            ctrlSig.ALUOp = '|';
            ctrlSig.Jump[1] = 0; ctrlSig.Jump[0] = 0;
            break;

        case 0xa :  // slti
            Icount++;
            printf("format : I   |   slti $%d, $%d, 0x%x\n", inst.rt, inst.rs, inst.imm);
            ctrlSig.RegDst[1] = 0; ctrlSig.RegDst[0] = 0;
            ctrlSig.ALUSrc = 1;
            ctrlSig.MemtoReg[1] = 0; ctrlSig.MemtoReg[0] = 0;
            ctrlSig.RegWrite = 1;
            ctrlSig.MemRead = 0;
            ctrlSig.MemWrite = 0;
            ctrlSig.SignZero = 0;
            ctrlSig.BranchNot = 0;
            ctrlSig.Branch = 0;
            ctrlSig.ALUOp = '<';
            ctrlSig.Jump[1] = 0; ctrlSig.Jump[0] = 0;
            break;

        case 0xb :  // sltiu
            Icount++;
            printf("format : I   |   sltiu $%d, $%d, 0x%x\n", inst.rt, inst.rs, inst.imm);
            ctrlSig.RegDst[1] = 0; ctrlSig.RegDst[0] = 0;
            ctrlSig.ALUSrc = 1;
            ctrlSig.MemtoReg[1] = 0; ctrlSig.MemtoReg[0] = 0;
            ctrlSig.RegWrite = 1;
            ctrlSig.MemRead = 0;
            ctrlSig.MemWrite = 0;
            ctrlSig.SignZero = 0;
            ctrlSig.BranchNot = 0;
            ctrlSig.Branch = 0;
            ctrlSig.ALUOp = '>';
            ctrlSig.Jump[1] = 0; ctrlSig.Jump[0] = 0;
            break;

        case 0x2B :  // sw
            Icount++;
            printf("format : I   |   sw $%d, 0x%x($%d)\n", inst.rt, inst.imm, inst.rs);
            ctrlSig.ALUSrc = 1; 
            ctrlSig.RegWrite = 0;
            ctrlSig.MemRead = 0;
            ctrlSig.MemWrite = 1;
            ctrlSig.SignZero = 0;
            ctrlSig.BranchNot = 0;
            ctrlSig.Branch = 0;
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

        default :
            fprintf(stderr, "ERROR: Instruction has wrong funct\n");
            exit(EXIT_FAILURE);
            
    }
}



/*============================Select ALU operation============================*/

char Rformat(uint8_t funct) {  // select ALU operation by funct (R-format)
    switch (funct) {
        case 0x20 :  // add
            printf("format : R   |   add $%d, $%d, $%d\n", inst.rd, inst.rs, inst.rt);
            return '+';

        case 0x21 :  // addu
            printf("format : R   |   addu $%d, $%d, $%d\n", inst.rd, inst.rs, inst.rt);
            return '+';

        case 0x24 :  // and
            printf("format : R   |   and $%d, $%d, $%d\n", inst.rd, inst.rs, inst.rt);
            return '&';

        case 0x08 :  // jr
            printf("format : R   |   jr $%d\n", inst.rs);
            return '+';
        
        case 0x9 :  // jalr
            printf("format : R   |   jalr $%d\n", inst.rs);
            return '+';

        case 0x27 :  // nor
            printf("format : R   |   nor $%d, $%d, $%d\n", inst.rd, inst.rs, inst.rt);
            return '~';

        case 0x25 :  // or
            printf("format : R   |   or $%d, $%d, $%d\n", inst.rd, inst.rs, inst.rt);
            return '|';

        case 0x2a :  // slt
            printf("format : R   |   slt $%d, $%d, $%d\n", inst.rd, inst.rs, inst.rt);
            return '<';

        case 0x2b :  // sltu
            printf("format : R   |   sltu $%d, $%d, $%d\n", inst.rd, inst.rs, inst.rt);
            return '>';

        case 0x00 :  // sll
            printf("format : R   |   sll $%d, $%d, %d\n", inst.rd, inst.rt, inst.shamt);
            return '{';

        case 0x02 :  // srl
            printf("format : R   |   srl $%d, $%d, %d\n", inst.rd, inst.rt, inst.shamt);
            return '}';

        case 0x22 :  // sub
            printf("format : R   |   sub $%d, $%d, $%d\n", inst.rd, inst.rs, inst.rt);
            return '-';

        case 0x23 :  // subu
            printf("format : R   |   subu $%d, $%d, $%d\n", inst.rd, inst.rs, inst.rt);
            return '-';

        default:
            fprintf(stderr, "ERROR: Instruction has wrong funct\n");
            exit(EXIT_FAILURE);
    }
}


/*============================Exception============================*/

// Overflow exception
void OverflowException() {  // check overflow in signed instruction
    fprintf(stderr, "ERROR: Arithmetic overflow occured\n");
    exit(EXIT_FAILURE);
}
