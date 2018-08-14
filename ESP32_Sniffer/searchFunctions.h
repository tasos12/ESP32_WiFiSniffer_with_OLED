#ifndef searchFunctions_h
#define searchFunctions_h

#include "data.h"

/*----------Functions take by https://github.com/spacehuhn/esp8266_deauther functions.h-----------------------------*/
String bytesToStr(uint8_t* b, uint32_t size) {
    String str;

    for (uint32_t i = 0; i < size; i++) {
        if (b[i] < 0x10) str += '0';
        str += String(b[i], HEX);

        if (i < size - 1) str += ':';
    }
    return str;
}

String macToStr(uint8_t* mac) {
    return bytesToStr(mac, 6);
}

bool strToMac(String macStr, uint8_t* mac) {
    macStr.replace(String(':'), String());  // ":" -> ""
    macStr.replace("0x", String());                 // "0x" -> ""
    macStr.replace(String(','), String());        // "," -> ""
    macStr.replace(String('\"'), String()); // "\"" -> ""
    macStr.toUpperCase();

    if (macStr.length() != 12) {
        Serial.println("Wrong mac format");
        return false;
    }

    for (uint8_t i = 0; i < 6; i++) mac[i] = strtoul((macStr.substring(i * 2, i * 2 + 2)).c_str(), NULL, 16);

    return true;
}

int binSearchVendors(uint8_t* searchBytes, int lowerEnd, int upperEnd) {
    uint8_t listBytes[3];
    int     res;
    int     mid = (lowerEnd + upperEnd) / 2;

    while (lowerEnd <= upperEnd) {
        listBytes[0] = pgm_read_byte_near(data_macs + mid * 5);
        listBytes[1] = pgm_read_byte_near(data_macs + mid * 5 + 1);
        listBytes[2] = pgm_read_byte_near(data_macs + mid * 5 + 2);

        res = memcmp(searchBytes, listBytes, 3);

        if (res == 0) {
            return mid;
        } else if (res < 0) {
            upperEnd = mid - 1;
            mid      = (lowerEnd + upperEnd) / 2;
        } else if (res > 0) {
            lowerEnd = mid + 1;
            mid      = (lowerEnd + upperEnd) / 2;
        }
    }

    return -1;
}

String searchVendor(uint8_t* mac) {
    String vendorName = String();
    int    pos        = binSearchVendors(mac, 0, sizeof(data_macs) / 5 - 1);
    int    realPos    = pgm_read_byte_near(data_macs + pos * 5 + 3) | pgm_read_byte_near(data_macs + pos * 5 + 4) << 8;

    if (pos >= 0) {
        char tmp;

        for (int i = 0; i < 8; i++) {
            tmp = (char)pgm_read_byte_near(data_vendors + realPos * 8 + i);

            if (tmp != '\0') vendorName += tmp;
            tmp += ' ';
        }
    }

    return vendorName;
}

bool compareMacs(uint8_t mac1[6], uint8_t mac2[6]){
  for(int i=0;i<6;i++){
    if(mac1[i] != mac2[i]) return false;
  }
  return true;
}

#endif
