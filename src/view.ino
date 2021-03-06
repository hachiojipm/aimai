#include "view.h"

View::View() {
    display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
    rxRDSTextOffset = 0;
    txRDSTextOffset = 0;
}

void View::begin() {
    while (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR));
}

void View::displayLogo() {
    const static unsigned char PROGMEM logoBMP[] = {
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

    display.clearDisplay();
    display.drawBitmap(0, 0, logoBMP, LOGO_WIDTH, LOGO_HEIGHT, WHITE);
    display.display();
    delay(1000);
}

void View::displayVol(int vol) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(25, 8);
    display.printf("Vol %2d", vol);
    display.display();
}

void View::displayRxFreq(int rxFreq) {
    double f = rxFreq / 10.0;
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 8);
    display.printf("Rx %.1fMHz", f);
    display.display();
}

void View::displayRxRDSTextAsMarquee(const char *rdsBuff) {
    displayRDSTextAsMarquee(rxRDSTextOffset, rdsBuff);
    incrementRxRDSTextOffset();
}

void View::incrementRxRDSTextOffset() {
    if (++rxRDSTextOffset >= RDS_TEXT_LENGTH) {
        rxRDSTextOffset = 0;
    }
}

void View::displayTxFreq(int txFreq) {
    double f = txFreq / 10.0;
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 8);
    display.printf("Tx %.1fMHz", f);
    display.display();
}

void View::displayTxRDSTextForInput(const char *rdsBuff) {
    char *rdsShowBuff = (char *) malloc(sizeof(char) * OLED_TEXT_LEN_BY_ONE_LINE);
    if (strlen(rdsBuff) >= OLED_TEXT_LEN_BY_ONE_LINE) {
        strncpy(rdsShowBuff, &rdsBuff[strlen(rdsBuff) - (OLED_TEXT_LEN_BY_ONE_LINE - 1)],
                OLED_TEXT_LEN_BY_ONE_LINE - 1);
    } else {
        strcpy(rdsShowBuff, rdsBuff);
    }

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 8);
    display.setTextWrap(false);
    display.printf(rdsShowBuff);
    display.display();

    free(rdsShowBuff);
}

void View::displayTxRDSTextAsMarquee(const char *rdsBuff) {
    displayRDSTextAsMarquee(txRDSTextOffset, rdsBuff);
    incrementTxRDSTextOffset();
}

void View::incrementTxRDSTextOffset() {
    if (++txRDSTextOffset >= RDS_TEXT_LENGTH) {
        txRDSTextOffset = 0;
    }
}

void View::displayRDSTextAsMarquee(int offset, const char *rdsBuff) {
    display.clearDisplay();

    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 8);
    display.setTextWrap(false);

    int counterOffset = 0;
    char rdsShowBuff[OLED_TEXT_LEN_BY_ONE_LINE];
    for (int i = 0; i < OLED_TEXT_LEN_BY_ONE_LINE; i++) {
        int cursor = i + offset - counterOffset;
        if (cursor >= RDS_TEXT_LENGTH) {
            cursor -= RDS_TEXT_LENGTH;
        }
        rdsShowBuff[i] = rdsBuff[cursor];
    }

    display.print(rdsShowBuff);
    display.display();

    delay(150);
}
