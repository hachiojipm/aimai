#include "awia.h"
#include "view.h"
#include <SparkFunSi4703.h>
#include <Adafruit_Si4713.h>
#include <esp_attr.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <esp32-hal-log.h>
#include <freertos/task.h>
#include <esp32-hal-timer.h>
#include <EEPROM.h>

View view;
Si4703_Breakout rx(RX_RST_PIN, SDA_PIN, SCL_PIN, UNUSED);
Adafruit_Si4713 tx(TX_RST_PIN);

volatile int16_t rxVol = 0; // TODO load init value from the nonvolatile memory
volatile int16_t rxFreq = JP_MINIMUM_FM_MHZ; // TODO load init value from the nonvolatile memory
volatile int16_t txFreq = JP_MINIMUM_FM_MHZ; // TODO load init value from the nonvolatile memory

volatile bool isRXRDSReading = false;

xTaskHandle xReadRDSTaskHandler;
char rdsBuff[RDS_TEXT_LENGTH + 1]; // thi param is used by both of the state (i.e. rx and tx)

void setup() {
    Serial.begin(115200);
    while (!Serial);
    log_i("setup...");

    view.begin();
    view.displayLogo();

    EEPROM.begin(EEPROM_LENGTH);

    pinMode(RIGHT_ENC_PIN_A, INPUT_PULLUP);
    pinMode(RIGHT_ENC_PIN_B, INPUT_PULLUP);
    pinMode(RIGHT_ENC_SW_PIN, INPUT_PULLUP);
    pinMode(LEFT_ENC_PIN_A, INPUT_PULLUP);
    pinMode(LEFT_ENC_PIN_B, INPUT_PULLUP);
    pinMode(LEFT_ENC_SW_PIN, INPUT_PULLUP);

    pinMode(RX_MODE_PIN, INPUT_PULLUP);
    detachInterrupt(RX_MODE_PIN);
    attachInterrupt(RX_MODE_PIN, loadActionMode, CHANGE);

    pinMode(TX_MODE_PIN, INPUT_PULLUP);
    detachInterrupt(TX_MODE_PIN);
    attachInterrupt(TX_MODE_PIN, loadActionMode, CHANGE);

    xTaskCreatePinnedToCore(readRDSPeriodically, TASK_READ_RDS_PERIODICALLY, 4096, NULL, READ_RDS_TASK_PRIORITY,
                            &xReadRDSTaskHandler, READ_RDS_TASK_CPU_NO);
    loadActionMode();
}

void initRx() {
    detachInterrupt(RIGHT_ENC_PIN_A);
    attachInterrupt(RIGHT_ENC_PIN_A, changeRxVolume, CHANGE);
    detachInterrupt(RIGHT_ENC_PIN_B);
    attachInterrupt(RIGHT_ENC_PIN_B, changeRxVolume, CHANGE);

    detachInterrupt(LEFT_ENC_PIN_A);
    attachInterrupt(LEFT_ENC_PIN_A, changeRxFreq, CHANGE);
    detachInterrupt(LEFT_ENC_PIN_B);
    attachInterrupt(LEFT_ENC_PIN_B, changeRxFreq, CHANGE);

    rx.powerOn();
    rx.setChannel(0);
    rx.setVolume(0);

    vTaskResume(xReadRDSTaskHandler);
}

char txRDSText[RDS_TEXT_LENGTH + 1] = {
        '(', 'e', 'm', 'p', 't', 'y', ')', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\0'
};
volatile int txRDSBuffCursor = 0;

void initTx() {
    detachInterrupt(RIGHT_ENC_PIN_A);
    attachInterrupt(RIGHT_ENC_PIN_A, editRDSText, CHANGE);
    detachInterrupt(RIGHT_ENC_PIN_B);
    attachInterrupt(RIGHT_ENC_PIN_B, editRDSText, CHANGE);

    detachInterrupt(RIGHT_ENC_SW_PIN);
    attachInterrupt(RIGHT_ENC_SW_PIN, enterRDSTextCharacter, FALLING);

    detachInterrupt(LEFT_ENC_PIN_A);
    attachInterrupt(LEFT_ENC_PIN_A, changeTxFreq, CHANGE);
    detachInterrupt(LEFT_ENC_PIN_B);
    attachInterrupt(LEFT_ENC_PIN_B, changeTxFreq, CHANGE);
    detachInterrupt(LEFT_ENC_SW_PIN);
    attachInterrupt(LEFT_ENC_SW_PIN, escapeRDSTextEditing, FALLING);

    tx.begin();
    tx.setTXpower(TX_POWER);

    tx.beginRDS();
    tx.setRDSstation(STATION_NAME);
    uint8_t isTxRDSTextWrote;
    EEPROM.get(EEPROM_IS_TX_RDS_WROTE_ADDR, isTxRDSTextWrote);
    if (isTxRDSTextWrote == EEPROM_IS_TX_RDS_WROTE_MARKER) {
        char tmpTxRDSText[RDS_TEXT_LENGTH + 1];
        for (int i = 0; i < RDS_TEXT_LENGTH + 1; i++) {
            char c;
            EEPROM.get(i + EEPROM_TX_RDS_TEXT_ADDR_BEGIN, c);
            tmpTxRDSText[i] = c;
        }
        strcpy(txRDSText, tmpTxRDSText);
    }
    tx.setRDSbuffer(txRDSText);
}

volatile bool txRDSTextChanged = false;

volatile bool doDisplayRXRDS = false;

void tickRXRDSDisplay() {
    doDisplayRXRDS = true;
}

void rxLoop(void *arg) {
    // TODO read from nonvolatile memory
    rxFreq = 761;
    int mainLoopRxFreq = rxFreq;
    rx.setChannel(mainLoopRxFreq);
    rxVol = 3;
    int mainLoopRxVol = rxVol;
    rx.setVolume(mainLoopRxVol);

    hw_timer_t *rxRDSDisplayTicker = nullptr;
    rxRDSDisplayTicker = timerBegin(0, 80, true);
    timerAttachInterrupt(rxRDSDisplayTicker, &tickRXRDSDisplay, true);
    timerAlarmWrite(rxRDSDisplayTicker, 1000000, true);
    timerAlarmEnable(rxRDSDisplayTicker);

    while (true) {
        bool isDisplayedRXCtrl = false;
        if (mainLoopRxFreq != rxFreq) {
            rx.setChannel(rxFreq);
            mainLoopRxFreq = rxFreq;
            view.displayRxFreq(rxFreq);
            isDisplayedRXCtrl = true;
        }

        if (mainLoopRxVol != rxVol) {
            rx.setVolume(rxVol);
            mainLoopRxVol = rxVol;
            view.displayVol(rxVol);
            isDisplayedRXCtrl = true;
        }

        if (isDisplayedRXCtrl) {
            doDisplayRXRDS = false;
            timerEnd(rxRDSDisplayTicker);
            rxRDSDisplayTicker = timerBegin(0, 80, true);
            timerAttachInterrupt(rxRDSDisplayTicker, &tickRXRDSDisplay, true);
            timerAlarmWrite(rxRDSDisplayTicker, 300000, true);
            timerAlarmEnable(rxRDSDisplayTicker);
            delay(50);
        }

        if (doDisplayRXRDS && !isRXRDSReading) {
            view.displayRxRDSTextAsMarquee(rdsBuff);
        }
    }
}

volatile bool rdsTextEntering = false;
volatile bool rdsEditing = false;

#define RUNES_NUM 44
char runes[RUNES_NUM] = {
        ' ',
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
        'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', ',', '-',
        '_', '@', '(', ')'
};
volatile int16_t textInputCursor = 0;

volatile bool doDisplayTxRDS = false;

void tickTxRDSDisplay() {
    doDisplayTxRDS = true;
}

void txLoop(void *arg) {
    // TODO read from nonvolatile memory
    txFreq = JP_MINIMUM_FM_MHZ;
    int mainLoopTxFreq = txFreq;
    tx.tuneFM(txFreq * 10);

    hw_timer_t *txRDSDisplayTicker = nullptr;
    txRDSDisplayTicker = timerBegin(0, 80, true);
    timerAttachInterrupt(txRDSDisplayTicker, &tickTxRDSDisplay, true);
    timerAlarmWrite(txRDSDisplayTicker, 1000000, true);
    timerAlarmEnable(txRDSDisplayTicker);

    while (true) {
        if (mainLoopTxFreq != txFreq) {
            tx.tuneFM(txFreq * 10);
            mainLoopTxFreq = txFreq;
            view.displayTxFreq(txFreq);

            doDisplayTxRDS = false;
            timerEnd(txRDSDisplayTicker);
            txRDSDisplayTicker = timerBegin(0, 80, true);
            timerAttachInterrupt(txRDSDisplayTicker, &tickTxRDSDisplay, true);
            timerAlarmWrite(txRDSDisplayTicker, 300000, true);
            timerAlarmEnable(txRDSDisplayTicker);
            delay(50);
        }

        if (txRDSTextChanged) {
            tx.setRDSbuffer(txRDSText);
            EEPROM.put(EEPROM_IS_TX_RDS_WROTE_ADDR, EEPROM_IS_TX_RDS_WROTE_MARKER);
            for (int i = 0; i < RDS_TEXT_LENGTH + 1; i++) {
                EEPROM.put(EEPROM_TX_RDS_TEXT_ADDR_BEGIN + i, txRDSText[i]);
            }
            EEPROM.commit();
            txRDSTextChanged = false;
        }

        // workaround for avoiding chattering
        if (rdsTextEntering) {
            delay(200);
            rdsTextEntering = false;
        }

        if (rdsEditing) {
            rdsBuff[txRDSBuffCursor] = runes[textInputCursor];
            view.displayTxRDSTextForInput(rdsBuff);
            delay(100);
        } else if (doDisplayTxRDS) {
            view.displayTxRDSTextAsMarquee(txRDSText);
        }
    }
}

void nopLoop(void *arg) {
    while (true) {
    }
}

const static unsigned char defaultRXRDSText[RDS_TEXT_LENGTH + 1] = {
        '<', 'n', 'o', ' ', 'r', 'd', 's', ' ', 'd', 'a',
        't', 'a', '>', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
        '\0'
};

void readRDSPeriodically(void *arg) {
    delay(1000); // if this delay doesn't exist, the RX procedure be stuck
    while (true) {
        log_d("read RDS");
        isRXRDSReading = true;
        rx.readRDS(rdsBuff, RDS_READING_TIMEOUT_MILLIS);
        log_d("RDS buff: %s", rdsBuff);
        if (rdsBuff[0] == '\0') {
            memcpy(rdsBuff, defaultRXRDSText, RDS_TEXT_LENGTH + 1);
        }
        isRXRDSReading = false;
        delay(RDS_READING_PERIOD_MILLIS);
    }
}


volatile byte posForRxVol;

void changeRxVolume() {
    EncCountStatus encStatus = _readEncCountStatus(RIGHT, &posForRxVol, &rxVol);
    if (encStatus.currentCnt != encStatus.previousCnt) {
        if (encStatus.currentCnt <= 0) {
            rxVol = 0;
        } else if (encStatus.currentCnt > 15) {
            rxVol = 15;
        }
        log_d("Rx vol: %d", rxVol);
    }
}

volatile byte posForRxFreq;

void changeRxFreq() {
    EncCountStatus encStatus = _readEncCountStatus(LEFT, &posForRxFreq, &rxFreq);
    if (encStatus.currentCnt != encStatus.previousCnt) {
        if (encStatus.currentCnt < JP_MINIMUM_FM_MHZ) {
            rxFreq = JP_MAXIMUM_FM_MHZ;
        } else if (encStatus.currentCnt > JP_MAXIMUM_FM_MHZ) {
            rxFreq = JP_MINIMUM_FM_MHZ;
        }
        log_d("Rx freq: %fMHz", rxFreq / 10.0);
    }
}

volatile byte posForTxFreq;

void changeTxFreq() {
    EncCountStatus encStatus = _readEncCountStatus(LEFT, &posForTxFreq, &txFreq);
    if (encStatus.currentCnt != encStatus.previousCnt) {
        if (encStatus.currentCnt < JP_MINIMUM_FM_MHZ) {
            txFreq = JP_MAXIMUM_FM_MHZ;
        } else if (encStatus.currentCnt > JP_MAXIMUM_FM_MHZ) {
            txFreq = JP_MINIMUM_FM_MHZ;
        }
        log_d("Tx freq: %fMHz", txFreq / 10.0);
    }
}

volatile byte posForTextInput;

void editRDSText() {
    if (!rdsEditing) {
        rdsEditing = true;
        txRDSBuffCursor = 0;
        memset(rdsBuff, '\0', RDS_TEXT_LENGTH + 1);
    }

    EncCountStatus encStatus = _readEncCountStatus(RIGHT, &posForTextInput, &textInputCursor);
    if (encStatus.currentCnt != encStatus.previousCnt) {
        if (encStatus.currentCnt < 0) { // alignment
            textInputCursor = RUNES_NUM - 1;
        } else if (encStatus.currentCnt > (RUNES_NUM - 1)) {
            textInputCursor = 0;
        }
    }
}

void enterRDSTextCharacter() {
    if (rdsTextEntering || !rdsEditing) {
        return;
    }
    rdsTextEntering = true;
    log_d("char: %c", runes[textInputCursor]);
    rdsBuff[txRDSBuffCursor] = runes[textInputCursor];
    txRDSBuffCursor++;
    if (txRDSBuffCursor >= RDS_TEXT_LENGTH) {
        memcpy(txRDSText, rdsBuff, RDS_TEXT_LENGTH);
        log_d("RDS text: %s", txRDSText);
        txRDSTextChanged = true;
        escapeRDSTextEditing();
    }
}

void escapeRDSTextEditing() {
    rdsEditing = false;
}

EncCountStatus _readEncCountStatus(EncSide encSide, volatile byte *pos, volatile int16_t *cnt) {
    byte current;
    switch (encSide) {
        case LEFT:
            current = (!digitalRead(LEFT_ENC_PIN_B) << 1) + !digitalRead(LEFT_ENC_PIN_A);
            break;
        case RIGHT:
            current = (!digitalRead(RIGHT_ENC_PIN_B) << 1) + !digitalRead(RIGHT_ENC_PIN_A);
            break;
        default:
            // absolutely unreachable
            log_e("[ERROR] unexpected enc side has come: %s", encSide);
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
                } else if (direction == 0b11 && previous == 0b01) {
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

void inactivateRx() {
    pinMode(RX_RST_PIN, OUTPUT);
    digitalWrite(RX_RST_PIN, HIGH);
    delay(10);
    digitalWrite(RX_RST_PIN, LOW);
    delay(10);
    digitalWrite(RX_RST_PIN, HIGH);

    vTaskSuspend(xReadRDSTaskHandler);
}

void inactivateTx() {
    tx.reset();
}

void loadActionMode() {
    int rx = digitalRead(RX_MODE_PIN);
    int tx = digitalRead(TX_MODE_PIN);
    if (rx == HIGH && tx == LOW) {
        log_i("Tx mode");
        inactivateRx();
        initTx();
        xTaskCreatePinnedToCore(txLoop, TASK_MAIN, 4096, NULL, MAIN_TASK_PRIORITY, NULL, MAIN_TASK_CPU_NO);
        return;
    }

    if (tx == HIGH && rx == LOW) {
        log_i("Rx mode");
        inactivateTx();
        initRx();
        xTaskCreatePinnedToCore(rxLoop, TASK_MAIN, 4096, NULL, MAIN_TASK_PRIORITY, NULL, MAIN_TASK_CPU_NO);
        return;
    }

    // both of the mode pins are 0 or 1, this handles it as NOP mode
    log_i("NOP mode");
    inactivateTx();
    inactivateRx();
    xTaskCreatePinnedToCore(nopLoop, TASK_MAIN, 4096, NULL, MAIN_TASK_PRIORITY, NULL, MAIN_TASK_CPU_NO);
}

void loop() {
}

