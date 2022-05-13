//
// Created by SNMac on 2022/05/13.
//
#include <stdio.h>
#include "Units.h"
#include "Debug.h"

// from Units.c
extern IFID ifid[2];
extern IDEX idex[2];
extern EXMEM exmem[2];
extern MEMWB memwb[2];

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