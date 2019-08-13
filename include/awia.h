#ifndef _AWIA_INC_GUARD_H_
#define _AWIA_INC_GUARD_H_

#include <Arduino.h>

#define STATION_NAME "JOHN DOE"

#define UNUSED -1

#define SDA_PIN 21
#define SCL_PIN 22
#define RX_RST_PIN 17
#define TX_RST_PIN 16

#define RX_MODE_PIN 32
#define TX_MODE_PIN 33

#define LEFT_ENC_SW_PIN 23
#define LEFT_ENC_PIN_A 18
#define LEFT_ENC_PIN_B 19
#define RIGHT_ENC_SW_PIN 14
#define RIGHT_ENC_PIN_A 12
#define RIGHT_ENC_PIN_B 13

#define JP_MINIMUM_FM_MHZ 761
#define JP_MAXIMUM_FM_MHZ 949
#define TX_POWER 115
#define RDS_TEXT_LENGTH 20
#define RDS_READING_TIMEOUT_MILLIS 10000
#define RDS_READING_PERIOD_MICROS 15000000

#define OLED_ADDR 0x3c

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
void txLoop();
void rxLoop();
void nopLoop();
void loadActionMode();
EncCountStatus _readEncCountStatus(EncSide encSide, volatile byte* pos, volatile int16_t* cnt);

#endif // _AWIA_INC_GUARD_H_
