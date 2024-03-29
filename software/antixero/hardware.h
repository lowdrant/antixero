/**
 * Hardware configuration header file
 */
#ifndef _HARDWARE_H
#define _HARDWARE_H

#include "DHT.h"

#define BAUD (115200)
#define RAIN_PIN (A0)
#define LOG_T_MS ((unsigned long) 1000)

/* DHT pinout + type*/
#define DHT_PIN ((uint8_t) 8)
#define DHT_TYPE DHT11

/* LCD pinout */
#define LCD_T_RST_MS ((unsigned long) 10000)
#define LCD_ADDR (0x27)
#define LCD_COLS ((uint8_t) 16)
#define LCD_ROWS ((uint8_t) 2)

/* SD Card */
#define SD_CHIPDETECT ((uint8_t) 7)
#define SD_CS ((uint8_t) 4)
#define SD_FN_BASE ("humlog")
#define SD_FN_SUF ("csv")
#define SD_MAX_DIGITS ((int) 5)

#endif /* _PINOUT_H */
