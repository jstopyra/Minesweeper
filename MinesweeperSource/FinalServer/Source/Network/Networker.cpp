#include "Networker.h"

#include "Data\Database.h"
#include "Logic\Lobby.h"
#define DEFAULT_BUFLEN 4096


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
				printf("new client connected\n\n");
				Client* pNewClient = new Client();
				pNewClient->sockfd = connfd;
				pNewClient->pPlayer = nullptr;
				m_listConnections[connfd] = pNewClient;
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
				if ((int)(n = recv(sockfd, buff, sizeof(buff), 0)) < 0)
				{
					if (errno == ECONNRESET)
					{
						printf("Connection reset by the client\n\n");
						ClosePlayerConnection(sockfd);
						closesocket(sockfd);
						client[i].fd = UNSIGNED_NEG_1;
					}
					else
					{
						printf("Connection lost with client\n\n");
						ClosePlayerConnection(sockfd);
						closesocket(sockfd);
						client[i].fd = UNSIGNED_NEG_1;
					}
					break;
				}
				else if (n == 0)
				{
					printf("Connection closed by the client\n\n");
					ClosePlayerConnection(sockfd);
					closesocket(sockfd);
					client[i].fd = UNSIGNED_NEG_1;
				}
				else
				{
					//deal with the message
					Proto::ClientRequest clientRequest;
					clientRequest.ParseFromArray(buff, sizeof(buff));
					//printf("Message: %s", clientRequest.DebugString().c_str());
					ProcessRequest(clientRequest);
				}
				if (--nready <= 0)
				{
					break; //no more readable discriptors
				}
			}
		}
	}
}

void Networker::ProcessRequest(Proto::ClientRequest clientRequest)
{
	if (clientRequest.has_loginrequest())
	{
		Proto::LoginRequest loginRequest = clientRequest.loginrequest();
		ProcessLoginRequest(loginRequest);
	}
	if (clientRequest.has_createroom())
	{
		Proto::CreateRoomRequest createRequest = clientRequest.createroom();
		ProcessCreateRoomRequest(createRequest);

	}
	if (clientRequest.has_joinroom())
	{
		Proto::JoinRoomRequest joinRequest = clientRequest.joinroom();
		ProcessJoinRoomRequest(joinRequest);

	}
	if (clientRequest.has_quitgame())
	{
		Proto::QuitGameRequest quitRequest = clientRequest.quitgame();
		ProcessQuitGameRequest(quitRequest);
	}
	if (clientRequest.has_tileclicked())
	{
		Proto::TileClickedRequest tileRequest = clientRequest.tileclicked();
		ProcessTileClickedRequest(tileRequest);
	}
	if (clientRequest.has_quitroom())
	{
		Proto::QuitRoom quitRoom = clientRequest.quitroom();
		ProcessQuitRoomRequest(quitRoom);
	}
}

void Networker::ProcessQuitRoomRequest(Proto::QuitRoom quitRoom)
{
	m_pLobby->RemovePlayerFromRooms(quitRoom.token());
}

void Networker::ProcessLoginRequest(Proto::LoginRequest loginRequest)
{
	Proto::ServerResponse* serverResponse = new Proto::ServerResponse();
	Proto::LoginResponse* loginResponse = new Proto::LoginResponse();
	if (m_database.CheckUserPassword(loginRequest.login(), loginRequest.password()))
	{
		printf("User: %s provided correct password.\n", loginRequest.login().c_str());
		//0 means person is not logged in.
		if (!m_database.CheckUserLoggedIn(loginRequest.login()))
		{
			//serverResponse->set_gamestate(Proto::GameState::LOBBY);

			int token = m_database.GetUserToken(loginRequest.login().c_str());
			if (token == 0)
				token = GetNewUserToken();

			loginResponse->set_token(token);
			printf("User: %s was not yet logged in.\n", loginRequest.login().c_str());
			User* addedUser = m_database.LogUser(connfd, loginRequest.login(), token);
			addedUser->connfd = sockfd;
			serverResponse->set_allocated_loginresponse(loginResponse);
			Send(*serverResponse, sockfd);

			m_pLobby->AddUserToLobby(addedUser);
			m_listConnections[connfd]->pPlayer = addedUser;
		}
		//anything else than 0 means person is logged in.
		else
		{
			printf("User: %s was already logged in.\n", loginRequest.login().c_str());
			loginResponse->set_token(0);
			serverResponse->set_allocated_loginresponse(loginResponse);
			Send(*serverResponse, sockfd);
		}
	}
	else
	{
		loginResponse->set_token(-1);
		serverResponse->set_allocated_loginresponse(loginResponse);
		Send(*serverResponse, sockfd);
		printf("User: %s provided incorrect password.\n", loginRequest.login().c_str());
	}
	printf("\n");
}

//Check if there is max num of rooms.
//Spawn a new room.
//Send message to all clients that are in the lobby about the lobby change.
void Networker::ProcessCreateRoomRequest(Proto::CreateRoomRequest createRequest)
{
	m_pLobby->CreateRoom(&createRequest);
}

void Networker::ProcessJoinRoomRequest(Proto::JoinRoomRequest joinRequest)
{
	m_pLobby->MovePlayerToRoom(&joinRequest);
}

void Networker::ProcessQuitGameRequest(Proto::QuitGameRequest quitRequest)
{

}

void Networker::ProcessTileClickedRequest(Proto::TileClickedRequest tileRequest)
{
	m_pLobby->UpdateRoomsMaps(&tileRequest);
}


void Networker::ClosePlayerConnection(int connfd)
{
	printf("Checking for user connection: %d.\n", connfd);
	auto it = m_listConnections.find(connfd);
	if (it != m_listConnections.end())
	{
		//connection found
		printf("user connection: %d was found.\n", connfd);
		//Check if the player is in the database
		//m_database.CheckUserLoggedIn(connfd);
		Client* pClient = m_listConnections[connfd];
		if (pClient->pPlayer)
		{
			m_pLobby->RemovePlayer(pClient->pPlayer->token);
			m_pLobby->RemovePlayerFromRooms(pClient->pPlayer->token);
		}
		m_database.KickUser(connfd);
		m_listConnections.erase(connfd);
		//m_listConnections
	}
}

int Networker::GetNewUserToken()
{

	int randToken = 0;
	bool keepLooking = true;
	while (std::find(usedTokens.begin(), usedTokens.end(), randToken) != usedTokens.end() || keepLooking)
	{
		randToken = rand() % 1000;
		keepLooking = false;
	}
	usedTokens.push_back(randToken);
	return randToken;
}

void Networker::Send(Proto::ServerResponse& response, SOCKET socket)
{
	if (socket == 0)
		for (auto client : m_clientsToNotify)
		{
			send(client, response.SerializeAsString().c_str(), DEFAULT_BUFLEN, 0);
		}
	else
	{
		send(socket, response.SerializeAsString().c_str(), DEFAULT_BUFLEN, 0);
	}
}

Networker::Networker()
{
	m_pLobby = new Lobby(this);
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
	delete m_pLobby;
	closesocket(listenfd);
	WSACleanup();
}