#ifndef Display_h
#define Display_h

#include "data.h"
#include "DeviceList.h"
#include "SSD1306.h"
#include "OLEDDisplayUi.h"

//extern DeviceList connectedDevices;

class Display{
  public:
    Display();
    void initDisplay();
    void displayNets(int selector, int _networks, Network _Networks[]);
    void displayDevices(DeviceList* connectedDevices);
    void displayMessage(int row, int coll, String message);

  private:
    SSD1306       display;
    OLEDDisplayUi ui;
};

#endif

