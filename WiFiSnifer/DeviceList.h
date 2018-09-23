#ifndef DeviceList_h
#define DeviceList_h

#include "LinkedList.h"
#include "Device.h"

class DeviceList {
  public:
   DeviceList();

   void add(Device mac);
   void clear();
   int  getSize();
   int contains(uint8_t mac[6]);
   Device  getDevice(uint8_t mac[6]);
   Device  getDevice(int i);

  private:
    LinkedList<Device>* list = new LinkedList<Device>();
};

#endif
