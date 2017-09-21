/* 
Sending formatted data packet with nRF24L01. 
Maximum size of data struct is 32 bytes.
1 - GND
2 - VCC 3.3V !!! NOT 5V
3 - CE to Arduino pin 9
4 - CSN to Arduino pin 10
5 - SCK to Arduino pin 13
6 - MOSI to Arduino pin 11
7 - MISO to Arduino pin 12
8 - UNUSED
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "transceiver_stuff.h"

const uint64_t pipe = 0xE8E8F0F0E1LL;

RF24 radio(9, 10);

// For printf support
int serial_putc( char c, FILE * ) 
{
  Serial.write( c );
  return c;
}

/******************** SETUP ***********************/

void setup()
{
  Serial.begin(9600);
  fdevopen( &serial_putc, 0 ); // for printf

  printf("\nTransceiver\n\n");

  // Set up radio module
  radio.begin();
  //radio.setDataRate(RF24_2MBPS); // Both endpoints must have this set the same
  //radio.setAutoAck(false);       // Either endpoint can set to false to disable ACKs
  //radio.setPALevel(RF24_PA_MIN); // Power amplifier level. Also LOW,MED,HIGH (default is HIGH)
  radio.printDetails();

  while ( myDynamicAddr == NEW_ELEMENT_ADDR ) {
    joinNetwork();
    delay(100);
    recvData();
  }
    
  radio.openReadingPipe(1,pipe);
  radio.startListening();
}

/**************************************************/

/******************** LOOP ************************/

void loop()
{
  recvData();
  delay(20);
}

/**************************************************/

/***************** FUNCTIONS **********************/

bool sendData( byte origem, byte destino, byte hops, byte opcode, byte id, byte payload[] ) 
{
  radio.stopListening();
  radio.openWritingPipe(pipe);   

  bool   write_status;

  data.origem = origem;
  data.destino = destino;
  data.hop = hops;
  data.opcode = opcode;
  data.id = id;
  //data.checksum = checksum;
  for ( int i = 0; i < 24; i++ )
    data.payload[i] = payload[i];

  write_status = radio.write(&data, sizeof(MyData));

  radio.openReadingPipe(1,pipe);
  radio.startListening();

  return write_status;
}

void recvData()
{
  if ( radio.available() ) {

    radio.read(&data, sizeof(MyData));

    Opcode op = (Opcode) data.opcode;
    
    switch( op ) {
      case payload:
        if ( data.destino == myDynamicAddr ) {
          printf("RECEBENDO: ORIGEM: %X DESTINO: %X HOP: %d OPCODE: %X ID: %X CHKSUM: %X PAYLOAD: \"%s\"\n",data.origem, data.destino, data.hop, data.opcode, data.id, data.checksum, data.payload);
        }
        else {
          if ( data.hop < 253 ) {
            sendData( data.origem, data.destino, data.hop++, data.opcode, data.id, data.payload );
          }
        }
        break;
      case addrRequest:
        break;
      case addrAssign:
      {
        bool isThisMine = true;
        for ( int i = 0; i < 6; i++ ) {
          if ( data.payload[i] != MY_PHYSICAL_ADDR[i] )
            isThisMine = false;
        }
        if ( isThisMine ) {
          printf("Connecting to the network...\nYour dynamic address is %d.\n", data.payload[6]);
          myDynamicAddr = data.payload[6];
        }        
        break;
      }
      default:
        printf("Unespecified operation code received.\n");
        break;
    }
  }
}

void joinNetwork()
{
  radio.stopListening();
  radio.openWritingPipe(pipe);
  
  if ( sendData( myDynamicAddr, GATEWAY_ADDR, 0, addrRequest, myPackageId, MY_PHYSICAL_ADDR ) ) {
    printf("Requesting to join the network...\n");
    myPackageId++;
  }
  else {
    printf("Trying to reach the network, no response yet...\n");
  }
  
  radio.openReadingPipe(1,pipe);
  radio.startListening();
}


