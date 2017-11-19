#pragma once

#define WIN32_LEAN_AND_MEAN
#define DEFAULT_BUFLEN 4096
#define DEFAULT_PORT "9999"
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <thread>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#include "ProtoMessages.pb.h"


//A global class that sends and receives data from server.
class Networker
{
	//static Networker* m_pInstance;
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct sockaddr_in servaddr;
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;
	//std::thread workerThread;
	std::thread* m_pListenThread = nullptr;
	int numSent;
public:

	Networker();
	~Networker();
	Networker* GetInstance();
	void SendMessage(Proto::ClientRequest& clientRequest);
	void Destroy();
	void ConnectToServer(char* serverIp);



private:
	DWORD WINAPI ProcessClient(LPVOID lpParameter);

	int Init();

};
