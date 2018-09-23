#ifndef searchFunctions_h
#define searchFunctions_h

#include  "data.h"
//#include <WiFi.h>

/*----------Functions take by https://github.com/spacehuhn/esp8266_deauther functions.h-----------------------------*/
inline int binSearchVendors(uint8_t* searchBytes, int lowerEnd, int upperEnd) {
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

inline String searchVendor(uint8_t* mac) {
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

    if (vendorName == "") vendorName = "Uknown Device";
    
    return vendorName;
}

inline bool compareMacs(uint8_t mac1[6], uint8_t mac2[6]){
  for(int i=0;i<6;i++){
    if(mac1[i] != mac2[i]) return false;
  }
  return true;
}

/*Brief: Prints the mac given
*/
inline void printMac(uint8_t* mac) {
  Serial.print(mac[0]);
  for (int i = 1; i < 6; i++) {
    Serial.print(":");
    Serial.print(mac[i]);
  }
  Serial.println();
}


#endif
