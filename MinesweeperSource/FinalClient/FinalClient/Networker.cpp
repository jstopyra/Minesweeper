#include "Networker.h"
#include <iostream>
#include "SDL.h"
#include "Utilities\EventSystem\EventSystem.h"
#include "Utilities\EventSystem\Events\ServerResponseEvent.h"
#include "Application.h"
#include "GameScenes\GameScene.h"

extern EventSystem* g_pEventSystem;

Networker::Networker()
{
//	Init();
}

int Networker::Init()
{
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ConnectSocket = socket(AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP);
	if (ConnectSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	ZeroMemory(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(9999);
	
	return 0;
}

void Networker::Destroy()
{

}

void Networker::SendMessage(Proto::ClientRequest& clientRequest)
{
	SDL_Log("Sending a message times: %d \n", numSent);
	++numSent;
	send(ConnectSocket, clientRequest.SerializeAsString().c_str(), sizeof(Proto::ClientRequest), 0);
}

Networker::~Networker()
{
	// cleanup
	WSACleanup();

}

void Networker::ConnectToServer(char* serverIp)
{
	Init();
	iResult = inet_pton(AF_INET, serverIp, &servaddr.sin_addr);
	if (iResult <= 0)
	{
		if (iResult == 0)
			printf("pAddrBuf parameter points to a string that is not a valid IPv4 dotted-decimal string or a valid IPv6 address string.");
		else
			printf("InetPton failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return;
	}
	iResult = connect(ConnectSocket, (sockaddr *)&servaddr, sizeof(servaddr));
	if (iResult == SOCKET_ERROR)
	{
		printf("connect failed: %ld\n", WSAGetLastError());
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}
	else
	{
		printf("connected to the server: %d.\n", (int)ConnectSocket);
		char ipAdress[16];
		inet_ntop(servaddr.sin_family, &servaddr.sin_addr, ipAdress, sizeof(ipAdress));
		printf("%s\n", ipAdress);
		if (m_pListenThread)
		{

		}
		std::thread workerThread(&Networker::ProcessClient, this, (LPVOID)ConnectSocket);
		m_pListenThread = &workerThread;
		workerThread.detach();
		Application::GetApplication()->LoadScene(k_loginScene);
	}
}

DWORD WINAPI Networker::ProcessClient(LPVOID lpParameter)
{
	char buff[DEFAULT_BUFLEN];
	char recvbuf[DEFAULT_BUFLEN];
	size_t iResult;

	for (;;)
	{
		//	std::getline(std::cin, buff);
		//	std::cin.read(buff, sizeof(buff));
		ZeroMemory(&buff, sizeof(buff));

		//Assignment #1
		ZeroMemory(&recvbuf, sizeof(recvbuf));

		do {
			iResult = (int)recv((int)lpParameter, recvbuf, sizeof(recvbuf), 0);
			if (iResult > 0)
			{
				//while (Application::GetApplication()->IsRendering()) std::this_thread::yield();
				Proto::ServerResponse* serverResponse = new Proto::ServerResponse();
				serverResponse->ParseFromArray(recvbuf, sizeof(recvbuf));
				//printf("Message: %s", clientRequest.DebugString().c_str());
				std::unique_lock<std::mutex> lk(Application::GetApplication()->m_mutex);
				Application::GetApplication()->m_condition.wait(lk, [] {return Application::GetApplication()->IsNotRendering(); });
				
				Application::s_pServerResponse.push(serverResponse);
				//g_pEventSystem->TriggerEvent(new ServerResponseEvent(serverResponse));
				
				lk.unlock();
				Application::GetApplication()->m_condition.notify_all();
				//Receive data from the server
				SDL_Log("receive data from the server");
			}
		} while (iResult > 0);
		if (strcmp(recvbuf, "quit") == 0)
		{
			closesocket((int)lpParameter);
			printf("Closed the connection with the server\n\n\n\n");
			break;
		}
		closesocket((int)lpParameter);
		break;

	}
	while (Application::GetApplication()->IsRendering()) std::this_thread::yield();
	Application::GetApplication()->LoadScene(k_loadingScene);

	WSACleanup();
	//std::terminate();

	return NULL;
}