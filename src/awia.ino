#include "awia.h"
#include <SparkFunSi4703.h>
#include <Adafruit_Si4713.h>
#include <esp_attr.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <esp32-hal-log.h>
#include <freertos/task.h>
#include <esp32-hal-timer.h>

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
Si4703_Breakout rx(RX_RST_PIN, SDA_PIN, SCL_PIN, UNUSED);
Adafruit_Si4713 tx(TX_RST_PIN);

volatile int16_t rxVol = 0; // TODO load init value from the nonvolatile memory
volatile int16_t rxFreq = JP_MINIMUM_FM_MHZ; // TODO load init value from the nonvolatile memory
volatile int16_t txFreq = JP_MINIMUM_FM_MHZ; // TODO load init value from the nonvolatile memory

static const unsigned char PROGMEM logo_bmp[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0xFF, 0xFF, 0x80, 0x0F, 0xC0, 0x3F, 0xFF, 0xFF, 0xE0, 0x01, 0xFF, 0xFF, 0x00, 0x0F, 0xC0,
        0x03, 0xFF, 0xFF, 0xE0, 0x0F, 0x80, 0x7F, 0xFF, 0xFF, 0xE0, 0x07, 0xFF, 0xFF, 0xC0, 0x07, 0xC0,
        0x07, 0xFD, 0x5F, 0xF0, 0x0F, 0xC0, 0xFE, 0x1F, 0x87, 0xF0, 0x07, 0xFD, 0x5F, 0xF0, 0x0F, 0xC0,
        0x0F, 0xE0, 0x01, 0xF0, 0x0F, 0x80, 0xFC, 0x1F, 0x81, 0xF0, 0x0F, 0xE0, 0x03, 0xF0, 0x07, 0xC0,
        0x0F, 0xC0, 0x01, 0xF8, 0x0F, 0xC0, 0xF8, 0x1F, 0x81, 0xF0, 0x0F, 0xC0, 0x01, 0xF8, 0x0F, 0xC0,
        0x05, 0x40, 0x01, 0xF8, 0x0F, 0x80, 0xF8, 0x1F, 0x81, 0xF0, 0x0A, 0x80, 0x03, 0xF8, 0x07, 0xC0,
        0x00, 0x00, 0x17, 0xF8, 0x0F, 0xC0, 0xF8, 0x1F, 0x81, 0xF0, 0x00, 0x00, 0x2F, 0xF8, 0x0F, 0xC0,
        0x00, 0x7F, 0xFF, 0xF8, 0x0F, 0x80, 0xF8, 0x1F, 0x81, 0xF0, 0x00, 0xFF, 0xFF, 0xF8, 0x07, 0xC0,
        0x03, 0xFF, 0xFF, 0xF8, 0x0F, 0xC0, 0xF8, 0x1F, 0x81, 0xF0, 0x07, 0xFF, 0xFD, 0xF8, 0x0F, 0xC0,
        0x0F, 0xFE, 0x81, 0xF8, 0x0F, 0x80, 0xF8, 0x1F, 0x81, 0xF0, 0x0F, 0xFE, 0x81, 0xF8, 0x07, 0xC0,
        0x1F, 0xC0, 0x01, 0xF8, 0x0F, 0xC0, 0xF8, 0x1F, 0x81, 0xF0, 0x1F, 0xC0, 0x01, 0xF8, 0x0F, 0xC0,
        0x1F, 0x80, 0x01, 0xF8, 0x0F, 0x80, 0xF8, 0x1F, 0x81, 0xF0, 0x1F, 0x80, 0x03, 0xF8, 0x07, 0xC0,
        0x1F, 0x80, 0x03, 0xF8, 0x0F, 0xC0, 0xF8, 0x1F, 0x81, 0xF0, 0x3F, 0x80, 0x07, 0xF8, 0x0F, 0xC0,
        0x1F, 0xC0, 0x07, 0xF8, 0x0F, 0x80, 0xF8, 0x1F, 0x81, 0xF0, 0x1F, 0x80, 0x0F, 0xF8, 0x07, 0xC0,
        0x0F, 0xE0, 0x3F, 0xFC, 0x0F, 0xC1, 0xF8, 0x1F, 0x81, 0xF8, 0x1F, 0xC0, 0x3F, 0xFC, 0x0F, 0xC0,
        0x0F, 0xFF, 0xFF, 0xFF, 0x0F, 0x87, 0xF8, 0x1F, 0x81, 0xFE, 0x0F, 0xFF, 0xFF, 0xFE, 0x07, 0xC0,
        0x03, 0xFF, 0xFC, 0xFE, 0x0F, 0xC3, 0xF8, 0x1F, 0x81, 0xFE, 0x03, 0xFF, 0xF8, 0xFE, 0x0F, 0xC0,
        0x00, 0x5F, 0x40, 0x3E, 0x0F, 0x82, 0xE0, 0x1F, 0x80, 0x74, 0x00, 0x7E, 0x80, 0x7E, 0x07, 0xC0,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

volatile bool isRXRDSReading = false;

void displayLogo() {
    display.clearDisplay();
    display.drawBitmap(0, 0, logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, WHITE);
    display.display();
    delay(1000);
}

xTaskHandle xReadRDSTaskHandler;
char rdsBuff[RDS_TEXT_LENGTH]; // thi param is used by both of the state (i.e. rx and tx)

void displayRXFreq(int rxFreq) {
    double f = rxFreq / 10.0;
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 8);
    display.printf("RX %.1fMHz", f);
    display.display();
}

void displayVol(int vol) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(25, 8);
    display.printf("Vol %2d", vol);
    display.display();
}

void setup() {
    Serial.begin(115200);
    while (!Serial);
    Serial.println("setup...");

    while (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR));
    displayLogo();

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

// TODO
char txRDSText[RDS_TEXT_LENGTH] = {'(', 'e', 'm', 'p', 't', 'y', ')', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                                   ' ', ' ', ' '};
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

    // TODO
    tx.beginRDS();
    tx.setRDSstation(STATION_NAME);
    tx.setRDSbuffer(txRDSText);
}

volatile bool txRDSTextChanged = false;

volatile bool doDisplayRXRDS = false;

void tickRXRDSDisplay() {
    doDisplayRXRDS = true;
}

void rxLoop(void *arg) {
    // TODO read from nonvolatile memory
    rxFreq = 888;
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

    int rxRDSTextOffset = 0;

    while (true) {
        bool isDisplayedRXCtrl = false;
        if (mainLoopRxFreq != rxFreq) {
            rx.setChannel(rxFreq);
            mainLoopRxFreq = rxFreq;
            displayRXFreq(rxFreq);
            isDisplayedRXCtrl = true;
        }

        if (mainLoopRxVol != rxVol) {
            rx.setVolume(rxVol);
            mainLoopRxVol = rxVol;
            displayVol(rxVol);
            isDisplayedRXCtrl = true;
        }

        if (isDisplayedRXCtrl) {
            doDisplayRXRDS = false;
            timerEnd(rxRDSDisplayTicker);
            rxRDSDisplayTicker = timerBegin(0, 80, true);
            timerAttachInterrupt(rxRDSDisplayTicker, &tickRXRDSDisplay, true);
            timerAlarmWrite(rxRDSDisplayTicker, 300000, true);
            timerAlarmEnable(rxRDSDisplayTicker);
        }

        if (doDisplayRXRDS && !isRXRDSReading) {
            display.clearDisplay();

            display.setTextSize(2);
            display.setTextColor(WHITE);
            display.setCursor(0, 8);
            display.setTextWrap(false);

            int counterOffset = 0;
            char rdsShowBuff[OLED_TEXT_LEN_BY_ONE_LINE];
            for (int i = 0; i < OLED_TEXT_LEN_BY_ONE_LINE; i++) {
                int cursor = i + rxRDSTextOffset - counterOffset;
                if (cursor >= RDS_TEXT_LENGTH) {
                    cursor -= RDS_TEXT_LENGTH;
                }
                rdsShowBuff[i] = rdsBuff[cursor];
            }

            display.print(rdsShowBuff);
            display.display();

            delay(150);

            rxRDSTextOffset++;
            if (rxRDSTextOffset >= RDS_TEXT_LENGTH) {
                rxRDSTextOffset = 0;
            }
        }
    }
}

void txLoop(void *arg) {
    // TODO read from nonvolatile memory
    txFreq = JP_MINIMUM_FM_MHZ;
    int mainLoopTxFreq = txFreq;
    tx.tuneFM(txFreq * 10);

    while (true) {
        if (mainLoopTxFreq != txFreq) {
            tx.tuneFM(txFreq * 10);
            mainLoopTxFreq = txFreq;
        }

        if (txRDSTextChanged) {
            tx.setRDSbuffer(txRDSText);
            txRDSTextChanged = false;
        }
    }
}

void nopLoop(void *arg) {
    while (true) {
    }
}

char defaultRXRDSText[RDS_TEXT_LENGTH] = {
        '<', 'N', 'O', ' ', 'R', 'D', 'S', ' ', 'D', 'A',
        'T', 'A', '>', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
};

void readRDSPeriodically(void *arg) {
    delay(1000); // if this delay doesn't exist, the RX procedure be stuck
    while (true) {
        isRXRDSReading = true;
        rx.readRDS(rdsBuff, RDS_READING_TIMEOUT_MILLIS);
        if (rdsBuff[0] == '\0') {
            for (int i = 0; i < RDS_TEXT_LENGTH; i++) {
                rdsBuff[i] = defaultRXRDSText[i];
            }
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
        Serial.print("rx vol: ");
        Serial.println(rxVol);
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
        double f = rxFreq / 10.0;
        Serial.print("rx freq: ");
        Serial.print(f);
        Serial.println("MHz");
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
        double f = txFreq / 10.0;
        Serial.print("tx freq: ");
        Serial.print(f);
        Serial.println("MHz");
    }
}

#define RUNES_NUM 44
char runes[RUNES_NUM] = {
        ' ',
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
        'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', ',', '-',
        '_', '@', '(', ')'
};
volatile byte posForTextInput;
volatile int16_t textInputCursor = 0;
volatile bool rdsEditing = false;

void editRDSText() {
    if (!rdsEditing) {
        rdsEditing = true;
        txRDSBuffCursor = 0;
    }

    EncCountStatus encStatus = _readEncCountStatus(RIGHT, &posForTextInput, &textInputCursor);
    if (encStatus.currentCnt != encStatus.previousCnt) {
        if (encStatus.currentCnt < 0) { // alignment
            textInputCursor = RUNES_NUM - 1;
        } else if (encStatus.currentCnt > (RUNES_NUM - 1)) {
            textInputCursor = 0;
        }
        Serial.println(runes[textInputCursor]);
    }
}

void enterRDSTextCharacter() {
    if (!rdsEditing) {
        return;
    }
    Serial.print("char: ");
    Serial.println(runes[textInputCursor]);
    rdsBuff[txRDSBuffCursor++] = runes[textInputCursor];
    if (txRDSBuffCursor >= RDS_TEXT_LENGTH) {
        escapeRDSTextEditing();
        memcpy(txRDSText, rdsBuff, RDS_TEXT_LENGTH);
        Serial.print("RDS Text: ");
        Serial.println(txRDSText);
        txRDSTextChanged = true;
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
        Serial.println("TX mode");
        inactivateRx();
        initTx();
        xTaskCreatePinnedToCore(txLoop, TASK_MAIN, 4096, NULL, MAIN_TASK_PRIORITY, NULL, MAIN_TASK_CPU_NO);
        return;
    }

    if (tx == HIGH && rx == LOW) {
        Serial.println("RX mode");
        inactivateTx();
        initRx();
        xTaskCreatePinnedToCore(rxLoop, TASK_MAIN, 4096, NULL, MAIN_TASK_PRIORITY, NULL, MAIN_TASK_CPU_NO);
        return;
    }

    // both of the mode pins are 0 or 1, this handles it as NOP mode
    Serial.println("NOP mode");
    inactivateTx();
    inactivateRx();
    xTaskCreatePinnedToCore(nopLoop, TASK_MAIN, 4096, NULL, MAIN_TASK_PRIORITY, NULL, MAIN_TASK_CPU_NO);
}

void loop() {
}

