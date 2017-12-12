#include <winsock.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define LOCAL_SERVER_PORT 10100
#define SIZE_OF_PAYLOAD 24

struct MyData {
  byte origem;
  byte destino;
  byte hop;
  byte opcode;
  byte id;
  byte checksum;
  byte payload[SIZE_OF_PAYLOAD];
}MyData;

int main(int argc, char *argv[])
{
	WSADATA wsInformacao;
	SOCKET socketServidor;
	struct sockaddr_in service, cliente;
	int iResult,tamamnhoEndereco,bytes,i;
	char bufferEntrada[sizeof(MyData)],bufferSaida[sizeof(MyData)];
	struct MyData data;

	// inicializa Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsInformacao);
	if (iResult != NO_ERROR) {
		printf("Erro na chamada da funcao WSAStartup().\n");
	}

	socketServidor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (socketServidor == INVALID_SOCKET) {
		printf("Erro na chamada da funcao socket(): %ld.\n", WSAGetLastError());
		WSACleanup();
		return 0;
	}


	// asocia a uma porta o servidor

	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr("172.16.152.98");
	service.sin_port = htons(10100);

	if (bind(socketServidor, (SOCKADDR *) &service, sizeof(service)) == SOCKET_ERROR ) {
		printf("Erro na chamada da funcao bind().\n");
		closesocket(socketServidor);
	    return 0;
	}


	tamamnhoEndereco = sizeof(cliente);

	while (1) {
        printf("Esperando um pacote.\n");
		bytes = recvfrom(socketServidor, bufferEntrada, sizeof(MyData), 0, (struct sockaddr *) &cliente, &tamamnhoEndereco);
		printf("Pacote recebido. \n");
		printf("Recebido Pacote de %d com HOP:%d ID:%d PAYLOAD:", data.origem,data.hop,data.id);
		for(i=0;i<4;i++)
			printf("%d",data.payload[i]);
		printf("\n");

		//sprintf(bufferSaida, "Quem esta %s", bufferEntrada);
		//sendto(socketServidor, bufferSaida, strlen(bufferSaida), 0, (struct sockaddr *) &cliente, tamamnhoEndereco);
	}

	return 0;
}
