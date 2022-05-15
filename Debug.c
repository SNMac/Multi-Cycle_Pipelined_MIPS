//
// Created by SNMac on 2022/05/13.
//
#include <stdio.h>
#include <string.h>

#include "Debug.h"
#include "main.h"
#include "Units.h"

DEBUGIF debugif;
DEBUGID debugid[2];
DEBUGEX debugex[2];
DEBUGMEM debugmem[2];
DEBUGWB debugwb[2];

// from main.c
extern uint32_t R[32];

// from Units.c
extern PROGRAM_COUNTER PC;
extern IFID ifid[2];
extern IDEX idex[2];
extern EXMEM exmem[2];
extern MEMWB memwb[2];
extern BRANCH_PREDICT BranchPred;
extern HAZARD_DETECTION_SIGNAL hzrddetectSig;

void printIF(void) {
    printf("\n<<<<<<<<<<<<<<<<<<<<<IF>>>>>>>>>>>>>>>>>>>>>\n");
    if (debugif.IFPC == 0xffffffff) {
        printf("End of program\n");
        printf("Wait for finishing other stage\n");
        printf("**********************************************\n");
        return;
    }

    // Print information of IF
    printf("PC : 0x%08x\n", debugif.IFPC);
    printf("Fetch instruction : 0x%08x\n", debugif.IFinst);



    printf("**********************************************\n");

    // Hands over data to next debug stage
    debugid[0].IDPC = debugif.IFPC; debugid[0].IDinst = debugif.IFinst;
    return;
}

void printID(void) {
    printf("\n<<<<<<<<<<<<<<<<<<<<<ID>>>>>>>>>>>>>>>>>>>>>\n");
    if (!(debugid[1].valid)) {  // Pipeline is invalid
        printf("IF/ID pipeline is invalid\n");
        printf("**********************************************\n");
        return;
    }

    // Print information of ID
    printf("Processing PC : 0x%08x\n", debugid[1].IDPC);
    printf("Decode instruction : 0x%08x\n", debugid[1].IDinst);
    memset(debugid[1].instprint, 0, sizeof(debugid[1].instprint));
    switch (debugid[1].inst.opcode) {
        case 0x0 :  // R-format
            printRformat(debugid[1].inst.funct);
            break;

        case 0x8 :  // addi
            snprintf(debugid[1].instprint, sizeof(debugid[1].instprint),
                     "format : I   |   addi $%d, $%d, 0x%x", debugid[1].inst.rt, debugid[1].inst.rs, debugid[1].inst.imm);
            break;

        case 0x9 :  // addiu
            snprintf(debugid[1].instprint, sizeof(debugid[1].instprint),
                     "format : I   |   addiu $%d, $%d, 0x%x", debugid[1].inst.rt, debugid[1].inst.rs, debugid[1].inst.imm);
            break;

        case 0xc :  // andi
            snprintf(debugid[1].instprint, sizeof(debugid[1].instprint),
                     "format : I   |   andi $%d, $%d, 0x%x", debugid[1].inst.rt, debugid[1].inst.rs, debugid[1].inst.imm);
            break;

        case 0x4 :  // beq
            snprintf(debugid[1].instprint, sizeof(debugid[1].instprint),
                     "format : I   |   beq $%d, $%d, 0x%x", debugid[1].inst.rt, debugid[1].inst.rs, debugid[1].inst.imm);
            break;

        case 0x5 :  // bne
            snprintf(debugid[1].instprint, sizeof(debugid[1].instprint),
                     "format : I   |   bne $%d, $%d, 0x%x", debugid[1].inst.rt, debugid[1].inst.rs, debugid[1].inst.imm);
            break;

        case 0x2 :  // j
            snprintf(debugid[1].instprint, sizeof(debugid[1].instprint),
                     "format : J   |   j 0x%08x", debugid[1].inst.address);
            break;

        case 0x3 :  // jal
            snprintf(debugid[1].instprint, sizeof(debugid[1].instprint),
                     "format : J   |   jal 0x%08x", debugid[1].inst.address);
            break;

        case 0xf :  // lui
            snprintf(debugid[1].instprint, sizeof(debugid[1].instprint),
                     "format : I   |   lui $%d, 0x%x", debugid[1].inst.rt, debugid[1].inst.imm);
            break;

        case 0x23 :  // lw
            snprintf(debugid[1].instprint, sizeof(debugid[1].instprint),
                     "format : I   |   lw $%d, 0x%x($%d)", debugid[1].inst.rt, debugid[1].inst.imm, debugid[1].inst.rs);
            break;

        case 0xd :  // ori
            snprintf(debugid[1].instprint, sizeof(debugid[1].instprint),
                     "format : I   |   ori $%d, $%d, 0x%x", debugid[1].inst.rt, debugid[1].inst.rs, debugid[1].inst.imm);
            break;

        case 0xa :  // slti
            snprintf(debugid[1].instprint, sizeof(debugid[1].instprint),
                     "format : I   |   slti $%d, $%d, 0x%x", debugid[1].inst.rt, debugid[1].inst.rs, debugid[1].inst.imm);
            break;

        case 0xb :  // sltiu
            snprintf(debugid[1].instprint, sizeof(debugid[1].instprint),
                     "format : I   |   sltiu $%d, $%d, 0x%x", debugid[1].inst.rt, debugid[1].inst.rs, debugid[1].inst.imm);
            break;

        case 0x2B :  // sw
            snprintf(debugid[1].instprint, sizeof(debugid[1].instprint),
                     "format : I   |   sw $%d, 0x%x($%d)", debugid[1].inst.rt, debugid[1].inst.imm, debugid[1].inst.rs);
            break;

        default :
            break;
    }
    printf("%s\n", debugid[1].instprint);
    printf("**********************************************\n");
    // Hands over data to next debug stage
    debugex[0].ControlNOP = hzrddetectSig.ControlNOP;
    debugex[0].EXPC = debugid[1].IDPC; debugex[0].EXinst = debugid[1].IDinst;
    strcpy(debugex[0].instprint, debugid[1].instprint);
    return;
}

void printEX(void) {
    printf("\n<<<<<<<<<<<<<<<<<<<<<EX>>>>>>>>>>>>>>>>>>>>>\n");
    if (!(debugex[1].valid)) {
        printf("ID/EX pipeline is invalid\n");
        printf("**********************************************\n");
        return;
    }

    // Print information of EX
    printf("Processing PC : 0x%08x\n", debugex[1].EXPC);
    printf("Processing instruction : 0x%08x\n", debugex[1].EXinst);
    if (debugex[1].ControlNOP) {
        printf("Control signals are NOP.\n");
        printf("**********************************************\n");
        return;
    }
    printf("%s\n", debugex[1].instprint);

    printf("ALU input1 : 0x%08x (%u)\nALU input2 : 0x%08x (%u)\n",
           debugex[1].ALUinput1, debugex[1].ALUinput1, debugex[1].ALUinput2, debugex[1].ALUinput2);
    printf("ALU result : 0x%08x (%u)\n", debugex[1].ALUresult, debugex[1].ALUresult);
    printf("**********************************************\n");

    // Hands over data to next debug stage
    debugmem[0].ControlNOP = debugex[1].ControlNOP;
    debugmem[0].MEMPC = debugex[1].EXPC; debugmem[0].MEMinst = debugex[1].EXinst;
    strcpy(debugmem[0].instprint, debugex[1].instprint);
    return;
}

void printMEM(void) {
    printf("\n<<<<<<<<<<<<<<<<<<<<<MEM>>>>>>>>>>>>>>>>>>>>>\n");
    if (!(debugmem[1].valid)) {
        printf("EX/MEM pipeline is invalid\n");
        printf("**********************************************\n");
        return;
    }

    // Print information of MEM
    printf("Processing PC : 0x%08x\n", debugmem[1].MEMPC);
    printf("Processing instruction : 0x%08x\n", debugmem[1].MEMinst);
    if (debugmem[1].ControlNOP) {
        printf("Control signals are NOP.\n");
        printf("**********************************************\n");
        return;
    }
    printf("%s\n", debugmem[1].instprint);

    if (debugmem[1].MemRead) {
        printf("Memory[0x%08x] load -> 0x%x (%d)\n", debugmem[1].Addr, debugmem[1].Writedata, debugmem[1].Writedata);
    }
    else if (debugmem[1].MemWrite) {
        printf("Memory[0x%08x] <- store 0x%x (%d)\n", debugmem[1].Addr, debugmem[1].Writedata, debugmem[1].Writedata);
    }
    else {
        printf("There is no memory access\n");
    }
    printf("**********************************************\n");

    // Hands over data to next debug stage
    debugwb[0].ControlNOP = debugmem[1].ControlNOP;
    debugwb[0].WBPC = debugmem[1].MEMPC; debugwb[0].WBinst = debugmem[1].MEMinst;
    strcpy(debugwb[0].instprint, debugmem[1].instprint);
    return;
}

void printWB(void) {
    printf("\n<<<<<<<<<<<<<<<<<<<<<WB>>>>>>>>>>>>>>>>>>>>>\n");
    if (!(debugwb[1].valid)) {
        printf("MEM/WB pipeline is invalid\n");
        printf("**********************************************\n");
        return;
    }

    printf("Processing PC : 0x%08x\n", debugwb[1].WBPC);
    printf("Processing instruction : 0x%08x\n", debugwb[1].WBinst);
    if (debugwb[1].ControlNOP) {
        printf("Control signals are NOP.\n");
        printf("**********************************************\n");
        return;
    }
    printf("%s\n", debugwb[1].instprint);

    if (debugwb[1].RegWrite) {
        printf("R[%d] = 0x%x (%d)\n", debugwb[1].Writereg, R[debugwb[1].Writereg], R[debugwb[1].Writereg]);
    }
    else {
        printf("There is no register write\n");
    }

    printf("**********************************************\n");
    return;
}



void printRformat(uint8_t funct) {
    switch (funct) {
        case 0x20 :  // add
            snprintf(debugid[1].instprint, sizeof(debugid[1].instprint),
                     "format : R   |   add $%d, $%d, $%d", debugid[1].inst.rd, debugid[1].inst.rs, debugid[1].inst.rt);
            break;

        case 0x21 :  // addu
            snprintf(debugid[1].instprint, sizeof(debugid[1].instprint),
                     "format : R   |   addu $%d, $%d, $%d", debugid[1].inst.rd, debugid[1].inst.rs, debugid[1].inst.rt);
            break;

        case 0x24 :  // and
            snprintf(debugid[1].instprint, sizeof(debugid[1].instprint),
                     "format : R   |   and $%d, $%d, $%d", debugid[1].inst.rd, debugid[1].inst.rs, debugid[1].inst.rt);
            break;

        case 0x08 :  // jr
            snprintf(debugid[1].instprint, sizeof(debugid[1].instprint),
                     "format : R   |   jr $%d", debugid[1].inst.rs);
            break;

        case 0x9 :  // jalr
            snprintf(debugid[1].instprint, sizeof(debugid[1].instprint),
                     "format : R   |   jalr $%d", debugid[1].inst.rs);
            break;

        case 0x27 :  // nor
            snprintf(debugid[1].instprint, sizeof(debugid[1].instprint),
                     "format : R   |   nor $%d, $%d, $%d", debugid[1].inst.rd, debugid[1].inst.rs, debugid[1].inst.rt);
            break;

        case 0x25 :  // or
            snprintf(debugid[1].instprint, sizeof(debugid[1].instprint),
                     "format : R   |   or $%d, $%d, $%d", debugid[1].inst.rd, debugid[1].inst.rs, debugid[1].inst.rt);
            break;

        case 0x2a :  // slt
            snprintf(debugid[1].instprint, sizeof(debugid[1].instprint),
                     "format : R   |   slt $%d, $%d, $%d", debugid[1].inst.rd, debugid[1].inst.rs, debugid[1].inst.rt);
            break;

        case 0x2b :  // sltu
            snprintf(debugid[1].instprint, sizeof(debugid[1].instprint),
                     "format : R   |   add $%d, $%d, $%d", debugid[1].inst.rd, debugid[1].inst.rs, debugid[1].inst.rt);
            printf("format : R   |   sltu $%d, $%d, $%d", debugid[1].inst.rd, debugid[1].inst.rs, debugid[1].inst.rt);
            break;

        case 0x00 :  // sll
            snprintf(debugid[1].instprint, sizeof(debugid[1].instprint),
                     "format : R   |   sll $%d, $%d, %d", debugid[1].inst.rd, debugid[1].inst.rs, debugid[1].inst.rt);
            break;

        case 0x02 :  // srl
            snprintf(debugid[1].instprint, sizeof(debugid[1].instprint),
                     "format : R   |   srl $%d, $%d, %d", debugid[1].inst.rd, debugid[1].inst.rs, debugid[1].inst.rt);
            break;

        case 0x22 :  // sub
            snprintf(debugid[1].instprint, sizeof(debugid[1].instprint),
                     "format : R   |   sub $%d, $%d, $%d", debugid[1].inst.rd, debugid[1].inst.rs, debugid[1].inst.rt);
            break;

        case 0x23 :  // subu
            snprintf(debugid[1].instprint, sizeof(debugid[1].instprint),
                     "format : R   |   subu $%d, $%d, $%d", debugid[1].inst.rd, debugid[1].inst.rs, debugid[1].inst.rt);
            break;

        default:
            break;
    }
    return;
}

void DebugPipelineHandsOver(void) {
    if (!hzrddetectSig.IFIDnotWrite){
        debugid[1] = debugid[0];
        strcpy(debugid[1].instprint, debugid[0].instprint);
    }
    debugex[1] = debugex[0];
    strcpy(debugex[1].instprint, debugex[0].instprint);
    debugmem[1] = debugmem[0];
    strcpy(debugmem[1].instprint, debugmem[0].instprint);
    debugwb[1] = debugwb[0];
    strcpy(debugwb[1].instprint, debugwb[0].instprint);
    return;
}