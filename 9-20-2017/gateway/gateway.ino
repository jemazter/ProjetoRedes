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
#include "gateway_stuff.h"

const uint64_t pipe = 0xE8E8F0F0E1LL;

RF24 radio(9, 10);

// For printf support
int serial_putc( char c, FILE * ) 
{
  Serial.write( c );
  return c;
}

/**************************************************/

void setup()
{
  Serial.begin(9600);
  fdevopen( &serial_putc, 0 ); // for printf

  printf("Gateway\n");
 
  // Set up radio module
  radio.begin();
  //radio.setDataRate(RF24_2MBPS); // Both endpoints must have this set the same
  //radio.setAutoAck(false);       // Either endpoint can set to false to disable ACKs
  //radio.setPALevel(RF24_PA_MIN); // Power amplifier level. Also LOW,MED,HIGH (default is HIGH)
  radio.printDetails();
    
  radio.openReadingPipe(1,pipe);
  radio.startListening();
}

/**************************************************/

void sendData() 
{
  radio.stopListening();
  radio.openWritingPipe(pipe);   
  MyData  data;
  bool   write_status;

  data.origem = 0xFE;
  data.destino = 0x00;
  data.hop = 0xFF;
  data.opcode = 0x01;
  data.id = 0;
  data.checksum = 0;

  printf("Size of Data:%d\n",sizeof(data));
  write_status=radio.write(&data, sizeof(MyData));
  if(write_status)
    printf("ENVIANDO -> ORIGEM: %X DESTINO: %X HOP: %d OPCODE: %X ID: %X CHKSUM: %X\n",data.origem, data.destino, data.hop, data.opcode, data.id, data.checksum);
  else
    printf("FAILED\n");
  //radio.printDetails();
  radio.openReadingPipe(1,pipe);
  radio.startListening();
}

void recvData()
{
  if ( radio.available() ) {
    radio.read(&data, sizeof(MyData));

    Opcode op = (Opcode) data.opcode;
    
    switch( op ) {
      case payload:
        
        break;
      case addrRequest:
        if ( deviceCount < 253 ) {
          arpTable[deviceCount].dynamicAddr = deviceCount;
          
          /*for ( int i = 0; i < 6; i++ ) {
            arpTable[deviceCount].physicalAddr[i] = data.payload[i];
          }*/

          // Informa ao novo membro seu endereço dinâmico
          radio.stopListening();
          radio.openWritingPipe(pipe);
          data.origem = 0x00;
          data.destino = 0xFE;
          data.hop++;
          data.opcode = addrAssign;
          data.id++;
          data.checksum = 0;
          data.payload[6] = deviceCount;
          radio.write(&data, sizeof(MyData));
          radio.openReadingPipe(1,pipe);
          radio.startListening();

          printf("\"%02X:%02X:%02X:%02X:%02X:%02X\" has joined the network and got dynamic address \"%d\".\n", data.payload[0], data.payload[1], data.payload[2], data.payload[3], data.payload[4], data.payload[5], data.payload[6], data.payload[7]);
          
          deviceCount++;
        }
        else
          printf("Error: The network has reached its limit of devices.\n");
        break;
      case addrAssign:
        break;
      default:
        printf("Unespecified operation code received.\n");
        break;
    }
  
    printf("RECEBENDO: ORIGEM: %X DESTINO: %X HOP: %d OPCODE: %X ID: %X CHKSUM: %X\n",data.origem, data.destino, data.hop, data.opcode, data.id, data.checksum);
  }
}

/**************************************************/

void loop()
{
  recvData();
  delay(20);
}
