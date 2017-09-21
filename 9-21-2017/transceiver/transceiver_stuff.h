#ifndef transceiver_stuff
#define transceiver_stuff

#define GATEWAY_ADDR     0x00
#define NEW_ELEMENT_ADDR 0xFE
#define BROADCAST        0xFF

// The sizeof this struct should not exceed 32 bytes
struct MyData {
  byte origem;
  byte destino;
  byte hop;
  byte opcode;
  byte id;
  byte checksum;
  byte payload[24];
};

enum Opcode { payload, addrRequest, addrAssign };

struct Arp {
  byte dynamicAddr;
  //byte physicalAddr[6];
};

Arp arpTable [255];

const byte MY_PHYSICAL_ADDR[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x07};
byte myDynamicAddr = NEW_ELEMENT_ADDR;

byte myPackageId = 0;

MyData data;

#endif
