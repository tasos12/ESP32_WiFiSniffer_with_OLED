#ifndef Sniffer_h
#define Sniffer_h

#include <WiFi.h>
#include "esp_wifi.h"
#include "searchFunctions.h"
#include "Display.h"
#include "Device.h"
#include "DeviceList.h"


extern String searchVendor(uint8_t* mac);

extern Network selectedNetwork;
extern Network Networks[MAX_NETS];
extern DeviceList* connectedDevices;
extern Display display;


class Sniffer {
  public:
    enum State{IDLE, STANDBY, INTERRUPTED, AWAKE};
  public:
    void Init(int channel);
    void Stop();
    void setChannel(uint8_t channel);
    bool isAwake();
    bool isInterrupted();
    bool isStandby();
    void switchFlag();
    void switchState(); 
    void resetState();    

  private:
    //void addDevicetoList(uint8_t device[6]);

    bool sniffer_flag = true;
    bool sniffing = true;
    int channel = 0;
    State state = IDLE;
};


#endif
