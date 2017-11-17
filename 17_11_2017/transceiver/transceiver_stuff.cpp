#include <SPI.h>
#include "nRF24L01.h"
#include "RF24_config.h"
#include "RF24.h"
#include "transceiver_stuff.h"

#define debug

void Transceiver::clearPayload(){
  for(int i=0;i<PAYLOAD_SIZE;i++)
    data.payload[i] = 0x00;
}

Transceiver::Transceiver(){
  radio = new RF24(9,10);
  gatewayAcess=0;
  myPhysicalAddr[5] = 0x09;
  int i;
  for(i=0;i<5;i++)
    myPhysicalAddr[i] = 0x00;
  for(i=0;i<252;i++){
    neighbors[i].addr=0xFF;
    neighbors[i].cost=0x00;
  }
  myDynamicAddr = NEW_ELEMENT_ADDR;
  myPackageId = 0;
}

Transceiver::~Transceiver()
{
   delete radio;
}

void Transceiver::setPackage(byte origem,byte destino,byte opcode,byte payload[],byte sizeOfPayload){
  data.origem = origem;
  data.destino = destino;
  data.hop = 0;
  data.opcode = opcode;
  data.id = myPackageId++;
  clearPayload();
  for(int i=0;i<sizeOfPayload;i++)
    data.payload[i]= payload[i];
  data.checksum = 0; //FAZER FUNÇÃO DE CHECKSUM
}

void Transceiver::transceiverBegin(){
  radio->begin();
  radio->openReadingPipe(1,pipe);
  radio->startListening();
}

bool Transceiver::sendData() 
{
  radio->stopListening();
  radio->openWritingPipe(pipe);   

  bool   write_status;

  write_status = radio->write(&data, sizeof(MyData));

  radio->openReadingPipe(1,pipe);
  radio->startListening();

  return write_status;
}

void Transceiver::recvData()
{
  if ( radio->available() ) {

    radio->read(&data, sizeof(MyData));

    opcode_t op = (opcode_t)data.opcode;
    
    switch( op ) {
      case PAYLOAD_SEND:
        if ( data.destino == myDynamicAddr ) {
          #ifdef debug
          printf("RECEBENDO: ORIGEM: %X DESTINO: %X HOP: %d OPCODE: %X ID: %X CHKSUM: %X PAYLOAD: \"%s\"\n",data.origem, data.destino, data.hop, data.opcode, data.id, data.checksum, data.payload);
          #endif
        }
        else {
          if ( data.hop < 253 ) {
            sendData();
          }
        }
        break;
      case ADDRESS_REQUEST:
        break;
      case ADDRESS_RESPONSE:
      {
        bool isThisMine = true;
        for ( int i = 0; i < 6; i++ ) {
          if ( data.payload[i] != myPhysicalAddr[i] )
            isThisMine = false;
        }
        if ( isThisMine ) {
          #ifdef debug
          printf("Connecting to the network...\nYour dynamic address is %d.\n", data.payload[6]);
          #endif
          myDynamicAddr = data.payload[6];
        }        
        break;
      }
      case MAP_BROADCAST:
        
        break;
      default:
        #ifdef debug
        printf("Unespecified operation code received.\n");
        #endif
        break;
    }
  }
}

void Transceiver::joinNetwork()
{
  radio->stopListening();
  radio->openWritingPipe(pipe);

  setPackage(NEW_ELEMENT_ADDR,GATEWAY_ADDR,ADDRESS_REQUEST,myPhysicalAddr,6);
  
  if ( sendData() ) {
    #ifdef debug
    printf("Requesting to join the network...\n");
    #endif
    myPackageId++;
  }
  else {
    #ifdef debug
    printf("Trying to reach the network, no response yet...\n");
    #endif
  }
  
  radio->openReadingPipe(1,pipe);
  radio->startListening();
}

void  Transceiver::mapRequest(){
   byte payload[] = {0};
   #ifdef debug
   printf("Sending a map request");
   #endif
   setPackage(myDynamicAddr,BROADCAST,MAP_BROADCAST,payload,1);
   sendData();
}

