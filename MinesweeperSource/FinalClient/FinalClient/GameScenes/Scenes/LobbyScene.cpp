#include "LobbyScene.h"
#include "../../Texture.h"

#include "../../Utilities/EventSystem/EventSystem.h"
#include "../../Utilities/EventSystem\Events/CloseClientEvent.h"
#include "../../Utilities/EventSystem\Events/ServerResponseEvent.h"
#include "../../Networker.h"
#include "../../Application.h"
#include <ProtoMessages.pb.h>
#include <string>

extern EventSystem* g_pEventSystem;
extern Networker* g_pNetworker;

Button* LobbyScene::pExitGameButton = nullptr;
Button* LobbyScene::pCreateRoomButton = nullptr;
int LobbyScene::numsPressed = 0;

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
LobbyScene::LobbyScene()
	:m_pMessage(nullptr)
{

	m_pMessage = new TextField();
	m_pMessage->x = 10;
	m_pMessage->y = 10;
	m_pMessage->m_stringMessage = "LOBBY";


	m_pPlayerList = new TextField();
	m_pPlayerList->m_stringMessage = "Name + Score:";
	m_pPlayerList->x = 400;
	m_pPlayerList->y = 20;

	pCreateRoomButton = new Button(10, 50, 200, 50, "CREATE ROOM", OnCreateRoomButton, 0);
	pCreateRoomButton->SetColor(66, 134, 244, 255);

	pExitGameButton = new Button(400, 300, 200, 50, "EXIT GAME", OnExitGameButton, 0);
	pExitGameButton->SetColor(66, 134, 244, 255);
}
LobbyScene::~LobbyScene()
{
	auto it = m_pLobbyRoomList.begin();
	while (it != m_pLobbyRoomList.end())
	{
		auto prevIt = it;
		++it;
		delete (*prevIt);
	}
	auto it2 = m_pLobbyPlayerList.begin();
	while (it2 != m_pLobbyPlayerList.end())
	{
		auto prevIt = it2;
		++it2;
		delete (*prevIt);
	}
	m_pLobbyPlayerList.clear();
	m_pLobbyRoomList.clear();

	//	delete pJoinRoomButton;
	delete pExitGameButton;
	delete pCreateRoomButton;
	delete m_pPlayerList;
	delete m_pMessage;
}

void LobbyScene::OnJoinRoomButton(Button* pButton, int roomNum)
{
	pButton->SetColor(47, 97, 168, 255);
	SDL_Log("button was pressed. %d\n", numsPressed);
	Proto::ClientRequest* pClientRequest = new Proto::ClientRequest();
	Proto::JoinRoomRequest* pJoinRequest = new Proto::JoinRoomRequest();
	pJoinRequest->set_roomnumber(roomNum);
	pJoinRequest->set_token(Application::s_mUserToken);
	pClientRequest->set_allocated_joinroom(pJoinRequest);
	g_pNetworker->SendMessage(*pClientRequest);
	//g_pNetworker->ConnectToServer((char*)m_pServerIpButton->m_pMessage->m_stringMessage.c_str());
}

void LobbyScene::OnExitGameButton(Button* pButton, int)
{

	g_pEventSystem->TriggerEvent(new CloseClientEvent());
	pButton->SetColor(47, 97, 168, 255);

}

void LobbyScene::OnCreateRoomButton(Button* pButton, int)
{
	pButton->SetColor(47, 97, 168, 255);
	++numsPressed;
	SDL_Log("button was pressed. %d\n", numsPressed);
	Proto::ClientRequest* pClientRequest = new Proto::ClientRequest();
	Proto::CreateRoomRequest* pCreateRequest = new Proto::CreateRoomRequest();
	pCreateRequest->set_token(Application::s_mUserToken);
	pClientRequest->set_allocated_createroom(pCreateRequest);
	g_pNetworker->SendMessage(*pClientRequest);

}


void LobbyScene::UpdateRender()
{
	m_rendering = true;
	for (auto playerList : m_pLobbyPlayerList)
	{
		playerList->RenderText();
	}
	for (auto roomButton : m_pLobbyRoomList)
	{
		roomButton->Draw();
	}

	m_pPlayerList->RenderText();
	m_pMessage->RenderText();
	pExitGameButton->Draw();
	pCreateRoomButton->Draw();
	m_rendering = false;

}

void LobbyScene::Update(Proto::ServerResponse* pServerResponse)
{

	if (pServerResponse)
	{
		if (pServerResponse->has_lobbyupdate())
		{
			Proto::LobbyUpdate lobbyUpdate = pServerResponse->lobbyupdate();
			if (lobbyUpdate.has_playerlist())
			{
				Proto::PlayerList playerList = lobbyUpdate.playerlist();

				auto it = m_pLobbyPlayerList.begin();
				while (it != m_pLobbyPlayerList.end())
				{
					auto prevIt = it;
					++it;
					delete (*prevIt);
				}
				m_pLobbyPlayerList.clear();
				for (int i = 0; i < playerList.playerinfo_size(); ++i)
				{
					Proto::PlayerInfo playerInfo = playerList.playerinfo(i);
					TextField* newField = new TextField();
					TextField* playerScore = new TextField();
					newField->m_stringMessage = playerInfo.name();
					playerScore->m_stringMessage = std::to_string(playerInfo.score());
					int yPos = 50 + (i * 30);
					newField->x = 400;
					playerScore->x = 500;
					playerScore->y = yPos;
					newField->y = yPos;
					m_pLobbyPlayerList.push_back(newField);
					m_pLobbyPlayerList.push_back(playerScore);
				}
			}
			if (lobbyUpdate.has_roomlist())
			{
				Proto::RoomList roomList = lobbyUpdate.roomlist();
				auto it = m_pLobbyRoomList.begin();
				while (it != m_pLobbyRoomList.end())
				{
					auto prevIt = it;
					++it;
					delete (*prevIt);
				}
				m_pLobbyRoomList.clear();
				int roomButtonPosX = 100;
				for (int i = 0; i < roomList.numrooms(); ++i)
				{
					std::string buttonString = "Join Room";
					char buffer[33];
					buttonString.append(_itoa(i + 1, buffer, 10));
					Button* pButton = new Button(roomButtonPosX, 150 + (i * 50), 150, 40, (buttonString), OnJoinRoomButton, i);
					pButton->SetColor(116, 157, 224, 255);
					m_pLobbyRoomList.push_back(pButton);
				}
			}
		}
	}
	UpdateRender();
}

void LobbyScene::OnEvent(const Event* pEvent)
{

}

bool LobbyScene::Rendering()
{
	return m_rendering;
}