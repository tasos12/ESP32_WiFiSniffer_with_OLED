#include "Sniffer.h"

/* Brief: Adds the device to the device list of the sniffed network
   Params: uint8_t devicep[6]: Mac address of the device to be added
*/
void addDevicetoList(uint8_t device[6]) {
  if (!compareMacs(device, INVALID_MAC) && !compareMacs(device, selectedNetwork.bssid) && connectedDevices->contains(device) == -1) {
    String vendor = searchVendor(device);

    //Create Device Object
    Device dev;
    dev.setDevInfo(device, vendor);
    connectedDevices->add(dev);

    display.displayDevices(connectedDevices);  
    
    Serial.print("Device added to List size:");
    Serial.println(connectedDevices->getSize());

  }
}


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

/*Brief: Initializes the sniffer
*/
void Sniffer::Init(int channel) {
  WiFi.disconnect(true);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(&wifi_sniffer_packet_handler);
  setChannel(channel);
  //sniffing = true;
}

/*Brief: Stops the sniffer
*/
void Sniffer::Stop() {
  esp_wifi_set_promiscuous(false);
  //sniffing = false;
}

/* Brief: Sets the sniffers channel to scan
   Params: uint8_t channel: the number of the channel
*/
void Sniffer::setChannel(uint8_t channel) {
  if (channel <= MAX_CHANNEL && channel > 0)
    ESP_ERROR_CHECK(esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE));
}

bool Sniffer::isAwake() {
  if (!sniffer_flag && sniffing) return true;
  return false;
}

bool Sniffer::isInterrupted() {
  if (!sniffing && sniffer_flag) return true;
  return false;
}

bool Sniffer::isStandby() {
  if (!sniffing && !sniffer_flag) return true;
  return false;
}

void Sniffer::switchFlag() {
  sniffer_flag = !sniffer_flag;
}

void Sniffer::switchState() {
  if (state == AWAKE) {
    sniffer_flag = true;
    sniffing = false;
    state = INTERRUPTED;
  }
  else if (state == INTERRUPTED) {
    sniffer_flag = false;
    sniffing = false;
    state = STANDBY;
  }
  else if (state == STANDBY) {
    sniffer_flag = true;
    sniffing = true;
    state = IDLE;
  }
  else {
    sniffer_flag = false;
    sniffing = true;
    state = AWAKE;
  }
}

void Sniffer::resetState() {
  sniffing = true;
  sniffer_flag = true;
}




