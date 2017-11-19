#include "RoomScene.h"
#include "../../Networker.h"
#include "../../Application.h"
#include "../../Utilities/EventSystem/EventSystem.h"
#include "../..\Utilities/EventSystem/Event.h"
#include "../..\Utilities/EventSystem\Events/ServerResponseEvent.h"

extern EventSystem* g_pEventSystem;
extern Networker* g_pNetworker;

extern Networker* g_pNetworker;
Button* RoomScene::m_pQuitGameButton = nullptr;
Button* RoomScene::m_pBackgroundButton = nullptr;

void RoomScene::OnTilePressed(Button * pButton, int tileIndex)
{
	Proto::ClientRequest* pClientRequest = new Proto::ClientRequest();
	Proto::TileClickedRequest* pTileClicked = new Proto::TileClickedRequest();
	pTileClicked->set_tileindex(tileIndex);
	pTileClicked->set_token(Application::s_mUserToken);
	pClientRequest->set_allocated_tileclicked(pTileClicked);
	g_pNetworker->SendMessage(*pClientRequest);
	//pButton->SetColor(162, 214, 163, 255);
}

RoomScene::RoomScene()
	:m_buttonWidth(30)
	,m_buttonHeight(30)
	,m_numTilesX(10)
	,m_numTilesY(10)
{
	g_pEventSystem->AttachListener(k_inputKeyEvent, this);
	m_pQuitGameButton = new Button(400, 400, 150, 30, "Quit To Lobby", OnQuitButton);
	m_pQuitGameButton->SetColor(116, 157, 224, 255);
	int tileSpacing = 3;
	

	m_pBackgroundButton = new Button(0, 0, m_numTilesX *(tileSpacing+ m_buttonWidth) + tileSpacing, m_numTilesY *(tileSpacing + m_buttonWidth) + tileSpacing, "");
	m_pBackgroundButton->SetColor(50, 50, 50, 255);
}

RoomScene::~RoomScene()
{
	g_pEventSystem->RemoveListener(k_inputKeyEvent, this);
	delete m_pBackgroundButton;
	{
		auto it = m_pRoomPlayerList.begin();
		while (it != m_pRoomPlayerList.end())
		{
			auto prevIt = it;
			++it;
			delete (*prevIt);
		}
	}

	{
		auto it = m_pTileMap.begin();
		while (it != m_pTileMap.end())
		{
			auto prevIt = it;
			++it;
			delete (*prevIt);
		}
	}
	m_pTileMap.clear();
	delete m_pQuitGameButton;
}

void RoomScene::UpdateRender()
{
	m_pBackgroundButton->Draw();

	m_rendering = true;
	for (auto playerList : m_pRoomPlayerList)
	{
		playerList->RenderText();
	}
	m_pQuitGameButton->Draw();
	for (auto tile : m_pTileMap)
	{
		tile->Draw();
	}
	m_rendering = false;
}

void RoomScene::Update(Proto::ServerResponse* pServerResponse)
{
	if(pServerResponse)
	if (pServerResponse->has_roomupdate())
	{
		Proto::RoomUpdate roomUpdate = pServerResponse->roomupdate();

		if (roomUpdate.has_playerlist())
		{
			Proto::PlayerList playerList = roomUpdate.playerlist();

			auto it = m_pRoomPlayerList.begin();
			while (it != m_pRoomPlayerList.end())
			{
				auto prevIt = it;
				++it;
				delete (*prevIt);
			}
			m_pRoomPlayerList.clear();
			for (int i = 0; i < playerList.playerinfo_size(); ++i)
			{
				Proto::PlayerInfo playerInfo = playerList.playerinfo(i);
				TextField* playerName = new TextField();
				TextField* playerScore = new TextField();
				playerName->m_stringMessage = playerInfo.name();
				playerScore->m_stringMessage = std::to_string(playerInfo.score());
				int yPos = 50 + (i * 30);
				playerScore->x = 500;
				playerScore->y = yPos;
				playerName->x = 400;
				playerName->y = yPos;
				m_pRoomPlayerList.push_back(playerName);
				m_pRoomPlayerList.push_back(playerScore);
			}
		}
		if (roomUpdate.has_mapupdate())
		{
			Proto::Map map = roomUpdate.mapupdate();
			m_numTilesX = map.mapwidth();
			m_numTilesY = map.mapheight();
			auto it = m_pTileMap.begin();
			while(it != m_pTileMap.end())
			{
				auto prevIt = it;
				++it;
				delete(*prevIt);
			}
			m_pTileMap.clear();
			//for each map tile, create a new tile and set it up
			int tileSpacing = 3;
			std::string tileMessage = "";
			int r= 0, g = 0, b = 0;
			for (int i = 0; i < map.squares_size(); ++i)
			{
				int squareType = map.squares(i).squaretype();
				if (squareType == -1)
				{
					tileMessage = "";
					r = 0;
					g = 0;
					b = 255;
				}
				else if (squareType == 9)
				{
					tileMessage = "X";
					r = 255;
					g = 0;
					b = 0;
				}
				else
				{
					tileMessage = std::to_string(squareType);
					r = 0;
					g = 255;
					b = 0;
				}
				Tile* pTile = new Tile();
				int xPos = i% m_numTilesX;
				int yPos = i / m_numTilesY;
				pTile->m_pButton = new Button(tileSpacing + (xPos)*(tileSpacing + m_buttonWidth), tileSpacing + (yPos)*(tileSpacing + m_buttonHeight), m_buttonWidth, m_buttonHeight, tileMessage.c_str(), OnTilePressed, i);
				pTile->m_pButton->SetColor(r, g, b, 255);
				m_pTileMap.push_back(pTile);
			}
		}
	}
	UpdateRender();
}

void RoomScene::OnEvent(const Event * pEvent)
{

}

void RoomScene::OnQuitButton(Button * pButton, int)
{
	Proto::ClientRequest* pClientRequest = new Proto::ClientRequest();
	Proto::QuitRoom* pQuitRoom = new Proto::QuitRoom();
	//pJoinRequest->set_roomnumber(0);
	pQuitRoom->set_token(Application::s_mUserToken);
	pClientRequest->set_allocated_quitroom(pQuitRoom);
	g_pNetworker->SendMessage(*pClientRequest);
}
