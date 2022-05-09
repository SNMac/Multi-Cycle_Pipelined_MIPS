#include "header.h"

// from main.c
extern IFID ifid[2];
extern IDEX idex[2];
extern EXMEM exmem[2];
extern MEMWB memwb[2];
extern uint32_t Memory[0x400000];
extern uint32_t PC;
extern uint32_t R[32];
extern int Rcount;  // R-format instruction count
extern int Icount;  // I-format instruction count
extern int Jcount;  // J-format instruction count
extern int Memcount;  // Memory access instruction count

// from Stages.c
extern CONTROL_SIGNAL ctrlSig;
extern ALU_CONTROL_SIGNAL ALUctrlSig;

FORWARD_SIGNAL fwrdSig;
HAZARD_DETECTION_SIGNAL hzrddetectSig;

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
    if (signal) {
        return input2;
    }
    else {
        return input1;
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
    switch (opcode) {  // considered don't care as 0 or not written
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
    if (hzrddetectSig.ControlNOP) {
        memset(&ctrlSig, 0, sizeof(CONTROL_SIGNAL));
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

// (Fowarding Unit)
void ForwardUnit(uint8_t IDEXrt, uint8_t IDEXrs, uint8_t EXMEMWritereg, uint8_t MEMWBWritereg) {
    memset(&fwrdSig, 0, sizeof(FORWARD_SIGNAL));
    // EX hazard
    if (exmem[1].RegWrite && (EXMEMWritereg != 0) && (EXMEMWritereg == IDEXrs)) {
        fwrdSig.ForwardA[1] = 1; fwrdSig.ForwardA[0] = 0;  // ForwardA = 10
    }
    if (exmem[1].RegWrite && (EXMEMWritereg != 0) && (EXMEMWritereg == IDEXrt)) {
        fwrdSig.ForwardB[1] = 1; fwrdSig.ForwardB[0] = 0;  // ForwardB = 10
    }
    // MEM hazard
    if (memwb[1].RegWrite && (MEMWBWritereg != 0) &&
        !( exmem[1].RegWrite && (EXMEMWritereg != 0) && (EXMEMWritereg != IDEXrs) ) &&
        (MEMWBWritereg == IDEXrs)) {
        fwrdSig.ForwardA[1] = 0; fwrdSig.ForwardA[0] = 1;  // ForwardA = 01
    }
    if (memwb[1].RegWrite && (MEMWBWritereg != 0) &&
        !( exmem[1].RegWrite && (EXMEMWritereg != 0) && (EXMEMWritereg != IDEXrs) ) &&
        (MEMWBWritereg == IDEXrt)) {
        fwrdSig.ForwardB[1] = 0; fwrdSig.ForwardB[0] = 1;  // ForwardB = 01
    }
    return;
}

// (Hazard detection unit)
void HazardDetectUnit(uint8_t IFIDrs, uint8_t IFIDrt, uint8_t IDEXrt) {
    memset(&hzrddetectSig, 0, sizeof(HAZARD_DETECTION_SIGNAL));
    if (idex[1].MemRead && (IDEXrt == IFIDrs) || (IDEXrt == IFIDrt)) {
        hzrddetectSig.PCWrite = 0;
        hzrddetectSig.IFIDWrite = 0;
        hzrddetectSig.ControlNOP = 1;
        // TODO
        //  stall the pipeline
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
