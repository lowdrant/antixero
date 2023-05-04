#ifndef _HELPERS_H
#define _HELPERS_H
#include <SD.h>
#include <string.h>
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

/*
 * Avoid overwriting data files by using SD_FN in hardware.h
 * as a prefix, and then attaching a suffix to 
 */
int getDatalogNum(File dir) {
    int num = 0;
    size_t baselen = strlen(SD_FN_BASE);
    String fnbase = String(SD_FN_BASE);
    while (true) {
        File entry = dir.openNextFile();
        if (!entry) {
            break;
        }

        String nm = String(entry.name());
        nm.toLowerCase();

        if (strncmp(nm.substring(0, baselen).c_str(), fnbase.c_str(), baselen)
            == 0) {
            Serial.println("entered statement");
            size_t numlen = nm.length() - baselen;
            if (numlen > 4) { // remove filesuffix
                numlen -= 4;
            }
            String curnum_substr = nm.substring(baselen); // , numlen);
            int curnum = curnum_substr.toInt();
            Serial.print("nm=");
            Serial.print(nm);
            Serial.print("  curnum_substr=");
            Serial.print(curnum_substr);
            Serial.print(" curnum=");
            Serial.println(curnum);
            Serial.print("baselen=");
            Serial.print(baselen);
            Serial.print(" numlen=");
            Serial.println(numlen);
            if (curnum >= num) {
                Serial.println("curnum>=num");
                num = curnum + 1;
                Serial.println(num);
            }
        }
    }
    return num;
}

#endif
