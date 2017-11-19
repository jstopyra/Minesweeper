#include "Lobby.h"
#include "Data\Database.h"
#include "Room.h"
#include "Network/Networker.h"

Lobby::Lobby(Networker* pNetworker)
	:m_maxRooms(5)
	,m_currRooms(0)
	,m_pNetworker(pNetworker)
{

}

Lobby::~Lobby()
{

}

User* Lobby::FindPlayerWithToken(int token)
{
	auto it = m_pPlayerMap.find(token);
	if (it != m_pPlayerMap.end())
	{
		return it->second;
	}
	//no player was found
	return nullptr;
}

void Lobby::RemovePlayer(int token)
{
	auto it = m_pPlayerMap.find(token);
	if (it != m_pPlayerMap.end())
	{
		m_pPlayerMap.erase(it);
	}
	Proto::ServerResponse* serverResponse = new Proto::ServerResponse();
	Proto::LobbyUpdate* pLobbyUpdate = new Proto::LobbyUpdate();
	//Proto::PlayerInfo* pPlayerInfo = new Proto::PlayerInfo();
	Proto::PlayerList playerList = pLobbyUpdate->playerlist();
	for (auto player : m_pPlayerMap)
	{
		printf("Player %s from lobby added to a server response.\n", player.second->name.c_str());
		Proto::PlayerInfo* pPlayerInfo = playerList.add_playerinfo();
		pPlayerInfo->set_name(player.second->name);
		pPlayerInfo->set_score(player.second->score);
	}
	pLobbyUpdate->set_allocated_playerlist(&playerList);
	serverResponse->set_allocated_lobbyupdate(pLobbyUpdate);
	for (auto player : m_pPlayerMap)
	{
		m_pNetworker->Send(*serverResponse, player.second->connfd);
	}
}

void Lobby::RemovePlayerFromRooms(int token)
{

	//Tell each room to remove the player from the room
	for (auto room : m_pRooms)
	{
		User* pUser = room->RemovePlayer(token);
		if (pUser)
		{
			AddUserToLobby(pUser);
		}
	}
}

void Lobby::UpdateLobby()
{
	Proto::ServerResponse* serverResponse = new Proto::ServerResponse();
	Proto::LobbyUpdate* pLobbyUpdate = new Proto::LobbyUpdate();
	//Proto::PlayerInfo* pPlayerInfo = new Proto::PlayerInfo();
	Proto::PlayerList playerList = pLobbyUpdate->playerlist();
	Proto::RoomList roomList = pLobbyUpdate->roomlist();
	roomList.set_numrooms(m_currRooms);

	for (auto player : m_pPlayerMap)
	{
		printf("Player %s from lobby added to a server response.\n", player.second->name.c_str());
		Proto::PlayerInfo* pPlayerInfo = playerList.add_playerinfo();
		pPlayerInfo->set_name(player.second->name);
		pPlayerInfo->set_score(player.second->score);
	}
	pLobbyUpdate->set_allocated_playerlist(&playerList);
	pLobbyUpdate->set_allocated_roomlist(&roomList);
	serverResponse->set_allocated_lobbyupdate(pLobbyUpdate);
	for (auto player : m_pPlayerMap)
	{
		m_pNetworker->Send(*serverResponse, player.second->connfd);
	}
}

void Lobby::AddUserToLobby(User* pUser)
{
	if (pUser)
	{
		m_pPlayerMap[pUser->token] = pUser;

		//update every client about a new player in the lobby
		Proto::ServerResponse* serverResponse = new Proto::ServerResponse();
		serverResponse->set_gamestate(Proto::GameState::LOBBY);
		m_pNetworker->Send(*serverResponse, pUser->connfd);

		serverResponse = new Proto::ServerResponse();
		Proto::LobbyUpdate* pLobbyUpdate = new Proto::LobbyUpdate();


		//Proto::PlayerInfo* pPlayerInfo = new Proto::PlayerInfo();
		Proto::PlayerList playerList = pLobbyUpdate->playerlist();

		Proto::RoomList roomList = pLobbyUpdate->roomlist();
		roomList.set_numrooms(m_currRooms);

		for (auto player : m_pPlayerMap)
		{
			printf("Player %s from lobby added to a server response.\n", player.second->name.c_str());
			Proto::PlayerInfo* pPlayerInfo = playerList.add_playerinfo();
			pPlayerInfo->set_name(player.second->name);
			pPlayerInfo->set_score(player.second->score);
		}
		pLobbyUpdate->set_allocated_playerlist(&playerList);
		pLobbyUpdate->set_allocated_roomlist(&roomList);
		serverResponse->set_allocated_lobbyupdate(pLobbyUpdate);
		for (auto player : m_pPlayerMap)
		{
			m_pNetworker->Send(*serverResponse, player.second->connfd);
		}
	}
}

void Lobby::UpdateRoomsMaps(Proto::TileClickedRequest* pClick)
{
	auto room = m_pRooms.begin();
	while (room != m_pRooms.end())
	{
		Room* pRoom = (*room);
		if(pRoom)
			if (pRoom->OnPlayerOpenTile(pClick->tileindex(), pClick->token()))
			{
				return;
			}
		++room;
	}
}

void Lobby::MovePlayerToRoom(Proto::JoinRoomRequest* joinRequest)
{
	int roomNum = joinRequest->roomnumber();
	if (m_pRooms.size() >= roomNum)
	{
		User* pUser = m_pPlayerMap[joinRequest->token()];
		if (pUser)
		{
			m_pRooms[roomNum]->AddPlayer(pUser);
			RemovePlayer(joinRequest->token());
		}
	}
}
void Lobby::DestroyRoom(Room* pRoom)
{
	auto room = m_pRooms.begin();
	while (room != m_pRooms.end())
	{
		if (*room == pRoom)
		{
			pRoom->EmptyRoom();
			m_pRooms.erase(room);
			--m_currRooms;
			UpdateLobby();
			break;
		}
		++room;
	}
}

void Lobby::CreateRoom(Proto::CreateRoomRequest* createRequest)
{
	if (m_currRooms < m_maxRooms)
	{
		//create a new room
		m_pRooms.push_back(new Room(m_pNetworker, this));
		++m_currRooms;
		printf("Creating a new room. New number of rooms: %d.\n", m_currRooms);

		Proto::ServerResponse* serverResponse = new Proto::ServerResponse();
		Proto::LobbyUpdate* pLobbyUpdate = new Proto::LobbyUpdate();
		//Proto::PlayerInfo* pPlayerInfo = new Proto::PlayerInfo();
		Proto::PlayerList playerList = pLobbyUpdate->playerlist();
		Proto::RoomList roomList = pLobbyUpdate->roomlist();
		roomList.set_numrooms(m_currRooms);

		for (auto player : m_pPlayerMap)
		{
			printf("Player %s from lobby added to a server response.\n", player.second->name.c_str());
			Proto::PlayerInfo* pPlayerInfo = playerList.add_playerinfo();
			pPlayerInfo->set_name(player.second->name);
			pPlayerInfo->set_score(player.second->score);
		}
		pLobbyUpdate->set_allocated_playerlist(&playerList);
		pLobbyUpdate->set_allocated_roomlist(&roomList);
		serverResponse->set_allocated_lobbyupdate(pLobbyUpdate);
		for (auto player : m_pPlayerMap)
		{
			m_pNetworker->Send(*serverResponse, player.second->connfd);
		}
	}
}