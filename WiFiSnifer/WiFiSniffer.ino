#include "Sniffer.h"
#include "DeviceList.h"
#include "Display.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

//WebServer
AsyncWebServer server(80);
const char* ssid     = "ESP32-Access-Point";
const char* password = "tasos121";

//Network Scanning Information
int networks;
Network selectedNetwork;
Network Networks[MAX_NETS];

Sniffer sniffer;
Display display;
DeviceList* connectedDevices = new DeviceList();

int selector = 0;


/* Brief: Sets the options for scanning for networks
*/
void scanNets() {
  display.displayMessage(0, 1, "Scanning Networks...");
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


void initServer(){
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  String response;
  response += htmlStart;
  response += "<tr>";
  for(int i=0;i<connectedDevices->getSize();i++){
    response += "<td>";
    response += connectedDevices->getDevice(i).getMac2String();
    response += "</td>";
  }
  response += "</tr>";
  response += htmlEnd;
  
  server.on("/html", HTTP_GET, [response](AsyncWebServerRequest *request){
    request->send(200, "text/html", response);
  });

  server.begin();  
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
  display.initDisplay();
  scanNets();
  display.displayNets(selector, networks, Networks);
  //initServer();
}

void loop()
{

  while (buttonPressed() == -1) {} //Wait for a button to be pressed

  if (buttonPressed() == 2) {     //Down (15)
    selector = (selector + 1) % networks;
    display.displayNets(selector, networks, Networks);
  }
  else if (buttonPressed() == 3) { //Up (2)
    if (selector > 0)
      selector = (selector - 1) % networks;
    display.displayNets(selector, networks, Networks);
  }
  else if (buttonPressed() == 4) { //Select network to sniff (13)
    selectedNetwork = Networks[selector];
    connectedDevices->clear();
    sniffer.switchState();
  }

  if (sniffer.isAwake()) {
    sniffer.resetState();
    sniffer.Init(selectedNetwork.channel);
    String str = "Scanning " + selectedNetwork.ssid + " at ch:" + selectedNetwork.channel;
    display.displayMessage(0, 1, str);
    display.displayMessage(2, 1, "Press again to show results");
    Serial.print("Sniffing SSID: ");
    Serial.print(selectedNetwork.ssid);
    Serial.print(" Channel: ");
    Serial.println(selectedNetwork.channel);
    delay(1000);
  }
  else if (sniffer.isInterrupted()) {
    sniffer.resetState();
    sniffer.Stop(); 
    Serial.println("Sniffer Stopped");
    delay(1000);
  }
  else if(sniffer.isStandby()){
    sniffer.resetState();
    scanNets();
    display.displayNets(selector, networks, Networks);
  }
  delay(100);
}





