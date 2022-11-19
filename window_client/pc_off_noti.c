#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

#define PACKET_SIZE 4096
#define RESPONSE_SIZE 4096

typedef struct sockaddr_in SOCK_ADDR;

int main(int argc, char *argv[])
{
	int s = -1;
	int code = 0;

#ifdef _WIN32
	WSADATA wsa;
#endif
	SOCK_ADDR server;

	char message[PACKET_SIZE] = {0};
	char response[RESPONSE_SIZE] = {0};

	int recv_size = 0;

	memset(&server, 0, sizeof(server));
	server.sin_addr.s_addr = inet_addr("54.180.0.192");
	server.sin_family = AF_INET;
	server.sin_port = htons(1129);

#ifdef _WIN32
	if ((code = WSAStartup(MAKEWORD(2, 2), &wsa)) != 0)
	{
		exit(code);
	}
#endif

	if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		exit(s);
	}

	if (connect(s, (struct sockaddr *)(&server), sizeof(server)) < 0)
	{
		exit(s);
	}

	sprintf(message, "[PC]CLIENT_SHUTTING_DOWN");
	if (send(s, message, strlen(message), 0) < 0)
	{
		printf("send failed\n");
		exit(s);
	}

	sprintf(message, "SET_PC_STATE_OFF");
	if (send(s, message, strlen(message), 0) < 0)
	{
		printf("send failed\n");
		exit(s);
	}

	printf("send success\n");
	// if((recv_size = recv(s, response, RESPONSE_SIZE, 0)) < 0) {
	//     exit(s);
	// }
	// printf("%s\n", response);

#ifdef _WIN32
	closesocket(s);
	WSACleanup();
#else
	close(s);
#endif
}
