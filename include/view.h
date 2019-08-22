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

    // RX
    void displayVol(int vol);
    void displayRXFreq(int rxFreq);
    void displayRXRDSTextAsMarquee(const char rdsBuff[RDS_TEXT_LENGTH]);

private:
    uint8_t rxRDSTextOffset;
    Adafruit_SSD1306 display;
    void incrementRXRDSTextOffset();
};

#endif //_AWIA_VIEW_INC_GUARD_H_
