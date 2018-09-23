#include "Display.h"

Display::Display():display(0x3c, 5, 4), ui( &display ){
}

/* Brief: Initializing the OLED screen options
*/
void Display::initDisplay() {
  display.setColor(WHITE);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.init();
}


/* Brief: Displays the networks available and the one selected
   Params: int selector: Points to the selected network
*/
void Display::displayNets(int selector, int _networks, Network _Networks[]) {
  display.clear();
  display.drawString(0, 1, "Networks: ");
  if (_networks > 0) {
    for (int i = 0; i < _networks; i++)
      display.drawString(0, (i + 1) * 10 + 1, _Networks[i].ssid);
    display.drawHorizontalLine(0, (selector + 1) * 10 + 11, 116);
  }
  else
    display.drawString(0, 11, "No networks found");
  display.display();
}

/*Brief: Displays the devices found at the OLED screen
 */
void Display::displayDevices(DeviceList* connectedDevices){
  display.clear();
  display.drawString(0, 1, "Devices: ");
  Serial.print("Devices: ");
  Serial.println(connectedDevices->getSize());
  for(int i=0; i<connectedDevices->getSize(); i++){
    Serial.print("Device ");
    Serial.print(i);
    Device device = connectedDevices->getDevice(i);
    display.drawString(0, (i+1) * 10 + 1, device.getVendor());
  }
  display.display();
}

void Display::displayMessage(int row, int coll, String message){
  display.clear();
  display.drawString(row, coll, message);
  display.display();
}


