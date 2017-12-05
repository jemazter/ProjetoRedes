#ifndef gateway_stuff
#define gateway_stuff

#include </home/pi/RF24/nRF24L01.h>
#include </home/pi/RF24/RF24.h>

#define GATEWAY_ADDR     0x00
#define NEW_ELEMENT_ADDR 0xFE
#define BROADCAST        0xFF
#define PAYLOAD_SIZE     24

const uint64_t PIPE = 0xE8E8F0F0E1LL;

struct MyData {
  unsigned char origem;
  unsigned char destino;
  unsigned char hop;
  unsigned char opcode;
  unsigned char id;
  unsigned char checksum;
  unsigned char payload[24];
};

struct Arp {
  unsigned char dynamicAddr;
  //unsigned char physicalAddr[6];
};

struct accessMap {
  unsigned char addr;
  unsigned char cost;
};

enum opcode{PAYLOAD_SEND=0x00,ADDRESS_REQUEST,ADDRESS_RESPONSE,MAP_BROADCAST,MAP_RESPONSE};

typedef struct Arp Arp_t;
typedef struct MyData MyData_t;
typedef struct accessMap accessMap_t;
typedef enum opcode opcode_t;

// The sizeof this struct should not exceed 32 bytes

class Transceiver{
  public:
    void      setPackage(unsigned char origem,unsigned char destino,unsigned char opcode,unsigned char pay1oad[],unsigned char sizeOfPayload);
    MyData_t  getPackage() { return this->dataSent; };
    void      setPackageID(unsigned char id) { this->myPackageId = id; };
    void      incPackageID() { this->myPackageId++; };
    unsigned char      getPackageID() { return this->myPackageId; };
    void      setDynamicAddr(unsigned char addr) { this->myDynamicAddr = addr; };
    unsigned char      getDynamicAddr() { return this->myDynamicAddr; };
    void      setPhysicalAddr(unsigned char *addr);
    unsigned char*     getPhysicalAddr() { return this->myPhysicalAddr; };
    Transceiver();
    ~Transceiver();
    void      clearPayload();
    void      transceiverBegin();
    bool      sendData() ;
    void      recvData();
    void      mapRequest();
  private:
    RF24          *radio;
    Arp_t         arp;
    MyData_t      dataSent;
    MyData_t      dataReceived;
    unsigned char          gatewayAcess;
    unsigned char          myPackageId;
    unsigned char          myDynamicAddr;
    unsigned char          myPhysicalAddr[6];
    accessMap_t   neighbors[252];
    unsigned char          deviceCount;
};




#endif
