//
// Created by SNMac on 2022/05/09.
//

#ifndef CAMP_PROJECT3_MAIN_H
#define CAMP_PROJECT3_MAIN_H

typedef struct _COUNTING {
    char format;
    int cycle;
    int Rcount;  // R-format instruction count
    int Icount;  // I-format instruction count
    int Jcount;  // J-format instruction count
    int Memcount;  // Memory access instruction count
    int PredictHitCount;  // Branch prediction count
    int takenBranch;  // taken Branch count
    int nottakenBranch;  // not taken Branch count
    int stall;
}COUNTING;

void FileSelect(char** name);
void ReadDirectory(char** files, char** directory);
char PredSelect(void);
char PBSelect(void);
char CounterSelect(void);

void OnelevelPredict(const char* Predictbit, const char* Counter);
void GsharePredict(const char* Predictbit, const char* Counter);
void LocalPredict(const char* Predictbit, const char* Counter);
void AlwaysTaken(void);
void AlwaysnotTaken(void);
void BTFNT(void);

void Firstinit(const char* Predictbit);
void printnextPC(void);

void OnelevelPipelineHandsOver(void);
void GsharePipelineHandsOver(void);
void LocalPipelineHandsOver(void);
void AlwaysTakenPipelineHandsOver(void);
void BTFNTPipelineHandsOver(void);

void countingFormat(void);
void printFinalresult(const char* Predictor, const char* Predictbit,
                      const char* filename, const char* Counter);

#endif //CAMP_PROJECT3_MAIN_H