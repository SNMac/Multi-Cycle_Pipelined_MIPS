//
// Created by SNMac on 2022/05/09.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>

#include "main.h"
#include "Units.h"
#include "Stages.h"
#include "Debug.h"

#define DIRECTORY "../testbin"

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
    char PredictorSelector;
    char PredictionBitSelector = '0';
    char CounterSelector;

    if (argc == 2) {
        filename = argv[1];
        PredictorSelector = PredSelect();
        if (PredictorSelector == '1' || PredictorSelector == '2') {
            PredictionBitSelector = PBSelect();
        }
    }
    else if (argc == 3) {
        filename = argv[1];
        PredictorSelector = *argv[2];
        if (PredictorSelector == '1' || PredictorSelector == '2') {
            PredictionBitSelector = PBSelect();
        }
    }
    else if (argc == 4) {
        filename = argv[1];
        PredictorSelector = *argv[2];
        if (PredictorSelector == '1' || PredictorSelector == '2') {
            PredictionBitSelector = *argv[3];
        }
    }
    else if (argc >= 5) {
        filename = argv[1];
        PredictorSelector = *argv[2];
        if (PredictorSelector == '1' || PredictorSelector == '2') {
            PredictionBitSelector = *argv[3];
            CounterSelector = *argv[4];
        }
    }
    // TODO
    //  make counter selector

    // TODO
    //  make BTFNT predictor
    else {
        FileSelect(&filename);
        PredictorSelector = PredSelect();
        if (PredictorSelector == '1' || PredictorSelector == '2') {
            PredictionBitSelector = PBSelect();
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

        case '5' :
            BTFNT();
            break;

        default :
            fprintf(stderr, "ERROR: main) char PredictorSelector is wrong.\n");
            exit(EXIT_FAILURE);
    }

    END:
    printFinalresult(&PredictorSelector, &PredictionBitSelector, filename);
    fclose(fp);
    clock_t end = clock();
    printf("Execution time : %lf\n", (double)(end - start) / CLOCKS_PER_SEC);
    return 0;
}

// Read designated directory
void ReadDirectory(char** files) {
    int index = 0;
    DIR *dir;
    char* ext;
    struct dirent *ent;
    dir = opendir(DIRECTORY);

    if (dir != NULL) { /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL) {
            if ((ext = strrchr(ent->d_name, '.')) == NULL) {
                continue;
            }
            if (strcmp(ext, ".bin") == 0) {
                if (index != 0 && index % 3 != 0) {
                    printf(", ");
                }
                files[index] = ent->d_name;
                printf("%d : %s", index + 1, files[index]);
                index++;
                if (index % 3 == 0) {
                    printf("\n");
                }
            }
        }
        printf("\n");
        closedir(dir);
    }
    else { /* could not open directory */
        perror ("ERROR");
        exit(EXIT_FAILURE);
    }
}

// Filename select
void FileSelect(char** name) {
    char* files[10];
    memset(files, 0, sizeof(files));
    printf("###################################################\n");
    ReadDirectory(files);
    int filenameSelector;
    printf("###################################################\n");
    while (1) {
        printf("\nSelect filename : ");
        scanf("%d", &filenameSelector);
        getchar();
        if (filenameSelector < 1 || files[filenameSelector - 1] == NULL) {
            printf("User inputted wrong number. Please try again.\n");
        }
        else {
            *name = files[filenameSelector - 1];
            break;
        }
    }

}

// Predictor select
char PredSelect(void) {
    char retVal;
    printf("\n#############################################################################\n");
    printf("1 : One-level Branch Predictor, 2 : Two-level Global History Branch Predictor\n");
    printf("3 : Always Taken Predictor,     4 : Always Not Taken Predictor               \n");
    printf("            5 : Backward Taken, Forward Not Taken predictor                  \n");
    printf("#############################################################################\n");
    printf("\nSelect branch predictor : ");
    scanf("%c", &retVal);
    getchar();
    if (retVal == '1' || retVal == '2' || retVal == '3' || retVal == '4' || retVal == '5') {
        return retVal;
    }
    else {
        fprintf(stderr, "ERROR: PredSelect) char retVal is wrong.\n");
        exit(EXIT_FAILURE);
    }
}

// Prediction bit select
char PBSelect(void) {
    char retVal;
    printf("\n##########################################\n");
    printf("1 : 1-bit prediction, 2 : 2-bit prediction\n");
    printf("##########################################\n");
    printf("\nSelect prediction bit : ");
    scanf("%c", &retVal);
    getchar();
    if (retVal == '1' || retVal == '2') {
        return retVal;
    }
    else {
        fprintf(stderr, "ERROR: PBSelect) char retVal is wrong.\n");
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
        AlwaysTakenPipelineHandsOver();
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

void BTFNT(void) {
    while(1) {
        if (!(ifid[0].valid | idex[0].valid | exmem[0].valid | memwb[0].valid)) {
            return;
        }

        BTFNTIF();
        BTFNTID();
        EX();
        MEM();
        WB();
        printIF(5);
        printID(5, 0);
        printEX();
        printMEM();
        printWB();
        printnextPC();

        countingFormat();
        BTFNTPipelineHandsOver();
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
            fprintf(stderr, "ERROR: Firstinit) const char* Predictbit is wrong.\n");
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

void AlwaysTakenPipelineHandsOver(void) {
    if (!hzrddetectSig.IFIDnotWrite){
        ifid[1] = ifid[0];  // IF/ID pipeline hands data to ID
    }

    if (!hzrddetectSig.BTBnotWrite) {
        BranchPred.AddressHit[1] = BranchPred.AddressHit[0];
        BranchPred.BTBindex[1] = BranchPred.BTBindex[0];
        BranchPred.instPC[1] = BranchPred.instPC[0];
    }

    idex[1] = idex[0];  // ID/EX pipeline hands data to EX
    exmem[1] = exmem[0];  // EX/MEM pipeline hands data to MEM
    memwb[1] = memwb[0];  // MEM/WB pipeline hands data to WB
    return;
}

void BTFNTPipelineHandsOver(void) {
    if (!hzrddetectSig.IFIDnotWrite){
        ifid[1] = ifid[0];  // IF/ID pipeline hands data to ID
    }

    if (!hzrddetectSig.BTBnotWrite) {
        BranchPred.Predict[1] = BranchPred.Predict[0];
        BranchPred.AddressHit[1] = BranchPred.AddressHit[0];
        BranchPred.BTBindex[1] = BranchPred.BTBindex[0];
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
                    fprintf(stderr, "ERROR: countingFormat) counting.format is wrong.\n");
                    exit(EXIT_FAILURE);
            }
        }
    }
    return;
}

void printFinalresult(const char* Predictor, const char* Predictbit, const char* filename) {
    printf("\n\n===============================================================\n");
    printf("===============================================================\n");
    printf("<<<<<<<<<<<<<<<<<<<<<<<<End of execution>>>>>>>>>>>>>>>>>>>>>>>>\n");

    // Print Registers
    printf("\n\n################ Registers #################\n");
    for (int i = 0; i < 32; i += 2) {
        printf("R[%d] : %d   |   R[%d] : %d\n", i, R[i], i + 1, R[i + 1]);
    }
    printf("############################################\n");

    // Print selected predictor
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
            // Print nothing
            break;

        case '4' :
            // Print nothing
            break;

        case '5' :
            // Print BTB
            printf("\n\n################# Branch Target Buffer #################\n");
            printf("## Branch PCs ## Predicted target ## Frequency of use ##\n");
            for (int i = 0; i < BranchPred.BTBsize; i++) {
                printf("## 0x%08x ##    0x%08x    ## %16d ##\n", BranchPred.BTB[i][0], BranchPred.BTB[i][1], BranchPred.BTB[i][2]);
            }
            printf("########################################################\n");
            break;

        default :
            fprintf(stderr, "ERROR: printFinalresult) const char* Predictor is wrong.\n");
            exit(EXIT_FAILURE);
    }

    printf("\n\n");
    // Print executed file name
    printf("Selected filename : %s\n", filename);

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
            printf("Branch predictor : One-level\n");
            break;

        case '2' :
            printf("Branch predictor : Gshare\n");
            break;

        case '3' :
            printf("Branch predictor : Always taken\n");
            break;

        case '4' :
            printf("Branch predictor : Always not-taken\n");
            break;

        case '5' :
            printf("Branch predictor : Backward Taken, Forward Not Taken\n");
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
    printf("# of stalling : %d\n", counting.stall);
    return;
}