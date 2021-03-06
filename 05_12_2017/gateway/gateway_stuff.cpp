#include <SPI.h>
#include "nRF24L01.h"
#include "RF24_config.h"
#include "RF24.h"
#include "gateway_stuff.h"

#define debug

void Transceiver::clearPayload(){
  for(int i=0;i<PAYLOAD_SIZE;i++)
    dataSent.payload[i] = 0x00;
}

Transceiver::Transceiver(){
  radio = new RF24(9,10);
  gatewayAcess=0;
  myPhysicalAddr[5] = 0x09;
  int i;
  for(i=0;i<5;i++)
    myPhysicalAddr[i] = 0x00;
  myDynamicAddr = NEW_ELEMENT_ADDR;
  myPackageId = 0;
  neighbor.cost=0;
  neighbor.addr=GATEWAY_ADDR;
}

Transceiver::~Transceiver()
{
   delete radio;
}

void Transceiver::setPackage(byte origem,byte destino,byte opcode,byte payload[],byte sizeOfPayload){
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
  radio->openReadingPipe(1,pipe);
  radio->startListening();
}

bool Transceiver::sendData() 
{
  radio->stopListening();
  radio->openWritingPipe(pipe);   

  bool   write_status;

  write_status = radio->write(&dataSent, sizeof(MyData));

  radio->openReadingPipe(1,pipe);
  radio->startListening();

  return write_status;
}

void Transceiver::recvData()
{
  if ( radio->available() ) {
    radio->read(&dataReceived, sizeof(MyData));
    #ifdef debug
    printf("Package Received!\n");
    printf("RECEBENDO: ORIGEM: %X DESTINO: %X HOP: %d OPCODE: %X ID: %X CHKSUM: %X PAYLOAD: \"%s\"\n",dataReceived.origem, dataReceived.destino, dataReceived.hop, dataReceived.opcode, dataReceived.id, dataReceived.checksum, dataReceived.payload);
    #endif
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
        printf("ADRESS REQUEST\n");
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
      }
      break;
      case MAP_BROADCAST:
        setPackage(GATEWAY_ADDR,dataReceived.origem,MAP_RESPONSE,NULL,0);
        sendData();
        break;
      default:
        #ifdef debug
        printf("Unespecified operation code received.\n");
        #endif
        break;
    }
  }
}

