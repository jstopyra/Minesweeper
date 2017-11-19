#pragma once

#include <unordered_map>
struct User;
class Networker;
class Lobby;

struct Tile
{
	enum TileTypes:int
	{
		k_0 = 0
		, k_1 = 1
		, k_2 = 2
		, k_3 = 3
		, k_4 = 4
		, k_5 = 5
		, k_6 = 6
		, k_7 = 7
		, k_8 = 8
		, k_bomb = 9
		, k_numTypes
	};

	TileTypes m_tileType;
	bool m_isOpen;
};

struct RoomMap
{
	
	int m_width = 10, m_height = 10, m_numBombs = 25;
	int m_numOpenNonBombTiles = 0;
	Tile** m_pTiles;

	void InitializeMap();
	bool IsBombAtIndex(int index);
};

class Room
{
	std::unordered_map<int, User*> m_pPlayerMap;
	Networker* m_pNetworker;
	RoomMap m_map;
	Lobby* m_pLobby;

public:
	Room(Networker* pNetworker, Lobby* pLobby);
	~Room();
	void AddPlayer(User* pUser);
	User* RemovePlayer(int token);
	bool OnPlayerOpenTile(int tileIndex, int token);
	void EmptyRoom();

private:

};
