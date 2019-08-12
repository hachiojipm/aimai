#include "awia.h"
#include <SparkFunSi4703.h>

Si4703_Breakout radio(RX_RST_PIN, SDA_PIN, SCL_PIN, UNUSED);

volatile int16_t rxVol = 0; // TODO load init value from the nonvolatile memory
volatile int16_t rxFreq = JP_MINIMUM_FM_MHZ; // TODO load init value from the nonvolatile memory

void setup() {
    Serial.begin(115200);
    while (!Serial);
    Serial.println("setup...");

    pinMode(RIGHT_ENC_PIN_A, INPUT_PULLUP);
    pinMode(RIGHT_ENC_PIN_B, INPUT_PULLUP);
    pinMode(LEFT_ENC_PIN_A, INPUT_PULLUP);
    pinMode(LEFT_ENC_PIN_B, INPUT_PULLUP);

    initRx();
}

void initRx() {
    detachInterrupt(LEFT_ENC_PIN_A);
    attachInterrupt(LEFT_ENC_PIN_A, changeRxVolume, CHANGE);
    detachInterrupt(LEFT_ENC_PIN_B);
    attachInterrupt(LEFT_ENC_PIN_B, changeRxVolume, CHANGE);

    detachInterrupt(RIGHT_ENC_PIN_A);
    attachInterrupt(RIGHT_ENC_PIN_A, changeRxFreq, CHANGE);
    detachInterrupt(RIGHT_ENC_PIN_B);
    attachInterrupt(RIGHT_ENC_PIN_B, changeRxFreq, CHANGE);

    radio.powerOn();
    radio.setChannel(0);
    radio.setVolume(0);
}

void initTx() {
    detachInterrupt(RIGHT_ENC_PIN_A);
    attachInterrupt(RIGHT_ENC_PIN_A, readEncForTextInput, CHANGE);
    detachInterrupt(RIGHT_ENC_PIN_B);
    attachInterrupt(RIGHT_ENC_PIN_B, readEncForTextInput, CHANGE);
}

bool isInit = true;
int mainLoopRxFreq = 0;
int mainLoopRxVol = 0;

void loop() {
    if (isInit) {
        // TODO read from nonvolatile memory
        radio.setChannel(800);
        radio.setVolume(1);
        isInit = false;
    }

    if (mainLoopRxFreq != rxFreq) {
        Serial.println("freq change change");
        radio.setChannel(rxFreq);
        mainLoopRxFreq = rxFreq;
    }

    if (mainLoopRxVol != rxVol) {
        radio.setVolume(rxVol);
        mainLoopRxVol = rxVol;
    }

    // FIXME implement RDS Rx
}

char runes[40] = {
        ' ',
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
        'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '!', '?', '@',
};
volatile byte posForTextInput;
volatile int16_t textInputCursor = 0;
void readEncForTextInput() {
    EncCountStatus encStatus = _readEncCountStatus(LEFT, &posForTextInput, &textInputCursor);
    if (encStatus.currentCnt != encStatus.previousCnt) {
        if (encStatus.currentCnt < 0) { // alignment
            textInputCursor = 39;
        } else if (encStatus.currentCnt > 39) {
            textInputCursor = 0;
        }
        Serial.println(runes[textInputCursor]);
    }
}

volatile byte posForRxVol;
void changeRxVolume() {
    EncCountStatus encStatus = _readEncCountStatus(LEFT, &posForRxVol, &rxVol);
    if (encStatus.currentCnt != encStatus.previousCnt) {
        if (encStatus.currentCnt <= 0) {
            rxVol = 0;
        } else if (encStatus.currentCnt > 15) {
            rxVol = 15;
        }
        Serial.print("rx vol: ");
        Serial.println(rxVol);
    }
}

volatile byte posForRxFreq;
void changeRxFreq() {
    EncCountStatus encStatus = _readEncCountStatus(RIGHT, &posForRxFreq, &rxFreq);
    if (encStatus.currentCnt != encStatus.previousCnt) {
        if (encStatus.currentCnt < JP_MINIMUM_FM_MHZ) {
            rxFreq = JP_MAXIMUM_FM_MHZ;
        } else if (encStatus.currentCnt > JP_MAXIMUM_FM_MHZ) {
            rxFreq = JP_MINIMUM_FM_MHZ;
        }
        double f = rxFreq / 10.0;
        Serial.print("rx freq: ");
        Serial.print(f);
        Serial.print("MHz");
    }
}

EncCountStatus _readEncCountStatus(EncSide encSide, volatile byte* pos, volatile int16_t* cnt) {
    byte current;
    switch(encSide) {
        case LEFT:
            current = (!digitalRead(LEFT_ENC_PIN_B) << 1) + !digitalRead(LEFT_ENC_PIN_A);
            break;
        case RIGHT:
            current = (!digitalRead(RIGHT_ENC_PIN_B) << 1) + !digitalRead(RIGHT_ENC_PIN_A);
            break;
        default:
            // absolutely unreachable
            Serial.print("[ERROR] unexpected enc side has come: ");
            Serial.println(encSide);
            return {0, 0};
    }

    byte previous = *pos & 0b11;
    byte direction = (*pos & 0b00110000) >> 4;

    if (current == 3) current = 2;
    else if (current == 2) current = 3;

    int16_t previousCnt = *cnt;

    if (current != previous) {
        if (direction == 0) {
            if (current == 0b01 || current == 0b11) {
                direction = current;
            }
        } else {
            if (current == 0b00) {
                if (direction == 0b01 && previous == 0b11) {
                    (*cnt)++;
                }
                else if (direction == 0b11 && previous == 0b01) {
                    (*cnt)--;
                }
                direction = 0b00;
            }
        }
        *pos = (direction << 4) + (previous << 2) + current;
    }

    EncCountStatus encStatus = {previousCnt, *cnt};
    return encStatus;
}
