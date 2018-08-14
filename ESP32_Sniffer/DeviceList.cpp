#include "DeviceList.h"

DeviceList::DeviceList(){
  list = LinkedList<Device>();
}

void DeviceList::add(Device dev){
  list.add(dev);
}

int DeviceList::contains(uint8_t mac[6]){
  int s = list.size();
  Device dev;
  for(int i=0;i<s;i++){
    dev = list.get(i);
    if(dev.compare(mac)) return i;
  }
  //Serial.println("Mac not contained in List");
  return -1;
}

Device DeviceList::getDevice(uint8_t mac[6]){
  return list.get(contains(mac));
}

Device DeviceList::getDevice(int i){
  return list.get(i);
}

int DeviceList::getSize(){
  return list.size();
}

void DeviceList::clear(){
  list.clear();
}

