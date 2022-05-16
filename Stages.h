//
// Created by SNMac on 2022/05/13.
//

#ifndef CAMP_PROJECT3_STAGES_H
#define CAMP_PROJECT3_STAGES_H

#include <stdint.h>

/* Stages */
void OnelevelIF(const char* Predictbit);  // Instruction Fetch (One Level Predictor)
void OnelevelID(const char* Predictbit);  // Instruction Decode (One Level Predictor)
void GshareIF(const char* Predictbit);  // Instruction Fetch (Gshare Predictor)
void GshareID(const char* Predictbit);  // Instruction Decode (Gshare Predictor)
void AlwaysTakenIF(void);
void AlwaysnotTakenIF(void);
void AlwaysTakenID(void);
void AlwaysnotTakenID(void);
void EX(void);  // EXecute
void MEM(void);  // MEMory access
void WB(void);  // Write Back

// make GHR index (2^GHRs)
uint8_t makeGHRindex(const bool GHR[], uint32_t PCvalue);

#endif //CAMP_PROJECT3_STAGES_H
