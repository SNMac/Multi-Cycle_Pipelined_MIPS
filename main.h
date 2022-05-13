//
// Created by SNMac on 2022/05/09.
//

#ifndef CAMP_PROJECT3_MAIN_H
#define CAMP_PROJECT3_MAIN_H

typedef struct _COUNTING {
    int Rcount;  // R-format instruction count
    int Icount;  // I-format instruction count
    int Jcount;  // J-format instruction count
    int Memcount;  // Memory access instruction count
    int PredictHitCount;  // Branch prediction count
    int takenBranch;  // taken Branch count
    int nottakenBranch;  // not taken Branch count
}COUNTING;

#endif //CAMP_PROJECT3_MAIN_H