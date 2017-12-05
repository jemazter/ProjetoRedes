#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include </home/pi/RF24/RF24.h>
#include "/home/pi/RF24/nRF24L01.h"
#include "/home/pi/RF24/RF24_config.h"
#include "gateway_stuff.h"

#define debug

void Transceiver::clearPayload(){
  for(int i=0;i<PAYLOAD_SIZE;i++)
    dataSent.payload[i] = 0x00;
}

Transceiver::Transceiver(){
  //radio = new RF24(9,10);
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

void Transceiver::setPackage(unsigned char origem,unsigned char destino,unsigned char opcode,unsigned char payload[],unsigned char sizeOfPayload){
  dataSent.origem = origem;
  dataSent.destino = destino;
  dataSent.hop = 0;
  dataSent.opcode = opcode;
  dataSent.id = myPackageId++;
  clearPayload();
  for(int i=0;i<sizeOfPayload;i++)
    dataSent.payload[i]= payload[i];
  dataSent.checksum = 0; //FAZER FUNÇÃO DE CHECKSUM
}

void Transceiver::transceiverBegin(){
  radio->begin();
  //radio->setPALevel(RF24_PA_LOW);
  radio->openReadingPipe(1,PIPE);
  radio->printDetails();
  radio->startListening();
}

bool Transceiver::sendData()
{
  radio->stopListening();
  radio->openWritingPipe(PIPE);

  bool   write_status;

  write_status = radio->write(&dataSent, sizeof(MyData));

  radio->openReadingPipe(1,PIPE);
  radio->startListening();

  return write_status;
}

void Transceiver::recvData()
{
  if ( radio->available() ) {
  printf("radio available\n");
    radio->read(&dataReceived, sizeof(MyData));

    opcode_t op = (opcode_t)dataReceived.opcode;

    bool repeatedRequest = true;

    switch( op ) {
      case PAYLOAD_SEND:
        if ( dataReceived.destino == myDynamicAddr ) {
          #ifdef debug
          printf("RECEBENDO: ORIGEM: %X DESTINO: %X HOP: %d OPCODE: %X ID: %X CHKSUM: %X PAYLOAD: \"%s\"\n",dataReceived.origem, dataReceived.destino, dataReceived.hop, dataReceived.opcode, dataReceived.id, dataReceived.checksum, dataReceived.payload);
          #endif
        }
        else {
          if ( dataReceived.hop < 253 ) {
            sendData();
          }
        }
        break;
      case ADDRESS_REQUEST:
      	if ( deviceCount < 253 ) {
                for ( int i = 0; i < 6; i++ ) {
                  if ( dataSent.payload[i] != dataReceived.payload[i] ) {
                    repeatedRequest = false;
                  }
                }

                if ( !repeatedRequest ) {
                  deviceCount++;
                }

                // Informa ao novo membro seu endereço dinâmico
                dataReceived.payload[6] = deviceCount;
      	  setPackage(GATEWAY_ADDR, NEW_ELEMENT_ADDR, ADDRESS_RESPONSE, dataReceived.payload, 7);

                sendData();
                myPackageId++;

                printf("\"%02X:%02X:%02X:%02X:%02X:%02X\" has joined the network and got dynamic address \"%d\".\n", dataReceived.payload[0], dataReceived.payload[1], dataReceived.payload[2], dataReceived.payload[3], dataReceived.payload[4], dataReceived.payload[5], dataReceived.payload[6], dataReceived.payload[7]);
        break;
      case ADDRESS_RESPONSE:
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
