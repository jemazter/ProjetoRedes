#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include </home/pi/RF24/RF24.h>
#include "gateway_stuff.h"

#include<arpa/inet.h>
#include<sys/socket.h>

using namespace std;

#define SERVER "172.16.152.92"
#define BUFLEN 512  //Max length of buffer
#define PORT 10100   //The port on which to send data

// Hardware configuration - RPi generic:
RF24 radio(22,0);

// Setup for GPIO 15 CE and CE0 CSN with SPI Speed @ 8Mhz
//RF24 radio(RPI_V2_GPIO_P1_15, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ);

void die(string s)
{

    exit(1);
}

int main(int argc, char** argv){

    /******************** SETUP ***********************/
    Transceiver g;
    g.transceiverBegin();
    printf("\nGateway\n\n");

    MyData_t data;
    /**************************************************/

    /******************** NETWORK SETUP ***********************/
    	
	struct sockaddr_in si_other;
    int s, i, slen=sizeof(si_other);
    char buf[BUFLEN];
    char message[BUFLEN];
 
    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
 
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
     
    if (inet_aton(SERVER , &si_other.sin_addr) == 0) 
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    /**************************************************/



    /******************** LOOP ************************/
    while(1)
    {
      g.recvData();
      data = g.getRcvData();
	if (sendto(s, &data, sizeof(MyData_t) , 0 , (struct sockaddr *) &si_other, slen)==-1)
        {
            die("sendto()");
        }
      
      delay(20);
    }
    /**************************************************/
}
