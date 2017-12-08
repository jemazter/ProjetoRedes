#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include </home/pi/RF24/RF24.h>
#include "gateway_stuff.h"

using namespace std;

// Hardware configuration - RPi generic:
RF24 radio(22,0);

// Setup for GPIO 15 CE and CE0 CSN with SPI Speed @ 8Mhz
//RF24 radio(RPI_V2_GPIO_P1_15, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ);


int main(int argc, char** argv){

    /******************** SETUP ***********************/
    Transceiver g;
    g.transceiverBegin();
    printf("\nGateway\n\n");
    /**************************************************/

    /******************** LOOP ************************/
    while(1)
    {
      g.recvData();
      delay(20);
    }
    /**************************************************/
}
