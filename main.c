//
// Created by SNMac on 2022/05/09.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "main.h"
#include "Units.h"
#include "Stages.h"
#include "Debug.h"

COUNTING counting;  // for result counting

uint32_t Memory[0x400000];
uint32_t R[32];

// from Units.c
extern PROGRAM_COUNTER PC;
extern IFID ifid[2];
extern IDEX idex[2];
extern EXMEM exmem[2];
extern MEMWB memwb[2];
extern BRANCH_PREDICT BranchPred;
extern HAZARD_DETECTION_SIGNAL hzrddetectSig;

// from Debug.c
extern DEBUGID debugid[2];
extern DEBUGEX debugex[2];
extern DEBUGMEM debugmem[2];
extern DEBUGWB debugwb[2];

int main(int argc, char* argv[]) {
    clock_t start = clock();
    char* filename;
    int filenameSelector;
    char PredictorSelector;
    char PredictionBitSelector = '0';
    if (argc == 2) {
        filename = argv[1];
        PredictorSelector = PredSelector();
        if (PredictorSelector == '1' || PredictorSelector == '2') {
            PredictionBitSelector = PBSelector();
        }
    }
    else if (argc == 3) {
        filename = argv[1];
        PredictorSelector = *argv[2];
        if (PredictorSelector == '1' || PredictorSelector == '2') {
            PredictionBitSelector = PBSelector();
        }
    }
    else if (argc >= 4) {
        filename = argv[1];
        PredictorSelector = *argv[2];
        if (PredictorSelector == '1' || PredictorSelector == '2') {
            PredictionBitSelector = *argv[3];
        }
    }
    // TODO
    //  make counter selector

    // TODO
    //  make BTFNT predictor
    else {
        printf("###################################################\n");
        printf("1 : simple.bin,  2 : simple2.bin,  3 : simple3.bin\n");
        printf("4 : simple4.bin, 5 : fib.bin,      6 : fib_jalr.bin\n");
        printf("7 : gcd.bin,     8 : input4.bin\n");
        printf("###################################################\n");
        printf("\nSelect filename : \n");
        scanf("%d", &filenameSelector);
        getchar();
        switch (filenameSelector) {
            case 1 :
                filename = "simple.bin";
                break;

            case 2 :
                filename = "simple2.bin";
                break;

            case 3 :
                filename = "simple3.bin";
                break;

            case 4 :
                filename = "simple4.bin";
                break;

            case 5 :
                filename = "fib.bin";
                break;

            case 6 :
                filename = "fib_jalr.bin";
                break;

            case 7 :
                filename = "gcd.bin";
                break;

            case 8 :
                filename = "input4.bin";
                break;

            default :
                fprintf(stderr, "ERROR: Wrong filename select number\n");
                break;
        }
        PredictorSelector = PredSelector();
        if (PredictorSelector == '1' || PredictorSelector == '2') {
            PredictionBitSelector = PBSelector();
        }
    }

    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
	    perror("ERROR");
	    exit(EXIT_FAILURE);
    }
    int buffer;
    int index = 0;
    int ret;
    while (1) {
        ret = fread(&buffer, sizeof(buffer), 1, fp);
        if (ret != 1) {  // EOF or error
        break;
        }
        Memory[index] = __builtin_bswap32(buffer);  // Little endian to Big endian
        index++;
    }
    if (index == 0) {
        goto END;
    }

    for (int i = 0; i < index; i++) {
        printf("Memory [%d] : 0x%08x\n", i, Memory[i]);
    }

    Firstinit(&PredictionBitSelector);

    switch (PredictorSelector) {
        case '1' :
            OnelevelPredict(&PredictionBitSelector);
            break;

        case '2' :
            GsharePredict(&PredictionBitSelector);
            break;

        case '3' :
            AlwaysTaken();
            break;

        case '4' :
            AlwaysnotTaken();
            break;

        default :
            fprintf(stderr, "ERROR: Predictor select number is wrong.\n");
            exit(EXIT_FAILURE);
    }

    END:
    printFinalresult(&PredictorSelector, &PredictionBitSelector);
    fclose(fp);
    clock_t end = clock();
    printf("Execution time : %lf\n", (double)(end - start) / CLOCKS_PER_SEC);
    return 0;
}

// Predictor select
char PredSelector(void) {
    char retVal;
    printf("\n#############################################################################\n");
    printf("1 : One-level Branch Predictor, 2 : Two-level Global History Branch Predictor\n");
    printf("3 : Always Taken Predictor,     4 : Always Not Taken Predictor               \n");
    printf("#############################################################################\n");
    printf("\nSelect branch predictor : \n");
    scanf("%c", &retVal);
    getchar();
    if (retVal == '1' || retVal == '2' || retVal == '3' || retVal == '4') {
        return retVal;
    }
    else {
        fprintf(stderr, "ERROR: Wrong branch select number\n");
        exit(EXIT_FAILURE);
    }
}

// Prediction bit select
char PBSelector(void) {
    char retVal;
    printf("\n##########################################\n");
    printf("1 : 1-bit prediction, 2 : 2-bit prediction\n");
    printf("##########################################\n");
    printf("\nSelect prediction bit : \n");
    scanf("%c", &retVal);
    getchar();
    if (retVal == '1' || retVal == '2') {
        return retVal;
    }
    else {
        fprintf(stderr, "ERROR: Wrong prediction bit select number\n");
        exit(EXIT_FAILURE);
    }
}

// One-level predictor
void OnelevelPredict(const char* Predictbit) {
    while(1) {
        if (!(ifid[0].valid | idex[0].valid | exmem[0].valid | memwb[0].valid)) {
            return;
        }

        OnelevelIF(Predictbit);
        OnelevelID(Predictbit);
        EX();
        MEM();
        WB();
        printIF(1);
        printID(1, Predictbit);
        printEX();
        printMEM();
        printWB();
        printnextPC();

        countingFormat();
        OnelevelPipelineHandsOver();
        DebugPipelineHandsOver();

        counting.cycle++;
        printf("\n================================ CC %d ================================\n", counting.cycle);
    }
}

// Gshare predictor execute
void GsharePredict(const char* Predictbit) {
    while(1) {
        if (!(ifid[0].valid | idex[0].valid | exmem[0].valid | memwb[0].valid)) {
            return;
        }
        GshareIF(Predictbit);
        GshareID(Predictbit);
        EX();
        MEM();
        WB();
        printIF(2);
        printID(2, Predictbit);
        printEX();
        printMEM();
        printWB();
        printnextPC();

        countingFormat();
        GsharePipelineHandsOver();
        DebugPipelineHandsOver();

        counting.cycle++;
        printf("\n================================ CC %d ================================\n", counting.cycle);
    }
}

// Always taken predictor execute
void AlwaysTaken(void) {
    while(1) {
        if (!(ifid[0].valid | idex[0].valid | exmem[0].valid | memwb[0].valid)) {
            return;
        }

        AlwaysTakenIF();
        AlwaysTakenID();
        EX();
        MEM();
        WB();
        printIF(3);
        printID(3, 0);
        printEX();
        printMEM();
        printWB();
        printnextPC();

        countingFormat();
        OnelevelPipelineHandsOver();
        DebugPipelineHandsOver();

        counting.cycle++;
        printf("\n================================ CC %d ================================\n", counting.cycle);
    }
}

// Always not taken predictor execute
void AlwaysnotTaken(void) {
    while(1) {
        if (!(ifid[0].valid | idex[0].valid | exmem[0].valid | memwb[0].valid)) {
            return;
        }

        AlwaysnotTakenIF();
        AlwaysnotTakenID();
        EX();
        MEM();
        WB();
        printIF(4);
        printID(4, 0);
        printEX();
        printMEM();
        printWB();
        printnextPC();

        countingFormat();
        OnelevelPipelineHandsOver();
        DebugPipelineHandsOver();

        counting.cycle++;
        printf("\n================================ CC %d ================================\n", counting.cycle);
    }
}

void Firstinit(const char* Predictbit) {
    memset(&PC, 0, sizeof(PROGRAM_COUNTER));
    memset(&BranchPred, 0, sizeof(BRANCH_PREDICT));
    switch (*Predictbit) {
        case '0' :  // Always taken or  not taken
            break;

        case '1' :  // One-bit predictor
            for (int i = 0; i < BTBMAX; i++) {
                BranchPred.DP[i][1] = 0;  // Initialize Direction Predictor to 1
            }
            for (int i = 0; i < BHTMAX; i++) {
                BranchPred.BHT[i][0] = i;
                BranchPred.BHT[i][1] = 0;  // Initialize Branch History Table to 1
            }
            break;

        case '2' :  // Two-bit predictor
            for (int i = 0; i < BTBMAX; i++) {
                BranchPred.DP[i][1] = 1;  // Initialize Direction Predictor to 1
            }
            for (int i = 0; i < BHTMAX; i++) {
                BranchPred.BHT[i][0] = i;
                BranchPred.BHT[i][1] = 1;  // Initialize Branch History Table to 1
            }
            break;

        default :
            fprintf(stderr, "ERROR: Wrong prediction bit select number\n");
            exit(EXIT_FAILURE);
    }
    PC.valid = 1;
    ifid[0].valid = 1;
    R[31] = 0xFFFFFFFF;  // $ra = 0xFFFFFFFF
    R[29] = 0x1000000;  // $sp = 0x1000000
    return;
}

void printnextPC(void) {
    printf("\n##########################\n");
    if (PC.valid & !hzrddetectSig.PCnotWrite) {
        printf("## Next PC = 0x%08x ##\n", PC.PC);
    }
    else {
        printf("## !!! No PC update !!! ##\n");
    }
    printf("##########################\n");
    return;
}

void OnelevelPipelineHandsOver(void) {
    if (!hzrddetectSig.IFIDnotWrite){
        ifid[1] = ifid[0];  // IF/ID pipeline hands data to ID
    }

    if (!hzrddetectSig.BTBnotWrite) {
        BranchPred.Predict[1] = BranchPred.Predict[0];
        BranchPred.AddressHit[1] = BranchPred.AddressHit[0];
        BranchPred.BTBindex[1] = BranchPred.BTBindex[0];
        BranchPred.DPindex[1] = BranchPred.DPindex[0];
        BranchPred.instPC[1] = BranchPred.instPC[0];
    }

    idex[1] = idex[0];  // ID/EX pipeline hands data to EX
    exmem[1] = exmem[0];  // EX/MEM pipeline hands data to MEM
    memwb[1] = memwb[0];  // MEM/WB pipeline hands data to WB
    return;
}

void GsharePipelineHandsOver(void) {
    if (!hzrddetectSig.IFIDnotWrite){
        ifid[1] = ifid[0];  // IF/ID pipeline hands data to ID
    }

    if (!hzrddetectSig.BTBnotWrite) {
        BranchPred.Predict[1] = BranchPred.Predict[0];
        BranchPred.AddressHit[1] = BranchPred.AddressHit[0];
        BranchPred.BTBindex[1] = BranchPred.BTBindex[0];
        BranchPred.BHTindex[1] = BranchPred.BHTindex[0];
        BranchPred.instPC[1] = BranchPred.instPC[0];
    }

    idex[1] = idex[0];  // ID/EX pipeline hands data to EX
    exmem[1] = exmem[0];  // EX/MEM pipeline hands data to MEM
    memwb[1] = memwb[0];  // MEM/WB pipeline hands data to WB
    return;
}

void countingFormat(void) {
    if (!hzrddetectSig.IFIDnotWrite){
        if (ifid[1].valid) {
            switch (counting.format) {
                case 'R' :
                    counting.Rcount++;
                    break;

                case 'I' :
                    counting.Icount++;
                    break;

                case 'J' :
                    counting.Jcount++;
                    break;

                default :
                    fprintf(stderr, "ERROR: Instruction has wrong format.\n");
                    exit(EXIT_FAILURE);
            }
        }
    }
    return;
}

void printFinalresult(const char* Predictor, const char* Predictbit) {
    printf("\n\n===============================================================\n");
    printf("===============================================================\n");
    printf("<<<<<<<<<<<<<<<<<<<<<<<<End of execution>>>>>>>>>>>>>>>>>>>>>>>>\n");

    // Print Registers
    printf("\n\n################ Registers #################\n");
    for (int i = 0; i < 32; i += 2) {
        printf("R[%d] : %d   |   R[%d] : %d\n", i, R[i], i + 1, R[i + 1]);
    }
    printf("############################################\n");

    switch (*Predictor) {
        case '1' :  // One-level predictor
            // Print DP
            printf("\n\n######### Direction Predictor #########\n");
            printf("##   Branch PCs   ## Prediction bits ##\n");
            for (int i = 0; i < BranchPred.DPsize; i++) {
                printf("##    0x%08x  ##        %d        ##\n", BranchPred.DP[i][0], BranchPred.DP[i][1]);
            }
            printf("#######################################\n");
            // Print BTB
            printf("\n\n################# Branch Target Buffer #################\n");
            printf("## Branch PCs ## Predicted target ## Frequency of use ##\n");
            for (int i = 0; i < BranchPred.BTBsize; i++) {
                printf("## 0x%08x ##    0x%08x    ## %16d ##\n", BranchPred.BTB[i][0], BranchPred.BTB[i][1], BranchPred.BTB[i][2]);
            }
            printf("########################################################\n");
            break;

        case '2' :  // Gshare predictor
            // Print BHT
            printf("\n\n###### Branch History Table #######\n");
            printf("##   Index  ##  Prediction bits  ##\n");
            for (int i = 0; i < BHTMAX; i++) {
                printf("##    ");
                for (int j = 3; j >= 0; j--) {
                    printf("%d", i >> j & 1);
                }
                printf("  ##         %d         ##\n", BranchPred.BHT[i][1]);
            }
            printf("###################################\n");
            // Print BTB
            printf("\n\n################# Branch Target Buffer #################\n");
            printf("## Branch PCs ## Predicted target ## Frequency of use ##\n");
            for (int i = 0; i < BranchPred.BTBsize; i++) {
                printf("## 0x%08x ##    0x%08x    ## %16d ##\n", BranchPred.BTB[i][0], BranchPred.BTB[i][1], BranchPred.BTB[i][2]);
            }
            printf("########################################################\n");
            break;

        case '3' :
            break;

        case '4' :
            break;

        default :
            fprintf(stderr, "ERROR: Wrong predictor select number\n");
            exit(EXIT_FAILURE);
    }

    // Calculate Branch Hit rate
    double BranchHitrate = 0;
    if ((counting.takenBranch + counting.nottakenBranch) != 0) {
        BranchHitrate = (double)counting.PredictHitCount / (double)(counting.takenBranch + counting.nottakenBranch) * 100;
    }

    // Calculate Cycles Per Instruction
    double CPI = (double)counting.cycle / (double)(counting.Rcount + counting.Icount + counting.Jcount);

    // Print summary
    switch (*Predictor) {
        case '1' :
            printf("\n\nBranch predictor : One-level\n");
            break;

        case '2' :
            printf("\n\nBranch predictor : Gshare\n");
            break;

        case '3' :
            printf("\n\nBranch predictor : Always taken\n");
            break;

        case '4' :
            printf("\n\nBranch predictor : Always not-taken\n");
            break;
    }
    switch (*Predictbit) {
        case '0' :
            break;

        case '1' :
            printf("Prediction bit : 1bit\n");
            break;

        case '2' :
            printf("Prediction bit : 2bit\n");
            break;
    }
    printf("Final return value R[2] = %d\n", R[2]);
    printf("# of clock cycles : %d\n", counting.cycle);
    printf("# of executed instructions : %d\n", counting.Rcount + counting.Icount + counting.Jcount);
    printf("Cylces Per Instruction(CPI) : %.2lf\n", CPI);
    printf("# of executed R-format instructions : %d\n", counting.Rcount);
    printf("# of executed I-format instructions : %d\n", counting.Icount);
    printf("# of executed J-format instructions : %d\n", counting.Jcount);
    printf("# of memory operation instructions : %d\n", counting.Memcount);
    printf("# of taken branches : %d\n", counting.takenBranch);
    printf("# of not taken branches : %d\n", counting.nottakenBranch);
    printf("# of branch prediction HIT : %d\n", counting.PredictHitCount);
    printf("Hit rate of branch prediction : %.2lf %%\n", BranchHitrate);
    return;
}