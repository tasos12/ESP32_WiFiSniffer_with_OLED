#include <WiFi.h>
#include "esp_wifi.h"
#include "SSD1306.h"
#include "OLEDDisplayUi.h"
#include "searchFunctions.h"
#include "data.h"
#include "DeviceList.h"

//OLED Setup
SSD1306 display(0x3c, 5, 4);
OLEDDisplayUi ui ( &display );
int selector = 0;

//Sniffer Information
bool sniffer_flag = false;
bool sniffing = false;
int channel = 0;

//Network Scanning Information
int networks;
Network selectedNetwork;
Network Networks[MAX_NETS];

/* Brief: Callback function when sniffer captures a packet
   Params: void *buff: Buffer that contains the packet captured
           wifi_promiscuous_pkt_type_t type: Passed to promiscuous mode RX callback to indicate the type of parameter in the buffer.
*/
void wifi_sniffer_packet_handler(void *buff, wifi_promiscuous_pkt_type_t type) {

  wifi_promiscuous_pkt_t *ppkt  = (wifi_promiscuous_pkt_t *)buff;
  wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
  wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;


  //Finding new device and adding it to the List
  addDevicetoList(hdr->addr1);
  addDevicetoList(hdr->addr2);

  String sender = searchVendor(hdr->addr2);
  Serial.print("Sender: " + sender + " ");
  printMac(hdr->addr2);

  String receiver = searchVendor(hdr->addr1);
  Serial.print("Receiver: " + receiver + " ");
  printMac(hdr->addr1);

  Serial.println();
}

/* Brief: Adds the device to the device list of the sniffed network
 * Params: uint8_t devicep[6]: Mac address of the device to be added
 */
void addDevicetoList(uint8_t device[6]) {

  if (connectedDevices.contains(device) == -1) {
    if(!compareMacs(device, INVALID_MAC)){
      //Serial.println("Not invalid mac");
      if(!compareMacs(device, selectedNetwork.bssid)){
        String vendor = searchVendor(device);
        //Create Device Object
        Device dev;
        dev.setMac(device);
        dev.setVendor(vendor);
        
        connectedDevices.add(dev);
        displayDevices();
        Serial.print("Device added to List size:");
        Serial.println(connectedDevices.getSize());
      }
    }
  }
}

/*Brief: Prints the mac given
*/
void printMac(uint8_t* mac) {
  Serial.print(mac[0]);
  for (int i = 1; i < 6; i++) {
    Serial.print(":");
    Serial.print(mac[i]);
  }
  Serial.println();
}

/*Brief: Initializes the sniffer

*/
void wifiSnifferInit() {
  WiFi.disconnect(true);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(&wifi_sniffer_packet_handler);
  sniffing = true;
}

/*Brief: Stops the sniffer
*/
void wifiSnifferStop() {
  esp_wifi_set_promiscuous(false);
  sniffing = false;
}

/* Brief: Sets the sniffers channel to scan
   Params: uint8_t channel: the number of the channel
*/
void wifiSnifferSetChannel(uint8_t channel) {
  if (channel <= MAX_CHANNEL && channel > 0)
    ESP_ERROR_CHECK(esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE));
}


/* Brief: Initializing the OLED screen options
*/
void initOLED() {
  display.setColor(WHITE);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.init();
}

/*Brief: Displays the devices found at the OLED screen
 */
void displayDevices(){
  display.clear();
  display.drawString(0, 1, "Devices: ");
  for(int i=0; i<connectedDevices.getSize(); i++){
    Device device = connectedDevices.getDevice(i);
    display.drawString(0, (i+1) * 10 + 1, device.getVendor());
  }
  display.display();
}


/* Brief: Displays the networks available and the one selected
   Params: int selector: Points to the selected network
*/
void displayNets(int selector) {
  display.clear();
  display.drawString(0, 1, "Networks: ");
  if (networks > 0) {
    for (int i = 0; i < networks; i++)
      display.drawString(0, (i + 1) * 10 + 1, Networks[i].ssid);
    display.drawHorizontalLine(0, (selector + 1) * 10 + 11, 116);
  }
  else
    display.drawString(0, 11, "No networks found");
  display.display();
}

/* Brief: Sets the options for scanning for networks

*/
void scanNets() {
  WiFi.mode(WIFI_AP_STA);
  //**Number of Networks found
  networks =  WiFi.scanNetworks();
  Serial.println(networks);

  if (networks > MAX_NETS) {
    networks = MAX_NETS;
  }
  for (int i = 0; i < networks; i++) {
    Networks[i].ssid = WiFi.SSID(i);
    Networks[i].channel = WiFi.channel(i);
    Networks[i].rssis = WiFi.RSSI(i);
    Networks[i].bssid = WiFi.BSSID(i);
  }
}

/* Brief: Returns the button pressed

*/
int buttonPressed() {
  if (touchRead(T2) < 20) return 2;   //15
  else if (touchRead(T3) < 20) return 3; //2
  else if (touchRead(T4) < 20) return 4; //13
  else return -1;
}




void setup()
{
  Serial.begin(115200);

  initOLED();

  scanNets();

  displayNets(selector);
}

void loop()
{

  while (buttonPressed() == -1) {} //Wait for a button to be pressed

  if (buttonPressed() == 2) {     //Down (15)
    selector = (selector + 1) % networks;
    displayNets(selector);
  }
  else if (buttonPressed() == 3) { //Up (2)
    if (selector > 0)
      selector = (selector - 1) % networks;
    displayNets(selector);
  }
  else if (buttonPressed() == 4) { //Select network to sniff (13)
    selectedNetwork = Networks[selector];
    connectedDevices.clear();
    sniffer_flag = !sniffer_flag;
  }

  if (sniffer_flag && !sniffing) {
    wifiSnifferInit();
    wifiSnifferSetChannel(selectedNetwork.channel);
    Serial.print("Sniffing SSID: ");
    Serial.print(selectedNetwork.ssid);
    Serial.print(" Channel: ");
    Serial.println(selectedNetwork.channel);
  }
  else if (sniffing && !sniffer_flag) {
    wifiSnifferStop();
    scanNets();
    displayNets(selector);
    Serial.println("Sniffer Stopped");
    delay(1000);
  }
  delay(100);
}





