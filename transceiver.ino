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

// For printf support
int serial_putc( char c, FILE * ) 
{
  Serial.write( c );
  return c;
}

const uint64_t pipe = 0xE8E8F0F0E1LL;

RF24 radio(9, 10);

// The sizeof this struct should not exceed 32 bytes
struct MyData{
  byte origem;
  byte destino;
  byte hop;
  byte opcode;
  byte id;
  byte checksum;
  byte payload[24];
};

/**************************************************/

// Change this when uploading to each Arduino
boolean sender = true;
int count=0;

void setup()
{
  Serial.begin(9600);
  fdevopen( &serial_putc, 0 ); // for printf

  if(sender)
    printf("Transmissor\n");
  else
    printf("Receptor\n");

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
    MyData data;

    radio.read(&data, sizeof(MyData));
  
    printf("RECEBENDO: ORIGEM: %X DESTINO: %X HOP: %d OPCODE: %X ID: %X CHKSUM: %X\n",data.origem, data.destino, data.hop, data.opcode, data.id, data.checksum);
 
  }  
}

/**************************************************/

void loop()
{
  if(sender){
    sendData();
  }
  else
    recvData();
}
