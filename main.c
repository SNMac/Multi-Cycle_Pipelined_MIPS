#include "header.h"
#include <time.h>

IFID ifid[2];
IDEX idex[2];
EXMEM exmem[2];
MEMWB memwb[2];
BRANCH_PREDICT BranchPred;

uint32_t Memory[0x400000];
uint32_t PC; bool PCvalid;
uint32_t R[32];
int Rcount;  // R-format instruction count
int Icount;  // I-format instruction count
int Jcount;  // J-format instruction count
int Memcount;  // Memory access instruction count
int takenBranch;  // taken Branch count
int nottakenBranch;  // not taken Branch count

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
        filename = "Simple2.bin";
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
        ifid[1] = ifid[0];  // IF/ID pipeline hands data to ID
        idex[1] = idex[0];  // ID/EX pipeline hands data to EX
        exmem[1] = exmem[0];  // EX/MEM pipeline hands data to MEM
        memwb[1] = memwb[0];  // MEM/WB pipeline hands data to WB
        debugid[1] = debugid[0];
        debugex[1] = debugex[0];
        debugmem[1] = debugmem[0];
        debugwb[1] = debugwb[0];
        printf("\n");
        IFIDDebug();
        printf("\n");
        IDEXDebug();
        printf("\n");
        EXMEMDebug();
        printf("\n");
        MEMWBDebug();
        printf("\n");
        cycle++;
        printf("\n==================== CC %d ====================\n", cycle);
    }
    printf("===========================================\n");
    printf("===========================================\n");
    printf("\n<<<<<<<<<<<<<<End of execution>>>>>>>>>>>>>>\n");

    for (int i = 0; i < 32; i += 2) {
        printf("\nR[%d] : %d   |   R[%d] : %d", i, R[i], i + 1, R[i + 1]);
    }
    printf("\n\n###########################################################################\n");
    printf("## Branch PCs ## Predicted target ## Prediction bits ## Frequency of use ##\n");
    for (int i = 0; i < BranchPred.BTBsize; i++) {
        printf("## 0x%08x ##    0x%08x    ##        %d        ##         %d        ##\n", BranchPred.BTB[i][0], BranchPred.BTB[i][1], BranchPred.BTB[i][2], BranchPred.BTB[i][3]);
    }
    printf("###########################################################################\n");
    printf("\n\nFinal return value R[2] = %d\n", R[2]);
    printf("# of executed instructions : %d\n", Rcount + Icount + Jcount);
    printf("# of executed R-format instructions : %d\n", Rcount);
    printf("# of executed I-format instructions : %d\n", Icount);
    printf("# of executed J-format instructions : %d\n", Jcount);
    printf("# of memory operation instructions : %d\n", Memcount);
    printf("# of taken branches : %d\n", takenBranch);
    printf("# of not taken branches : %d\n", nottakenBranch);
    printf("# of clock cycles : %d\n", cycle);
    fclose(fp);
    clock_t end = clock();
    printf("Execution time : %lf\n", (double)(end - start) / CLOCKS_PER_SEC);
    return 0;
}