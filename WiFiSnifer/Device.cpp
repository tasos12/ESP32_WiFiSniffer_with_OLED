#include "Device.h"

Device::Device(){
  for(int i=0;i<6;i++){
    mac[i] = 0x00;
  }
  vendor = "";
}

bool Device::compare(uint8_t targetMac[6]){
  for(int i=0;i<6;i++){
    if(mac[i] != targetMac[i]) return false;
  }
  return true;
}

void Device::getMac(uint8_t* targetMac){
  targetMac = mac;
}

String Device::getMac2String(){
  String address;
  for(int i=0;i<6;i++)
    address += getMacPart(i);
  return address;
}

uint8_t Device::getMacPart(int num){
  return mac[num];
}

String Device::getVendor(){
  return vendor;
}

void Device::setMac(uint8_t targetMac[6]){
  for(int i=0;i<6;i++)
    mac[i] = targetMac[i];
}

void Device::setVendor(String targetVendor){
  vendor = targetVendor;
}

void Device::setDevInfo(uint8_t targetMac[6], String targetVendor){
  setMac(targetMac);
  setVendor(targetVendor);
}

