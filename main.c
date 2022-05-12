#include "header.h"
#include <time.h>

IFID ifid[2];
IDEX idex[2];
EXMEM exmem[2];
MEMWB memwb[2];
HAZARD_DETECTION_SIGNAL hzrddetectSig;
BRANCH_PREDICT BranchPred;


uint32_t Memory[0x400000];
uint32_t PC; bool PCvalid;
uint32_t R[32];

// for result counting
COUNTING counting;

// for checking
DEBUGID debugid[2];
DEBUGEX debugex[2];
DEBUGMEM debugmem[2];
DEBUGWB debugwb[2];

int main(int argc, char* argv[]) {
    clock_t start = clock();
    char* filename;
    if (argc == 2) {
        filename = argv[1];
    }
    else {
        filename = "Simple3.bin";
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
  
    int cycle = 0;  // to count clock cycle
    for (int i = 0; i < index; i++) {
        printf("Memory [%d] : 0x%08x\n", i, Memory[i]);
    }

    for (int i = 0; i < BTBMAX; i++) {
        BranchPred.BTB[i][2] = 1;  // Initialize predict bits to 1
    }
    ifid[0].valid = 1;
    PCvalid = 1;
    R[31] = 0xFFFFFFFF;  // $ra = 0xFFFFFFFF
    R[29] = 0x1000000;  // $sp = 0x1000000
    while(index != 0) {
        if (!(ifid[0].valid | idex[0].valid | exmem[0].valid | memwb[0].valid)) {
            break;
        }
        IF();
        ID();
        EX();
        MEM();
        WB();
        printf("\n##########################\n");
        if (PCvalid & !(hzrddetectSig.PCnotWrite)) {
            printf("## Next PC = 0x%08x ##\n", PC);
        }
        else {
            printf("## !!! No PC update !!! ##\n");
        }
        printf("##########################\n");
//        IFIDDebug();
//        IDEXDebug();
//        EXMEMDebug();
//        MEMWBDebug();
        if (!hzrddetectSig.IFIDnotWrite){
            debugid[1] = debugid[0];
            ifid[1] = ifid[0];  // IF/ID pipeline hands data to ID
        }
        idex[1] = idex[0];  // ID/EX pipeline hands data to EX
        exmem[1] = exmem[0];  // EX/MEM pipeline hands data to MEM
        memwb[1] = memwb[0];  // MEM/WB pipeline hands data to WB
        debugex[1] = debugex[0];
        debugmem[1] = debugmem[0];
        debugwb[1] = debugwb[0];
        cycle++;
        printf("\n======================================== CC %d ========================================\n", cycle);
    }
    printf("===============================================================\n");
    printf("===============================================================\n");
    printf("\n<<<<<<<<<<<<<<End of execution>>>>>>>>>>>>>>\n");

    for (int i = 0; i < 32; i += 2) {
        printf("\nR[%d] : %d   |   R[%d] : %d", i, R[i], i + 1, R[i + 1]);
    }
    printf("\n\n########################## Branch Target Buffer ###########################\n");
    printf("## Branch PCs ## Predicted target ## Prediction bits ## Frequency of use ##\n");
    for (int i = 0; i < BranchPred.BTBsize; i++) {
        printf("## 0x%08x ##    0x%08x    ##%16d ## %16d ##\n", BranchPred.BTB[i][0], BranchPred.BTB[i][1], BranchPred.BTB[i][2], BranchPred.BTB[i][3]);
    }
    double BranchHitrate = (double)counting.PredictHitCount / (double)(counting.takenBranch + counting.nottakenBranch) * 100;
    printf("###########################################################################\n");
    printf("\n\nFinal return value R[2] = %d\n", R[2]);
    printf("# of clock cycles : %d\n", cycle);
    printf("# of executed instructions : %d\n", counting.Rcount + counting.Icount + counting.Jcount);
    printf("# of executed R-format instructions : %d\n", counting.Rcount);
    printf("# of executed I-format instructions : %d\n", counting.Icount);
    printf("# of executed J-format instructions : %d\n", counting.Jcount);
    printf("# of memory operation instructions : %d\n", counting.Memcount);
    printf("# of taken branches : %d\n", counting.takenBranch);
    printf("# of not taken branches : %d\n", counting.nottakenBranch);
    printf("# of branch prediction HIT : %d\n", counting.PredictHitCount);
    printf("Hit rate of branch prediction : %.2lf %%\n", BranchHitrate);
    fclose(fp);
    clock_t end = clock();
    printf("Execution time : %lf\n", (double)(end - start) / CLOCKS_PER_SEC);
    return 0;
}