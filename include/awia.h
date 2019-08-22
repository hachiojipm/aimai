#ifndef _AWIA_INC_GUARD_H_
#define _AWIA_INC_GUARD_H_

#include <Arduino.h>
#include "constants.h"

enum EncSide {
    LEFT,
    RIGHT,
};

struct EncCountStatus {
    int16_t previousCnt;
    int16_t currentCnt;
};

void initRx();
void initTx();
void editRDSText();
void enterRDSTextCharacter();
void escapeRDSTextEditing();
void changeRxVolume();
void changeRxFreq();
void changeTxFreq();
void txLoop(void *arg);
void rxLoop(void *arg);
void nopLoop(void *arg);
void readRDSPeriodically(void *arg);
void loadActionMode();
EncCountStatus _readEncCountStatus(EncSide encSide, volatile byte* pos, volatile int16_t* cnt);

#endif // _AWIA_INC_GUARD_H_
