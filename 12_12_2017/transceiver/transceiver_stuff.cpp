#include <SPI.h>
#include "nRF24L01.h"
#include "RF24_config.h"
#include "RF24.h"
#include "transceiver_stuff.h"

#define debug

void Transceiver::clearPayload(){
  for(int i=0;i<PAYLOAD_SIZE;i++)
    sentData.payload[i] = 0x00;
}

Transceiver::Transceiver(){
  radio = new RF24(9,10);
  gatewayAcess=0;
  myPhysicalAddr[5] = 0x01;
  int i;
  for(i=0;i<5;i++)
    myPhysicalAddr[i] = 0x00;
  myDynamicAddr = NEW_ELEMENT_ADDR;
  myPackageId = 0;
  refreshRecentsPackage();
}

Transceiver::~Transceiver()
{
   delete radio;
}

void Transceiver::setPackage(byte origem,byte destino,byte opcode,byte id,byte hop,byte payload[],byte sizeOfPayload){
  sentData.origem = origem;
  sentData.destino = destino;
  sentData.hop = hop;
  sentData.opcode = opcode;
  sentData.id = id;
  clearPayload();
  for(int i=0;i<sizeOfPayload;i++)
    sentData.payload[i]= payload[i];
  sentData.checksum = 0; //FAZER FUNÇÃO DE CHECKSUM
}

void Transceiver::refreshRecentsPackage(){
  int i;
  for(i=0;i<recentReceivedPackages.sizeOfVector;i++){
    recentReceivedPackages.ID[i]=0;
  }
}

bool Transceiver::checkRecentsPackage(byte ID){
  int i;
  for(i=0;i<recentReceivedPackages.sizeOfVector;i++){
    if(recentReceivedPackages.ID[i] == ID)
      return TRUE;
  }
  return FALSE;
}

void Transceiver::transceiverBegin(){
  radio->begin();
  radio->printDetails();
  radio->openReadingPipe(1,pipe);
  radio->startListening();
}

bool Transceiver::sendData() 
{
  radio->stopListening();
  radio->openWritingPipe(pipe);   

  bool   write_status;
  write_status = radio->write(&sentData, sizeof(MyData));
  
  myPackageId++;
  
  radio->openReadingPipe(1,pipe);
  radio->startListening();

  return write_status;
}

bool Transceiver::retransmitData() 
{
  radio->stopListening();
  radio->openWritingPipe(pipe);   

  bool   write_status;
  write_status = radio->write(&sentData, sizeof(MyData));

  radio->openReadingPipe(1,pipe);
  radio->startListening();
  radio->stopListening();
  radio->openReadingPipe(1,pipe);
  radio->startListening();
  
  return write_status;
}

void Transceiver::recvData()
{
  if ( radio->available() ) {

    radio->read(&receivedData, sizeof(MyData));

    opcode_t op = (opcode_t)receivedData.opcode;

    if(!checkRecentsPackage(receivedData.id)){
      addNewRecentPackage(receivedData.id);
      switch( op ) {
        case PAYLOAD_SEND:
          if ( receivedData.destino == myDynamicAddr ) {
            #ifdef debug
            printf("RECEBENDO: ORIGEM: %X DESTINO: %X HOP: %d OPCODE: %X ID: %X CHKSUM: %X PAYLOAD:",receivedData.origem
              , receivedData.destino
              , receivedData.hop
              , receivedData.opcode
              , receivedData.id
              , receivedData.checksum);
            for(int i;i<PAYLOAD_SIZE;i++)
              printf("%X",receivedData.payload[i]);
            printf("\n");
            #endif
          }
          else{
            setPackage(receivedData.destino,receivedData.origem,receivedData.opcode,receivedData.id,receivedData.hop+1,receivedData.payload,PAYLOAD_SIZE);
            printf("Resend a package received from %d to %d.\n",receivedData.origem,receivedData.destino);
            if( !sendData()){
              while ( !retransmitData() ) {
                delay(500);
                #ifdef debug
                printf("Trying to reach the network, no response yet...\n");
                #endif
              }
            }
          }
          break;
        case ADDRESS_REQUEST:
        {
          setPackage(receivedData.destino,receivedData.origem,ADDRESS_REQUEST,myPackageId++,0,receivedData.payload,6);
          if( !sendData()){
              while ( !retransmitData() ) {
                delay(500);
                #ifdef debug
                printf("Trying to reach the network, no response yet...\n");
                #endif
              }
            }
          break;
        } 
        case ADDRESS_RESPONSE:
        {
          bool isThisMine = true;
          for ( int i = 0; i < 6; i++ ) {
            if ( receivedData.payload[i] != myPhysicalAddr[i] )
              isThisMine = false;
          }
          if ( isThisMine ) {
            #ifdef debug
            printf("Connecting to the network...\nYour dynamic address is %d.\n", receivedData.payload[6]);
            #endif
            myDynamicAddr = receivedData.payload[6];
          }        
          break;
        }
        default:
          #ifdef debug
          printf("Unespecified operation code received.\n");
          #endif
          break;
      }
    }
    else{
      #ifdef debug
      printf("Received a ID already received, ID: %d.\n",receivedData.id);
      #endif
    }
  }
}

void Transceiver::joinNetwork()
{
  radio->stopListening();
  radio->openWritingPipe(pipe);

  setPackage(NEW_ELEMENT_ADDR,GATEWAY_ADDR,ADDRESS_REQUEST,myPackageId++,0,myPhysicalAddr,6);

  #ifdef debug
  printf("Requesting to join the network...\n");
  #endif
  if( !sendData()){
    #ifdef debug
    printf("Trying to reach the network, no response yet...\n");
    #endif
  }
  
  radio->openReadingPipe(1,pipe);
  radio->startListening();
}

