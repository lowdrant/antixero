#ifndef _HELPERS_H
#define _HELPERS_H
#include <SD.h>
#include "hardware.h"
#include "safeLogger/safeLogger.h"

void printDirectory(File dir, int numTabs) {
    while (true) {

        File entry = dir.openNextFile();
        if (!entry) {
            // no more files
            break;
        }
        for (uint8_t i = 0; i < numTabs; i++) {
            Serial.print('\t');
        }
        Serial.print(entry.name());
        if (entry.isDirectory()) {
            Serial.println("/");
            printDirectory(entry, numTabs + 1);
        } else {
            // files have sizes, directories do not
            Serial.print("\t\t");
            Serial.println(entry.size(), DEC);
        }
        entry.close();
    }
}

bool sdPresent() {
    int state = digitalRead(SD_CHIPDETECT);
    return state == HIGH;
}

void clearLCD(LiquidCrystal_I2C * lcd, int row) {
    lcd->setCursor(0, row);
    for (int i = 0; i < LCD_COLS; i++) {
        lcd->print(" ");
    }
    lcd->setCursor(0, row);
}

/*
 * msg must be LCD_COLS+1 long
 */
char *padLCDMsg(char *const msg) {
    size_t len = strlen(msg);
    /* right-pad str with spaces to clear trailing chars on display */
    for (int i = len; i < LCD_COLS; i++) {
        msg[i] = ' ';
    }
    msg[LCD_COLS + 1] = '\0';
    return msg;
}

/* write error to serial and 2nd row of LCD */
void errMsg(LiquidCrystal_I2C * lcd, char *msg) {
    static char str[LCD_COLS + 1];
    strncpy(str, msg, LCD_COLS);
    /* right-pad str with spaces to clear trailing chars on display */
    for (int i = strlen(msg); i < LCD_COLS; i++) {
        str[i] = ' ';
    }
    str[LCD_COLS] = '\0';
    lcd->setCursor(0, 1);
    lcd->print(str);
    Serial.println(msg);
}

void errMsg(LiquidCrystal_I2C * lcd, const __FlashStringHelper * msg) {
    static char buf[LCD_COLS + 1];
    strncpy_P(buf, (const char *)msg, LCD_COLS);
    errMsg(lcd, buf);
}

String createDatalog() {
    File root = SD.open("/");
    if (!root) {
        return "rooterr";
    }
    String fn = getLogName(&root, SD_FN_BASE, SD_FN_SUF);
    root.close();
    Serial.print(F("fn="));
    Serial.println(fn);
    File logFile = SD.open(fn, FILE_WRITE);
    if (logFile) {
        logFile.println("time (ms),humidity (pct),rain,");
        logFile.close();
    }
    return fn;
}

#endif /* ifndef _HELPERS_H */
