#include "hardware.h"
#include "DHT.h"
#include <SPI.h>
#include <SD.h>
#include <LiquidCrystal_I2C.h>
#include <string.h>
#include "helpers.h"

String fn = String(SD_FN_BASE); // +String(SD_FN_SUF);
float hum, temp_c, temp_f, hi_c, hi_f;
unsigned int rain = 0;
int fnnum = 0;
DHT dht(DHT_PIN, DHT_TYPE);
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_ROWS, LCD_COLS);

char lcd_str[LCD_ROWS][LCD_COLS + 1];

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
        errMsg(&lcd, F("No SD card"));
        delay(200);
    }

    /* SD Card Reading */
    while (!SD.begin(SD_CS)) {
        errMsg(&lcd, F("SD card failure"));
        delay(200);
    }
    lcd.clear();
    lcd.print(F("card initialized."));

    /* Determine Logfile */
    fn = createDatalog();
    sdUnplugged = false;

    lcd.clear();
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
    snprintf(lcd_str[0], LCD_COLS, "Hum:%d%% Rn:%u", (int)hum, rain);
    padLCDMsg(lcd_str[0]);
    lcd.setCursor(0, 0);
    lcd.print(lcd_str[0]);

    /* Logging */
    /* construct log before opening for time */
    snprintf(logstr, logstr_len, "%lu,%d,%u,\n", timestamp, (int)hum, rain);
    Serial.print(logstr);
    if (!sdPresent()) {
        errMsg(&lcd, F("No SD card"));
        sdUnplugged = true;
    } else {
        /* if card was unplugged, re-open & increment logfile */
        if (sdUnplugged) {
            if (!SD.begin(SD_CS)) {
                errMsg(&lcd, F("SD card error"));
            } else {
                sdUnplugged = false;
                fn = createDatalog();
            }
        }
        /* write logfile */
        logFile = SD.open(fn, FILE_WRITE);
        if (!logFile) {
            errMsg(&lcd, F("logfile error"));
        } else {
            logFile.print(logstr);
            logFile.close();
            errMsg(&lcd, fn.c_str());
        }
    }

    delay(LOG_T_MS);
}
