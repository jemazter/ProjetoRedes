#ifndef transceiver_stuff
#define transceiver_stuff

#include <nRF24L01.h>
#include <RF24.h>

#define TRUE 1
#define FALSE 0

#define GATEWAY_ADDR      0x00
#define NEW_ELEMENT_ADDR  0xFE
#define BROADCAST         0xFF
#define PAYLOAD_SIZE      24
#define TOTAL_NEIGHBORS   50
#define TOTAL_NUMBER_OF_ELEMENTS 250

const uint64_t pipe = 0xE8E8F0F0E1LL;

using namespace std;

struct MyData {
  byte origem;
  byte destino;
  byte hop;
  byte opcode;
  byte id;
  byte checksum;
  byte payload[24];
};

struct vectorStruct{
  byte sizeOfVector;
  byte ID[TOTAL_NUMBER_OF_ELEMENTS];
};

enum opcode{PAYLOAD_SEND=0x00,ADDRESS_REQUEST,ADDRESS_RESPONSE,MAP_RESPONSE};

typedef struct Arp Arp_t;
typedef struct MyData MyData_t;
typedef struct vectorStruct vectorStruct_t;
typedef enum opcode opcode_t;

// The sizeof this struct should not exceed 32 bytes

class Transceiver{
  public:
    void      setPackage(byte origem,byte destino,byte opcode,byte id,byte hop,byte payload[],byte sizeOfPayload);
    MyData_t  getPackage() { return this->sentData; };
    void      setPackageID(byte id) { this->myPackageId = id; };
    void      incPackageID() { this->myPackageId++; };
    byte      getPackageID() { return this->myPackageId; };
    void      setDynamicAddr(byte addr) { this->myDynamicAddr = addr; };
    byte      getDynamicAddr() { return this->myDynamicAddr; };
    void      setPhysicalAddr(byte *addr);
    byte*     getPhysicalAddr() { return this->myPhysicalAddr; };
    void      addNewRecentPackage(byte ID) {recentReceivedPackages.ID[recentReceivedPackages.sizeOfVector++]=ID; };
    void      refreshRecentsPackage();
    bool      checkRecentsPackage(byte ID);
    Transceiver();
    ~Transceiver();
    void      clearPayload();
    void      transceiverBegin();
    bool      sendData();
    bool      retransmitData();
    void      recvData();
    void      joinNetwork();
  private:
    RF24            *radio;
    MyData_t        sentData;
    MyData_t        receivedData;
    byte            gatewayAcess;
    byte            myPackageId;
    byte            myDynamicAddr;
    byte            myPhysicalAddr[6];
    vectorStruct_t  recentReceivedPackages;
};

#endif
