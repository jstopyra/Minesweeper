#include "Room.h"
#include <ProtoMessages.pb.h>
#include "Data\Database.h"
#include "Network/Networker.h"
#include "Lobby.h"


Room::Room(Networker* pNetworker, Lobby* pLobby)
	:m_pNetworker(pNetworker)
	,m_pLobby(pLobby)
{
	m_map.InitializeMap();
}

Room::~Room()
{
}

void Room::AddPlayer(User * pUser)
{
	if (!pUser)
	{
		return;
	}
	m_pPlayerMap[pUser->token] = pUser;

	//Send message to the networker to move the player over to the room
	//update every client about a new player in the lobby
	Proto::ServerResponse* serverResponse = new Proto::ServerResponse();
	serverResponse->set_gamestate(Proto::GameState::GAME);
	m_pNetworker->Send(*serverResponse, pUser->connfd);

	serverResponse = new Proto::ServerResponse();

	Proto::RoomUpdate* pRoomUpdate = new Proto::RoomUpdate();
	Proto::PlayerList playerList = pRoomUpdate->playerlist();
	for (auto player : m_pPlayerMap)
	{
		printf("Player %s from lobby added to a server response.\n", player.second->name.c_str());
		Proto::PlayerInfo* pPlayerInfo = playerList.add_playerinfo();
		pPlayerInfo->set_name(player.second->name);
		pPlayerInfo->set_score(player.second->score);
	}
	
	pRoomUpdate->set_allocated_playerlist(&playerList);

	//Set the map for proto
	Proto::Map pMap;// = pRoomUpdate->mapupdate();
	//pMap.set_numexposedsquares(20);
	pMap.set_mapheight(m_map.m_height);
	pMap.set_mapwidth(m_map.m_width);
	for (int i = 0; i < m_map.m_width*m_map.m_height; ++i)
	{
		if (m_map.m_pTiles[i])
		{
			Proto::Square* pSquare = pMap.add_squares();
			if (m_map.m_pTiles[i]->m_isOpen)
				pSquare->set_squaretype((int)m_map.m_pTiles[i]->m_tileType);
			else
			{
				pSquare->set_squaretype(-1);
			}
		}
	}
	pRoomUpdate->set_allocated_mapupdate(&pMap);

	serverResponse->set_allocated_roomupdate(pRoomUpdate);

	for (auto player : m_pPlayerMap)
	{
		m_pNetworker->Send(*serverResponse, player.second->connfd);
	}
}

User* Room::RemovePlayer(int token)
{
	User* pUser = nullptr;
	auto it = m_pPlayerMap.find(token);
	if (it != m_pPlayerMap.end())
	{
		pUser = it->second;
		m_pPlayerMap.erase(it);
	}
	Proto::ServerResponse* serverResponse = new Proto::ServerResponse();
	Proto::RoomUpdate* pRoomUpdate = new Proto::RoomUpdate();
	//Proto::PlayerInfo* pPlayerInfo = new Proto::PlayerInfo();
	Proto::PlayerList playerList = pRoomUpdate->playerlist();
	for (auto player : m_pPlayerMap)
	{
//		printf("Player %s from room added to a server response.\n", player.second->name.c_str());
		Proto::PlayerInfo* pPlayerInfo = playerList.add_playerinfo();
		pPlayerInfo->set_name(player.second->name);
		pPlayerInfo->set_score(player.second->score);
	}
	pRoomUpdate->set_allocated_playerlist(&playerList);
	serverResponse->set_allocated_roomupdate(pRoomUpdate);
	for (auto player : m_pPlayerMap)
	{
		m_pNetworker->Send(*serverResponse, player.second->connfd);
	}
	return pUser;
}

void Room::EmptyRoom()
{
	User* pUser = nullptr;
	auto it = m_pPlayerMap.begin();
	if (it != m_pPlayerMap.end())
	{
		pUser = it->second;
		m_pLobby->AddUserToLobby(pUser);
		m_pPlayerMap.erase(it);
	}
}

bool Room::OnPlayerOpenTile(int tileIndex, int token)
{
	auto it = m_pPlayerMap.find(token);
	if (it == m_pPlayerMap.end())
	{
		return false;
	}
	User* pUser = it->second;

	if (tileIndex >= 0 && tileIndex < m_map.m_width*m_map.m_height)
	{
		//Open the new tile
		if (!m_map.m_pTiles[tileIndex]->m_isOpen)
		{
			m_map.m_pTiles[tileIndex]->m_isOpen = true;
			if (m_map.m_pTiles[tileIndex]->m_tileType != Tile::TileTypes::k_bomb)
			{
				++pUser->score;
				++m_map.m_numOpenNonBombTiles;
				if (m_map.m_numOpenNonBombTiles >= (m_map.m_width*m_map.m_height) - m_map.m_numBombs)
				{
					m_pLobby->DestroyRoom(this);
				}
			}
			else
			{
				pUser->score -= 10;
			}
			//notify the players about the new updated map;
			Proto::ServerResponse* serverResponse = new Proto::ServerResponse();
			Proto::RoomUpdate* pRoomUpdate = new Proto::RoomUpdate();
			//Proto::PlayerInfo* pPlayerInfo = new Proto::PlayerInfo();
			Proto::PlayerList playerList = pRoomUpdate->playerlist();
			for (auto player : m_pPlayerMap)
			{
				//		printf("Player %s from room added to a server response.\n", player.second->name.c_str());
				Proto::PlayerInfo* pPlayerInfo = playerList.add_playerinfo();
				pPlayerInfo->set_name(player.second->name);
				pPlayerInfo->set_score(player.second->score);
			}
			pRoomUpdate->set_allocated_playerlist(&playerList);

			//Set the map for proto
			Proto::Map* pMap = new Proto::Map();
			for (int i = 0; i < m_map.m_width*m_map.m_height; ++i)
			{
				if (m_map.m_pTiles[i])
				{
					Proto::Square* pSquare = pMap->add_squares();
					if (m_map.m_pTiles[i]->m_isOpen)
						pSquare->set_squaretype((int)m_map.m_pTiles[i]->m_tileType);
					else
					{
						pSquare->set_squaretype(-1);
					}
				}
			}
			pMap->set_mapheight(m_map.m_height);
			pMap->set_mapwidth(m_map.m_width);
			pRoomUpdate->set_allocated_mapupdate(pMap);
			serverResponse->set_allocated_roomupdate(pRoomUpdate);

			for (auto player : m_pPlayerMap)
			{
				m_pNetworker->Send(*serverResponse, player.second->connfd);
			}
		}
	}
	return true;
}

void RoomMap::InitializeMap()
{
	m_pTiles = new Tile*[m_width*m_height];
	for (int i = 0; i < m_width*m_height; ++i)
	{
		m_pTiles[i] = nullptr;
	}
	for (int i = 0; i < m_numBombs; ++i)
	{
		int bombI = rand() % (m_width*m_height);
		if (m_pTiles[bombI])
		{
			--i;
		}
		else
		{
			m_pTiles[bombI] = new Tile();
			m_pTiles[bombI]->m_tileType = Tile::TileTypes::k_bomb;
		}
	}
	for (int x = 0; x < m_width; ++x)
	{
		for (int y = 0; y < m_height; ++y)
		{
			int bombCount = 0;
			//Check each neighboor and check if its a bomb;
			if (m_pTiles[x + y*m_width])
				//continue if the tile already exists;
				continue;
			//Check if we can check to the left
			if (x > 0)
			{
				if(y > 0)
					if (IsBombAtIndex((x - 1) + ((y - 1)*m_width)))
						++bombCount;
				if(IsBombAtIndex((x - 1) + ((y)*m_width)))
					++bombCount;
				if(y < m_height-1)
					if (IsBombAtIndex((x - 1) + ((y+1)*m_width)))
						++bombCount;
			}
			//Check if we can check to the Right
			if (x < m_width-1)
			{
				if (y > 0)
					if (IsBombAtIndex((x + 1) + ((y - 1)*m_width)))
						++bombCount;
				if (IsBombAtIndex((x + 1) + ((y)*m_width)))
					++bombCount;
				if (y < m_height - 1)
					if (IsBombAtIndex((x + 1) + ((y + 1)*m_width)))
						++bombCount;
			}
			//top
			if(y > 0)
				if (IsBombAtIndex((x) + ((y - 1)*m_width)))
					++bombCount;
			//bottomm
			if (y < m_height-1)
				if (IsBombAtIndex((x)+((y + 1)*m_width)))
					++bombCount;
			//create a tile and assign how many bombs there are around it;
			m_pTiles[x + m_width*y] = new Tile();
			m_pTiles[x + m_width*y]->m_tileType = (Tile::TileTypes)bombCount;
		}
	}
//	for (int i = 0; i < m_width*m_height; ++i)
	//{
	//	//Check each neighboor and check if its a bomb;
	//	if (m_pTiles[i])
	//		//continue if the tile already exists;
	//		continue;
	//	
	//	if(i%m_width != 0 || i/m_width != 0)
	//	if (IsBombAtIndex(i - m_width - 1))
	//	{
	//		++bombCount;
	//	}
	//	if (IsBombAtIndex(i - m_width))
	//	{
	//		++bombCount;
	//	}
	//	if (i%m_width != 0 || i != m_width-1)
	//	if (IsBombAtIndex(i - m_width +1))
	//	{
	//		++bombCount;
	//	}
	//	if (IsBombAtIndex(i - 1))
	//	{
	//		++bombCount;
	//	}
	//	if(i%m_width)
	//	if (IsBombAtIndex(i +1))
	//	{
	//		++bombCount;
	//	}
	//	if (i%m_width != 0 || i / m_width != 0)
	//	if (IsBombAtIndex(i + m_width - 1))
	//	{
	//		++bombCount;
	//	}
	//	if (IsBombAtIndex(i + m_width))
	//	{
	//		++bombCount;
	//	}
	//	if (i%m_width != 0 || i != m_width - 1)
	//	if (IsBombAtIndex(i + m_width + 1))
	//	{
	//		++bombCount;
	//	}
	//}
}

bool RoomMap::IsBombAtIndex(int index)
{
	if (index < 0 || index > m_width*m_height - 1)
		//if the index is out of map bounds then it cannot be a bomb
		return false;
	if (m_pTiles[index])
		return (m_pTiles[index]->m_tileType == Tile::TileTypes::k_bomb);
	//if the tile is not valid then its not a bomb;
	return false;
}
