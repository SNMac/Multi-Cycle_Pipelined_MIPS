#include "header.h"

IFID ifid[2];
IDEX idex[2];
EXMEM exmem[2];
MEMWB memwb[2];

uint32_t Memory[0x400000];
uint32_t PC;
int32_t R[32];
int Rcount;  // R-format instruction count
int Icount;  // I-format instruction count
int Jcount;  // J-format instruction count
int Memcount;  // Memory access instruction count
int Branchcount;  // taken Branch count


int main(int argc, char* argv[]) {
  char* filename;
  if (argc == 2) {
    filename = argv[1];
  }
  else {
    filename = "fib_jalr.bin";
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
  
  R[31] = 0xFFFFFFFF;  // $ra = 0xFFFFFFFF
  R[29] = 0x1000000;  // $sp = 0x1000000
  while(PC != 0xffffffff && index != 0) {
    if (PC > (index - 1) * 4) {  // Exception : PC points over the end of instruction
      fprintf(stderr, "\nERROR: PC points over the end of instructions\n");
      exit(EXIT_FAILURE);
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
    cycle++;
    printf("\n===================CC %d===================\n", cycle);
  }
  printf("===========================================\n");
  printf("===========================================\n");
  printf("\n<<<<<<<<<<<<<<End of execution>>>>>>>>>>>>>>\n");

  for (int i = 0; i < 32; i += 2) {
    printf("\nR[%d] : %d   |   R[%d] : %d", i, R[i], i + 1, R[i + 1]);
  }
  printf("\n\nFinal return value R[2] = %d\n", R[2]);
  printf("# of executed instructions : %d\n", Rcount + Icount + Jcount);
  printf("# of executed R-format instructions : %d\n", Rcount);
  printf("# of executed I-format instructions : %d\n", Icount);
  printf("# of executed J-format instructions : %d\n", Jcount);
  printf("# of memory access instructions : %d\n", Memcount);
  printf("# of taken branches : %d\n", Branchcount);
  printf("# of clock cycles : %d\n", cycle);
  fclose(fp);
  return 0;
}