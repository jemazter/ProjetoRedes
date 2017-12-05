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

void startTimer(){
  TCCR1A = 0; 
  TCCR1B = 0;
  OCR1A = 15624;
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS10);
  TCCR1B |= (1 << CS12);
  TIMSK1 |= (1 << OCIE1A);
}
/******************** Variables ***********************/
Transceiver t;
volatile unsigned int refreshCount;
/******************** SETUP ***********************/
void setup()
{
  Serial.begin(9600);
  fdevopen( &serial_putc, 0 ); // for printf
  t.transceiverBegin();
  startTimer();
  refreshCount=0;
  sei();
    
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

ISR(TIMER1_COMPA_vect)
{
  t.refreshRecentsPackage();
}




