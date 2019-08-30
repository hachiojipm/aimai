#ifndef _AWIA_VIEW_INC_GUARD_H_
#define _AWIA_VIEW_INC_GUARD_H_

#include <Arduino.h>
#include "constants.h"
#include <Adafruit_SSD1306.h>
#include <Wire.h>

class View {
public:
    View();

    void begin();

    void displayLogo();

    // Rx
    void displayVol(int vol);
    void displayRxFreq(int rxFreq);
    void displayRxRDSTextAsMarquee(const char *rdsBuff);

    // Tx
    void displayTxFreq(int txFreq);
    void displayTxRDSTextForInput(const char *rdsBuff);
    void displayTxRDSTextAsMarquee(const char *rdsBuff);

private:
    uint8_t rxRDSTextOffset;
    uint8_t txRDSTextOffset;
    Adafruit_SSD1306 display;
    void incrementRxRDSTextOffset();
    void incrementTxRDSTextOffset();
    void displayRDSTextAsMarquee(int offset, const char *rdsBuff);
};

#endif //_AWIA_VIEW_INC_GUARD_H_
