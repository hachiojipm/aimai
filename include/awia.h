#ifndef _AWIA_INC_GUARD_H_
#define _AWIA_INC_GUARD_H_

#include <Arduino.h>

#define LEFT_ENC_PIN_A 32
#define LEFT_ENC_PIN_B 33
#define RIGHT_ENC_PIN_A 25
#define RIGHT_ENC_PIN_B 26
#define JP_MINIMUM_FM_MHZ 761
#define JP_MAXIMUM_FM_MHZ 949

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
void readEncForTextInput();
void changeRxVolume();
void changeRxFreq();
EncCountStatus _readEncCountStatus(EncSide encSide, volatile byte* pos, volatile int16_t* cnt);

#endif // _AWIA_INC_GUARD_H_