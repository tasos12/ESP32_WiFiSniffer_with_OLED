#ifndef Device_h
#define Device_h

#include <Arduino.h>

class Device{
  public:
    Device();
    bool    compare(uint8_t targetMac[6]);
    void    getMac(uint8_t* targetMac);
    String  getMac2String();
    uint8_t getMacPart(int num);
    String  getVendor();
    void    setMac(uint8_t targetMac[6]);
    void    setVendor(String targetVendor);
    void    setDevInfo(uint8_t targetMac[6], String targetVendor);

  private:
    uint8_t mac[6];
    String vendor;
};
#endif

