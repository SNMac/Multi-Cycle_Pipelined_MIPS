//
// Created by SNMac on 2022/05/13.
//

#ifndef CAMP_PROJECT3_STAGES_H
#define CAMP_PROJECT3_STAGES_H

#include <stdint.h>

/* Stages */
void IF(void);  // Instruction Fetch
void ID(void);  // Instruction Decode
void EX(void);  // EXecute
void MEM(void);  // MEMory access
void WB(void);  // Write Back

#endif //CAMP_PROJECT3_STAGES_H
