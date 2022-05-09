#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include <limits.h>

typedef struct _INSTRUCTION {  // Instruction
    uint32_t address;
    uint8_t opcode;
    uint8_t rs;
    uint8_t rt;
    uint8_t rd;
    uint8_t shamt;
    uint16_t imm;
    uint8_t funct;
}INSTRUCTION;

/* Control signals */
typedef struct _CONTROL_SIGNAL {  // Control signals
	bool PCBranch, ALUSrc, RegWrite, MemRead, MemWrite, SignZero, Branch, BranchNot,
        Zero, Shift, Jump[2], RegDst[2], MemtoReg[2];
    char ALUOp;
}CONTROL_SIGNAL;

typedef struct _ALU_CONTROL_SIGNAL {  // ALU control signals
	bool ArthOvfl;
    char ALUSig;
}ALU_CONTROL_SIGNAL;

typedef struct _FORWARD_SIGNAL {
    bool ForwardA[2], ForwardB[2];
}FORWARD_SIGNAL;

typedef struct _HAZARD_DETECTION_SIGNAL {
    bool PCWrite, IFIDWrite, ControlNOP;
}HAZARD_DETECTION_SIGNAL;

//////////////////////////////// Stages.c ////////////////////////////////
/* Stages */
void IF();  // Instruction Fetch
void ID();  // Instruction Decode
void EX();  // EXecute
void MEM();  // MEMory access
void WB();  // Write Back

//////////////////////////////// Units.c ////////////////////////////////
/* Pipelines units */
typedef struct _IFID {  // IF/ID pipeline
    bool valid;
    uint32_t instruction;
    uint32_t PCadd4;
}IFID;

typedef struct _IDEX {  // ID/EX pipeline
    bool valid;
    uint32_t PCadd8;
    uint8_t shamt;
    uint8_t funct;
    uint32_t Rrs;
    uint32_t Rrt;
    int32_t extimm;
    uint32_t upperimm;
    uint8_t rs;
    uint8_t rt;
    uint8_t rd;
    bool Shift, ALUSrc, RegDst[2], MemWrite, MemRead, MemtoReg[2], RegWrite;
    char ALUOp;
}IDEX;

typedef struct _EXMEM {  // EX/MEM pipeline
    bool valid;
    uint32_t PCadd8;
    uint32_t ALUresult;
    uint32_t ForwardBMUX;
    uint32_t upperimm;
    uint8_t Writereg;
    bool MemWrite, MemRead, MemtoReg[2], RegWrite;
}EXMEM;

typedef struct _MEMWB {  // MEM/WB pipeline
    bool valid;
    uint32_t PCadd8;
    uint32_t Readdata;
    uint32_t ALUresult;
    uint32_t upperimm;
    uint8_t Writereg;
    bool MemtoReg[2], RegWrite;
}MEMWB;

/* Data units */
uint32_t InstMem(uint32_t Readaddr);  // Instruction memory
uint32_t* RegsRead(uint8_t Readreg1, uint8_t Readreg2);  // Registers (ID)
void RegsWrite(uint8_t Writereg, uint32_t Writedata);  // Register (WB)
uint32_t DataMem(uint32_t Addr, uint32_t Writedata);  // Data memory


uint32_t ALU(uint32_t input1, uint32_t input2);  // ALU
uint32_t MUX(uint32_t input1, uint32_t input2, bool signal);  // signal == 0) input1, 1) input2
uint32_t MUX_3(uint32_t input1, uint32_t input2, uint32_t input3, bool signal[]);  // signal == 0) input1, 1) input2, 2) input3
uint32_t MUX_4(uint32_t input1, uint32_t input2, uint32_t input3, uint32_t input4, bool signal[]);  // signal == 0) input1, 1) input2, 2) input3, 3) input4

/* Control units */
void CtrlUnit(uint8_t opcode, uint8_t funct);  // Control unit
void ALUCtrlUnit(uint8_t funct);  // ALU control unit
void ForwardUnit(uint8_t IDEXrt, uint8_t IDEXrs, uint8_t EXMEMWritereg, uint8_t MEMWBWritereg);
void HazardDetectUnit(uint8_t IFIDrs, uint8_t IFIDrt, uint8_t IDEXrt);  // Hazard detection unit

/* select ALU operation */
char Rformat(uint8_t funct);  // select ALU operation by funct (R-format)

/* Overflow exception */
void OverflowException();  // Overflow exception

#ifndef CAMP_PROJECT3_HEADER_H
#define CAMP_PROJECT3_HEADER_H

#endif //CAMP_PROJECT3_HEADER_H