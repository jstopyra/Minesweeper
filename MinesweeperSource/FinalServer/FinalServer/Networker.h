#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define IP_STRING_LEN 16
#define MAXBUFFER 512
#define INFTIM -1
#define UNSIGNED_1 -1
#define UNSIGNED_NEG_1 0xffffffff
#include "ProtoMessages.pb.h"

#define OPEN_MAX 256 //max amount of open connections.
struct ConnectionStruct
{
	struct sockaddr_in servaddr;
	int connfd;
};

class Networker
{
	int i, maxi, listenfd, connfd, sockfd;
	int nready;
	size_t n;

	char buffer[MAXBUFFER];
	socklen_t clilen;

	struct pollfd client[OPEN_MAX];
	struct sockaddr_in cliaddr, servaddr;

	struct ConnectionStruct *connectionStruct = (struct ConnectionStruct*) malloc(sizeof *connectionStruct);
	char buff[MAXBUFFER];

	SOCKET ConnectSocket = INVALID_SOCKET;
	// Initialize Winsock
	int iResult;
	WSADATA wsaData;

public:
	Networker();
	~Networker();

	void Update();
private:

};


void Networker::Update()
{
	for (i = 0; i < OPEN_MAX; ++i)
	{
		client[i] = pollfd();
	}
	//	ZeroMemory(&client, sizeof(client));

	listen(listenfd, 8);
	//homework week 5
	client[0].fd = listenfd;
	client[0].events = (unsigned)POLLRDNORM;
	for (i = 1; i < OPEN_MAX; ++i)
	{
		client[i].fd = UNSIGNED_NEG_1;
	}
	maxi = 0;

	for (;;)
	{
		nready = WSAPoll(client, maxi + 1, INFTIM);

		if ((int)client[0].revents & POLLRDNORM)//new client connection
		{
			clilen = sizeof(cliaddr);
			connfd = accept(listenfd, (sockaddr*)&cliaddr, &clilen);
			printf("New client tries connecting\n");


			for (i = 1; i < OPEN_MAX; ++i)
			{
				if ((int)client[i].fd < 0)
				{
					client[i].fd = (unsigned)connfd; //save discriptor
					printf("saving a potential connection\n");
					break;
				}
			}
			if (i == OPEN_MAX)
			{
				printf("Too many clients\n");
			}
			client[i].events = (unsigned)POLLRDNORM;
			if (i > maxi)
			{
				printf("increasing the max number of connections\n");
				maxi = i;
			}
			if (--nready <= 0)
			{
				printf("new client connected\n");
				continue;
			}
		}
		for (i = 0; i <= maxi; ++i)
		{
			if ((sockfd = (int)client[i].fd) < 0)
			{
				continue;
			}
			if ((int)client[i].revents &(POLLRDNORM | POLLERR))
			{
				if ((int)(n = recv(sockfd, buff, MAXBUFFER, 0)) < 0)
				{
					if (errno == ECONNRESET)
					{
						printf("Connection reset by the client\n");
						closesocket(sockfd);
						client[i].fd = UNSIGNED_NEG_1;
					}
					else
					{
						printf("Connection lost with client\n");
						closesocket(sockfd);
						client[i].fd = UNSIGNED_NEG_1;
					}
					break;
				}
				else if (n == 0)
				{
					printf("Connection closed by the client\n");
					closesocket(sockfd);
					client[i].fd = UNSIGNED_NEG_1;
				}
				else
				{
				//deal with the message
					Proto::ClientRequest clientRequest;
					clientRequest.ParseFromArray(buff, sizeof(Proto::ClientRequest));
					printf("Message: %s", clientRequest.DebugString().c_str());
				}
				if (--nready <= 0)
				{
					break; //no more readable discriptors
				}
			}
		}
	}
}

Networker::Networker()
{
	WSADATA wsaData;
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return;
	}


	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return;
	}

	listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenfd == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return;
	}

	ZeroMemory(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(9999);
	bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr));

}

Networker::~Networker()
{
	closesocket(listenfd);
	WSACleanup();
}