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
}COUNTING;

void ReadDirectory(char** files);
void FileSelect(char** file);
char PredSelect(void);
char PBSelect(void);
char CounterSelect(void);
void OnelevelPredict(const char* Predictbit);
void GsharePredict(const char* Predictbit);
void AlwaysTaken();
void AlwaysnotTaken(void);
void Firstinit(const char* Predictbit);
void printnextPC(void);
void OnelevelPipelineHandsOver(void);
void GsharePipelineHandsOver(void);
void countingFormat(void);
void printFinalresult(const char* Predictor, const char* Predictbit, const char* filename);

#endif //CAMP_PROJECT3_MAIN_H