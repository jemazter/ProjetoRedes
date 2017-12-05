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

// For printf support
int serial_putc( char c, FILE * ) 
{
  Serial.write( c );
  return c;
}

/******************** SETUP ***********************/
Transceiver g;
void setup()
{
  Serial.begin(9600);
  fdevopen( &serial_putc, 0 ); // for printf
  g.transceiverBegin();
  printf("\nGateway\n\n");
}

/**************************************************/

/******************** LOOP ************************/

void loop()
{
  g.recvData();
  delay(20);
}

/**************************************************/
