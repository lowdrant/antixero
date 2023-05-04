/**
 * Hardware configuration header file
 */
#ifndef _HARDWARE_H
#define _HARDWARE_H

#include "DHT.h"

#define BAUD (1000000)
#define RAIN_PIN (A0)

/* DHT pinout + type*/
#define DHT_PIN ((uint8_t) 8)
#define DHT_TYPE DHT11

/* LCD pinout */
#define LCD_ADDR (0x27)
#define LCD_COLS ((uint8_t) 16)
#define LCD_ROWS ((uint8_t) 2)

/* SD Card */
#define SD_CS ((uint8_t) 4)
#define SD_FN_BASE ("humlog")
#define SD_FN_SUF (".csv")

#endif /* _PINOUT_H */
