#include "hardware.h"
#include "DHT.h"
#include <SPI.h>
#include <SD.h>
#include <LiquidCrystal_I2C.h>
#include <string.h>
#include "helpers.h"

String fn = String(SD_FN_BASE); // +String(SD_FN_SUF);
float hum, temp_c, temp_f, hi_c, hi_f;
uint16_t rain = 0;
int fnnum = 0;
DHT dht(DHT_PIN, DHT_TYPE);
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_ROWS, LCD_COLS);

char lcd_str[LCD_ROWS][LCD_COLS];

/* 10-digit (32-bit) timestamp + percent + 4-digit analog read 
 * + safety buffer
 */
const size_t logstr_len = 10 + 3 + 4 + 10;
char logstr[logstr_len];

File root, logFile;

bool sdUnplugged = true; /* track if sd was unplugged */

unsigned long timestamp;

void setup() {
    /* init communciations */
    Serial.begin(BAUD);
    lcd.init();
    lcd.backlight();
    lcd.clear();

    dht.begin();

    /* SD Card Detection */
    pinMode(SD_CHIPDETECT, INPUT_PULLUP);
    while (!sdPresent()) {
        errMsg(&lcd, "No SD card");
        delay(200);
    }

    /* SD Card Reading */
    while (!SD.begin(SD_CS)) {
        errMsg(&lcd, "SD card failure");
        delay(200);
    }
    lcd.clear();
    lcd.print("card initialized.");

    /* Determine Logfile */
    root = SD.open("/");
    printDirectory(root, 0);
    root.rewindDirectory();
    fnnum = getDatalogNum(root);
    root.close();

    /* Create Logfile */
    fn += String(fnnum) + String(SD_FN_SUF);
    Serial.println(fn);
    logFile = SD.open(fn, FILE_WRITE);
    if (logFile) {
        logFile.println("time (ms),humidity (pct),rain,");
        logFile.close();
    }
}

void loop() {
    timestamp = millis();
    /* Read Humidity */
    hum = dht.readHumidity();
    temp_f = dht.readTemperature(true);
    hi_f = dht.computeHeatIndex(temp_f, hum);

    /* Rain Sensor */
    rain = analogRead(RAIN_PIN);

    /* LCD */
    snprintf(lcd_str[0], LCD_COLS, "Hum:%d%% Rn:%d", (int)hum, rain);
//    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(lcd_str[0]);
    lcd.setCursor(0, 1);
    if (!sdPresent()) {
        lcd.print("No SD card");
    } else {
        lcd.print(fn);
    }

    /* Logging */
    if (!sdPresent()) {
        errMsg(&lcd, "No SD card");
        sdUnplugged = true;
    } else {
        /* if card was unplugged, re-open & increment logfile */
        if (sdUnplugged) {
            if (!SD.begin(SD_CS)) {
                errMsg(&lcd, "SD card error");
            } else {
                sdUnplugged = false;
                fnnum += 1;
                fn = String(SD_FN_BASE) + String(fnnum) + String(SD_FN_SUF);
            }
        }
        /* construct log before opening for time */
        snprintf(logstr, logstr_len, "%lu,%d,%d,\n", timestamp, hum, rain);
        /* write logfile */
        logFile = SD.open(fn, FILE_WRITE);
        if (!logFile) {
            errMsg(&lcd, "logfile error");
        } else {
            logFile.print(logstr);
            logFile.close();
            Serial.print("fn=");
            Serial.println(fn);
            Serial.print(logstr);
        }
    }

    delay(1000);
}
