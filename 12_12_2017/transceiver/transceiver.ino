#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "transceiver_stuff.h"

//Global Variables ----------------------------------------------

//---------------------------------------------------------------

#define debug

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
volatile unsigned int count;
byte payload[4];
/******************** SETUP ***********************/
void setup()
{
  Serial.begin(9600);
  fdevopen( &serial_putc, 0 ); // for printf
  t.transceiverBegin();
  startTimer();
  count=0;
  sei();
    
  printf("\nTransceiver\n\n");

  printf("Joining Network\n");
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
  count++;
  for(int i=0;i<4;i++)
    payload[i]=random(0,100);
  if(count>=5 && t.getDynamicAddr() != NEW_ELEMENT_ADDR){
    count=0;
    t.setPackage(t.getDynamicAddr(),GATEWAY_ADDR,PAYLOAD_SEND,t.getPackageID(),0,payload,4);
    #ifdef debug
    printf("ENVIANDO: ORIGEM: %X DESTINO: %X HOP: %d OPCODE: %X ID: %X PAYLOAD:",t.getDynamicAddr()
      ,GATEWAY_ADDR
      , 0
      , PAYLOAD_SEND
      , t.getPackageID());
    for(int i=0;i<4;i++)
      printf("%d",payload[i]);
    printf("\n");
    #endif
    if( !t.sendData()){
      while ( !t.retransmitData() ) {
        delay(1000);
        #ifdef debug
        printf("No response from network...\n");
        #endif
      }
    }          
  }
}




