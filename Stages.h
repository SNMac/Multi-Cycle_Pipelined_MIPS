//
// Created by SNMac on 2022/05/13.
//

#ifndef CAMP_PROJECT3_STAGES_H
#define CAMP_PROJECT3_STAGES_H

#include "stdint.h"

/* Instruction */
typedef struct _INSTRUCTION {
    uint32_t address;
    uint8_t opcode;
    uint8_t rs;
    uint8_t rt;
    uint8_t rd;
    uint8_t shamt;
    uint16_t imm;
    uint8_t funct;
}INSTRUCTION;

/* Stages */
void IF(void);  // Instruction Fetch
void ID(void);  // Instruction Decode
void EX(void);  // EXecute
void MEM(void);  // MEMory access
void WB(void);  // Write Back

#endif //CAMP_PROJECT3_STAGES_H
