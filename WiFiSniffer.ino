#include <WiFi.h>
#include "esp_wifi.h"
#include "SSD1306.h"
#include "OLEDDisplayUi.h"

#define MAX_CHANNEL 13  //Maximum wifi channels
#define MAX_NETS 6      //Max number of netowrks found


//Packet definition
typedef struct {
  unsigned frame_ctrl:16;
  unsigned duration_id:16;
  uint8_t addr1[6]; /* receiver address */
  uint8_t addr2[6]; /* sender address */
  uint8_t addr3[6]; /* filtering address */
  uint16_t sequence_ctrl;
  uint8_t addr4[6]; /* optional */
} wifi_ieee80211_mac_hdr_t;

typedef struct {
  wifi_ieee80211_mac_hdr_t hdr;
  uint8_t payload[0]; /* network data ended with 4 bytes csum (CRC32) */
} wifi_ieee80211_packet_t;

//OLED Setup
SSD1306 display(0x3c,5,4);
OLEDDisplayUi ui ( &display );
int selector = 0;

//Sniffer Information
bool sniffer_flag= false;
bool sniffing = false;
int channel = 0;

//Network Scanning Information
int networks;
int channels[MAX_NETS];
String ssids[MAX_NETS];
int rssis[MAX_NETS];

typedef struct {
  String ssid;
  int channel;
  int rssis;
}Network;

Network selectedNetwork;

void setup()
{
  Serial.begin(115200);

  initOLED();

  scanNets();

  displayNets(selector);
  
  wifiSnifferInit();
}

void loop()
{

  while(buttonPressed() == -1){} //Wait for a button to be pressed
  
  if(buttonPressed() == 2){       //Down (15)
    selector = (selector + 1) % networks;
    displayNets(selector);
  }
  else if(buttonPressed() == 3){  //Up (2)
    if(selector > 0)
      selector = (selector - 1) % networks;  
    displayNets(selector);
  }
  else if(buttonPressed() == 4){ //Select network to sniff (13)
    sniffer_flag = !sniffer_flag;
    selectedNetwork.ssid = ssids[selector];
    selectedNetwork.channel = channels[selector];
    selectedNetwork.rssis = rssis[selector];
  }

  if(sniffer_flag && !sniffing){
    wifiSnifferInit();
    wifiSnifferSetChannel(selectedNetwork.channel);
    Serial.print("Sniffing SSID: ");
    Serial.print(selectedNetwork.ssid);
    Serial.print(" Channel: ");
    Serial.println(selectedNetwork.channel);
  }
  else if(sniffing && !sniffer_flag){
    wifiSnifferStop();
    Serial.println("Sniffer Stopped");
  }
  
  delay(100);
}


/*Brief: Initializes the sniffer
 * 
 */
void wifiSnifferInit(){
  WiFi.disconnect(true);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(&wifi_sniffer_packet_handler);
  sniffing = true;
}

/*Brief: Stops the sniffer
 */
void wifiSnifferStop(){
  esp_wifi_set_promiscuous(false);
  sniffing = false;  
}

/* Brief: Sets the sniffers channel to scan
 * Params: uint8_t channel: the number of the channel
*/
void wifiSnifferSetChannel(uint8_t channel){
  if(channel<=MAX_CHANNEL && channel>0){
    ESP_ERROR_CHECK(esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE));
  }
}

/* Brief: Callback function when sniffer captures a packet
 * Params: void *buff: Buffer that contains the packet captured
 *         wifi_promiscuous_pkt_type_t type: Passed to promiscuous mode RX callback to indicate the type of parameter in the buffer.
 */
void wifi_sniffer_packet_handler(void *buff, wifi_promiscuous_pkt_type_t type){
  
  const wifi_promiscuous_pkt_t *ppkt  = (wifi_promiscuous_pkt_t *)buff;
  const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
  const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;

  Serial.print("Sender: ");
  Serial.print(hdr->addr2[0]);
  for(int i=1;i<6;i++){
    Serial.print(":");
    Serial.print(hdr->addr2[i]);
  }
  
  Serial.print("  Receiver: ");
  Serial.print(hdr->addr1[0]);
  for(int i=1;i<6;i++){
    Serial.print(":");
    Serial.print(hdr->addr1[i]);    
  }
  Serial.println();
}

/* Brief: Initializing the OLED screen options
 * 
 */
void initOLED(){
  display.setColor(WHITE);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.init();
}

/* Brief: Displays the networks available and the one selected
 * Params: int selector: Points to the selected network
 */
void displayNets(int selector){
  display.clear();
  display.drawString(0,1, "Networks: ");
  if(networks>0){
    for(int i =0;i<networks;i++)
      display.drawString(0,(i+1)*10+1, ssids[i]);
    display.drawHorizontalLine(0, (selector+1)*10 + 11, 116);
  }
  else
    display.drawString(0,11, "No networks found");
  display.display();
}

/* Brief: Sets the options for scanning for networks
 * 
 */
void scanNets(){
  WiFi.mode(WIFI_AP_STA);
  //**Number of Networks found
  networks =  WiFi.scanNetworks();
  Serial.println(networks);

  for(int i=0;i<networks;i++){  
    if(networks<MAX_NETS){  
      ssids[i] = WiFi.SSID(i);
      channels[i] = WiFi.channel(i);
      rssis[i] = WiFi.RSSI(i);
    }
    else networks = MAX_NETS;
  }  
}

/* Brief: Returns the button pressed
 * 
 */
int buttonPressed(){
  if(touchRead(T2)<20) return 2;      //15
  else if(touchRead(T3)<20) return 3; //2
  else if(touchRead(T4)<20) return 4; //13
  else return -1;
}




