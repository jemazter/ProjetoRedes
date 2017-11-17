#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "transceiver_stuff.h"

//Global Variables ----------------------------------------------

//---------------------------------------------------------------


// For printf support
int serial_putc( char c, FILE * ) 
{
  Serial.write( c );
  return c;
}

/******************** SETUP ***********************/
Transceiver t;
void setup()
{
  Serial.begin(9600);
  fdevopen( &serial_putc, 0 ); // for printf
  t.transceiverBegin();
  printf("\nTransceiver\n\n");
  
  while ( t.getDynamicAddr() == NEW_ELEMENT_ADDR ) {
    t.joinNetwork();
    delay(100);
    t.recvData();
  }

}

void loop()
{
  t.recvData();
  delay(20);
}




