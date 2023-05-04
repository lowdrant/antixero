#include "hardware.h"
#include "DHT.h"
#include <SPI.h>
#include <SD.h>
#include <LiquidCrystal_I2C.h>
#include <string.h>
#include "helpers.h"

String fn = String(SD_FN_BASE); // +String(SD_FN_SUF);
float hum, temp_c, temp_f, hi_c, hi_f;
bool rain = 0;
DHT dht(DHT_PIN, DHT_TYPE);
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_ROWS, LCD_COLS);

char lcd_str[LCD_ROWS][LCD_COLS];

File root, logFile;

unsigned long timestamp;

void setup() {
    /* init communciations */
    Serial.begin(BAUD);
    lcd.init();
    lcd.backlight();

    dht.begin();

    /* sd card error loop / alert user */
    while (!SD.begin(SD_CS)) {
        lcd.clear();
        lcd.print("SD card failure");
        Serial.println("SD card failure");
        delay(200);
    }
    lcd.clear();
    lcd.print("card initialized.");

    root = SD.open("/");
//  printDirectory(root,0);
    int num = getDatalogNum(root);
    root.close();

    fn += String(num) + String(SD_FN_SUF);
    Serial.println(fn);
    logFile = SD.open(fn, FILE_WRITE);
    if (logFile) {
        logFile.println("time,humidity,rain,");
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
    snprintf(lcd_str[0], LCD_COLS, "Hum:%d%% Rain:%d", (int)hum, rain);
    snprintf(lcd_str[1], LCD_COLS, "Time:%ulms", timestamp);
    lcd.clear();
    lcd.print(lcd_str[0]);
    lcd.setCursor(0, 1);
    lcd.print(lcd_str[1]);

    /* Logging */
    String tmpfn = String(SD_FN_BASE) + String(SD_FN_SUF);
    Serial.println(fn);
    logFile = SD.open(fn, FILE_WRITE);
    if (logFile) {
        logFile.print(timestamp);
        logFile.print(",");
        logFile.print(hum);
        logFile.print(",");
        logFile.print(rain);
        logFile.println(",");
        logFile.close();
    } else {
        lcd.clear();
        lcd.print("logfile error");
        Serial.println("logfile error");
    }

    delay(1000);
}
