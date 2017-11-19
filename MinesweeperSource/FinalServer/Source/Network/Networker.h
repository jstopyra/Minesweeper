#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define IP_STRING_LEN 16
#define MAXBUFFER 512
#define INFTIM -1
#define UNSIGNED_1 -1
#define UNSIGNED_NEG_1 0xffffffff
#include "ProtoMessages.pb.h"

#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include "Data\Database.h"
#include "Logic\Lobby.h"
#include <winsock2.h>
//#include <sys/socket.h>
#include <windows.h>
#include <winsock.h>
#include <ws2tcpip.h>
struct User;
class Lobby;

#define OPEN_MAX 256 //max amount of open connections.
struct ConnectionStruct
{
	struct sockaddr_in servaddr;
	int connfd;
};

struct Client
{
	int sockfd;
	User* pPlayer;
};

class Networker
{
	Database m_database;
	Lobby* m_pLobby;
	
	int i, maxi, listenfd, connfd, sockfd;
	std::vector<int> m_clientsToNotify;
	int nready;
	size_t n;

	std::unordered_map<int, Client*> m_listConnections;

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
	void Send(Proto::ServerResponse& response, SOCKET socket = 0);
private:
	void ProcessLoginRequest(Proto::LoginRequest loginRequest);
	void ProcessCreateRoomRequest(Proto::CreateRoomRequest createRequest);
	void ProcessJoinRoomRequest(Proto::JoinRoomRequest joinRequest);
	void ProcessQuitGameRequest(Proto::QuitGameRequest quitRequest);
	void ProcessTileClickedRequest(Proto::TileClickedRequest tileRequest);
	void ProcessQuitRoomRequest(Proto::QuitRoom quitRoom);

	int GetNewUserToken();

	void ClosePlayerConnection(int connfd);
	std::vector<int> usedTokens;
	void ProcessRequest(Proto::ClientRequest clientRequest);
};

