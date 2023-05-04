#ifndef _HELPERS_H
#define _HELPERS_H
#include <SD.h>
#include <string.h>
#include <ctype.h>
#include "hardware.h"

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

/**
 * get logfile number suffix
 */
long get_fn_num(const char *const fn) {
    /* copy string to avoid overwrite */
    char fntmp[strlen(fn) + 1];
    strncpy(fntmp, fn, strlen(fn) + 1);

    /* find digit substring */
    char delim[2] = { SD_FN_SUF[0], '\0' };
    char nums[SD_MAX_DIGITS + 1];
    char *substr = strtok(fntmp, delim);
    size_t baselen = strlen(SD_FN_BASE);
    strncpy(nums, substr + baselen, SD_MAX_DIGITS);

    /* substring to number */
    long num = atol(nums);
//    Serial.print("delim=");Serial.print(delim);
//    Serial.print(", fntmp=");Serial.print(fntmp);
//    Serial.print(", substr=");Serial.print(substr);
//    Serial.print(", nums=");Serial.print(nums);
//    Serial.print(", num=");Serial.println(num);
//    printf("delim=%s, fntmp=%s, substr=%s, nums=%s, num=%d\n",
//           delim, fntmp, substr, nums, num);
    return num;
}

/*
 * Avoid overwriting data files by using SD_FN in hardware.h
 * as a prefix, and then attaching a suffix to 
 */
int getDatalogNum(File dir) {
    /* string comparison vars */
    size_t baselen = strlen(SD_FN_BASE);
    char fnbase[baselen + SD_MAX_DIGITS + 1];
    char *fnsuf;
    long num = 0;
    long curnum = 0;

    /* traverse files */
    long i;
    size_t fnlen = 50;
    char fn[fnlen + 1];
    dir.rewindDirectory();
    while (true) {
        /* null check */
        File entry = dir.openNextFile();
        if (!entry) {
            break;
        }
        Serial.print("entry=");
        Serial.println(entry.name());

        if (!entry.isDirectory()) {
            /* entry uppercases everything, so lower */
            for (i = 0; i < strlen(entry.name()); i++) {
                if (i > fnlen) {
                    Serial.println("Max filename exceeded for intermed var");
                    entry.close();
                    return -1;
                }
                fn[i] = tolower(entry.name()[i]);
            }
            fn[i] = '\0';

            /* see if correct filetype */
            fnsuf = strstr(fn, SD_FN_SUF);
            if (fnsuf != NULL) {
                /* check if correct basename */
                if (strncmp(SD_FN_BASE, fn, baselen) == 0) {
                    curnum = get_fn_num(fn);
                    if (curnum >= num) {
                        num = curnum + 1;
                    }
                }
            }
            entry.close();
            Serial.print("fn=");
            Serial.print(fn);
            Serial.print(", num=");
            Serial.print(num);
            Serial.print(", curnum=");
            Serial.println(curnum);
        }
    }
    return num;
}

#endif
