#pragma once

#include <unordered_map>
#include <string>
struct User;

class Room;

class Networker;
namespace Proto 
{
	class CreateRoomRequest;
	class JoinRoomRequest;
	class TileClickedRequest;
}

class Lobby
{
	std::unordered_map<int, User*> m_pPlayerMap;
	std::vector<Room*> m_pRooms;

	int m_maxRooms;
	int m_currRooms;

	Networker* m_pNetworker;

public:

	User* FindPlayerWithToken(int token);
	void AddUserToLobby(User* pUser);
	Lobby(Networker* pNetworker);

	~Lobby();

	void RemovePlayer(int token);

	void CreateRoom(Proto::CreateRoomRequest* createRequest);

	void MovePlayerToRoom(Proto::JoinRoomRequest* joinRequest);
	void UpdateRoomsMaps(Proto::TileClickedRequest* pClick);

	void DestroyRoom(Room* pRoom);

	void RemovePlayerFromRooms(int token);

private:
	void UpdateLobby();
};
