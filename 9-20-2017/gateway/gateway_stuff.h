#ifndef gateway_stuff
#define gateway_stuff

#define GATEWAY_ADDRESS     0x00
#define NEW_ELEMENT_ADDRESS 0xFE
#define BROADCAST           0xFF

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

int deviceCount = 1;

MyData data;

#endif
