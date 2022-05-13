//
// Created by SNMac on 2022/05/13.
//

#ifndef CAMP_PROJECT3_DEBUG_H
#define CAMP_PROJECT3_DEBUG_H

#include <stdint.h>

/* Visible states */
typedef struct _DEBUGID {
    uint32_t IDPC;
    uint32_t IDinst;
}DEBUGID;

typedef struct _DEBUGEX {
    uint32_t EXPC;
    uint32_t EXinst;
}DEBUGEX;

typedef struct _DEBUGMEM {
    uint32_t MEMPC;
    uint32_t MEMinst;
}DEBUGMEM;

typedef struct _DEBUGWB {
    uint32_t WBPC;
    uint32_t WBinst;
}DEBUGWB;

void IFIDDebug(void);
void IDEXDebug(void);
void EXMEMDebug(void);
void MEMWBDebug(void);

#endif //CAMP_PROJECT3_DEBUG_H
