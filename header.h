#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#define BTBMAX 32

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
	bool PCBranch, ALUSrc, RegWrite, MemRead, MemWrite, SignZero, BEQ,
        BNE, Equal, Shift, IFIDFlush, Jump[2], RegDst[2], MemtoReg[2];
    char ALUOp;
}CONTROL_SIGNAL;

typedef struct _ALU_CONTROL_SIGNAL {  // ALU control signals
	bool ArthOvfl;
    char ALUSig;
}ALU_CONTROL_SIGNAL;

typedef struct _FORWARD_SIGNAL {  // Forward unit signals
    bool ForwardA[2], ForwardB[2];
}FORWARD_SIGNAL;

typedef struct _HAZARD_DETECTION_SIGNAL {  // Hazard detection unit signals
    bool PCnotWrite, IFIDnotWrite, ControlNOP;
}HAZARD_DETECTION_SIGNAL;

typedef struct _BRANCH_PREDICT {
    bool Predict;  // 1) BTB has the PC value, 0) BTB hasn't the PC value
    bool AddressHit;
    int BTBindex;
    int BTBsize;
    uint32_t instPC;
    uint8_t PHT;  // Pattern History table
    uint8_t BHR;  // Branch History Register (4 bits)
    uint32_t BTB[BTBMAX][4];  // Branch Target Buffer
    // [i][0] = BranchinstPC, [i][1] = BranchTarget,
    // [i][2] = Prediction bits, [i][3] = Frequency (How many times used)
}BRANCH_PREDICT;

//////////////////////////////// Stages.c ////////////////////////////////
/* Stages */
void IF(void);  // Instruction Fetch
void ID(void);  // Instruction Decode
void EX(void);  // EXecute
void MEM(void);  // MEMory access
void WB(void);  // Write Back

void IFIDDebug(void);
void IDEXDebug(void);
void EXMEMDebug(void);
void MEMWBDebug(void);

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
    uint32_t extimm;
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
void CheckBranch(void);  // Check branch in IF
void UpdatePredictBits(void);  // Update prediction bits
void BranchBufferWrite(uint32_t Address, bool brjp);  // Write BranchAddr to BTB
uint32_t ALU(uint32_t input1, uint32_t input2);  // ALU
uint32_t MUX(uint32_t input1, uint32_t input2, bool signal);  // signal == 0) input1, 1) input2
uint32_t MUX_3(uint32_t input1, uint32_t input2, uint32_t input3, const bool signal[]);  // signal == 0) input1, 1) input2, 2) input3
uint32_t MUX_4(uint32_t input1, uint32_t input2, uint32_t input3, uint32_t input4, const bool signal[]);  // signal == 0) input1, 1) input2, 2) input3, 3) input4

/* Control units */
void CtrlUnit(uint8_t opcode, uint8_t funct);  // Control unit
void ALUCtrlUnit(uint8_t funct);  // ALU control unit
void ForwardUnit(uint8_t IDEXrt, uint8_t IDEXrs, uint8_t EXMEMWritereg, uint8_t MEMWBWritereg);
void HazardDetectUnit(uint8_t IFIDrs, uint8_t IFIDrt, uint8_t IDEXrt);  // Hazard detection unit

/* Select ALU operation */
char Rformat(uint8_t funct);  // select ALU operation by funct (R-format)

/* Update prediction bits */
uint8_t PBtaken(uint8_t Predbit);
uint8_t PBnottaken(uint8_t Predbit);

/* Overflow exception */
void OverflowException();  // Overflow exception

#ifndef CAMP_PROJECT3_HEADER_H
#define CAMP_PROJECT3_HEADER_H

#endif //CAMP_PROJECT3_HEADER_H