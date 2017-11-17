#ifndef transceiver_stuff
#define transceiver_stuff

#include <nRF24L01.h>
#include <RF24.h>

#define GATEWAY_ADDR     0x00
#define NEW_ELEMENT_ADDR 0xFE
#define BROADCAST        0xFF
#define PAYLOAD_SIZE     24

const uint64_t pipe = 0xE8E8F0F0E1LL;

struct MyData {
  byte origem;
  byte destino;
  byte hop;
  byte opcode;
  byte id;
  byte checksum;
  byte payload[24];
};

struct Arp {
  byte dynamicAddr;
  //byte physicalAddr[6];
};

struct accessMap {
  byte addr;
  byte cost;
};

enum opcode{PAYLOAD_SEND=0x00,ADDRESS_REQUEST,ADDRESS_RESPONSE,MAP_BROADCAST,MAP_RESPONSE};

typedef struct Arp Arp_t;
typedef struct MyData MyData_t;
typedef struct accessMap accessMap_t;
typedef enum opcode opcode_t;

// The sizeof this struct should not exceed 32 bytes

class Transceiver{
  public:
    void      setPackage(byte origem,byte destino,byte opcode,byte pay1oad[],byte sizeOfPayload);
    MyData_t  getPackage() { return this->data; };
    void      setPackageID(byte id) { this->myPackageId = id; };
    void      incPackageID() { this->myPackageId++; };
    byte      getPackageID() { return this->myPackageId; };
    void      setDynamicAddr(byte addr) { this->myDynamicAddr = addr; };
    byte      getDynamicAddr() { return this->myDynamicAddr; };
    void      setPhysicalAddr(byte *addr);
    byte*     getPhysicalAddr() { return this->myPhysicalAddr; };
    Transceiver();
    ~Transceiver();
    void      clearPayload();
    void      transceiverBegin();
    bool      sendData() ;
    void      recvData();
    void      joinNetwork();
    void      mapRequest();
  private:
    RF24          *radio;
    Arp_t         arp;
    MyData_t      data;
    byte          gatewayAcess;
    byte          myPackageId;
    byte          myDynamicAddr;
    byte          myPhysicalAddr[6];
    accessMap_t    neighbors[252];
};




#endif
