#include "LoadingScene.h"
#include "../../Networker.h"
#include "../../Application.h"

extern Networker* g_pNetworker;

Button* LoadingScene::m_pJoinServerButton = nullptr;
Button* LoadingScene::m_pServerIpButton = nullptr;


LoadingScene::LoadingScene()
{
	int x, y, w, h;
	w = 200;
	h = 50;
	x = (Application::GetApplication()->GetScreenWidth() / 2) - w/2;
	y = 200;

	m_pJoinServerButton = new Button(x, y, w, h, "Join Server", OnJoinButton);
	m_pJoinServerButton->SetColor(116, 157, 224, 255);
	m_pJoinServerButton->m_pMessage->m_string = "127.0.0.1";

	y = 100;
	m_pServerIpButton = new Button(x, y, w, h, "127.0.0.1", OnServerIpButton);
	m_pServerIpButton->SetColor(116, 157, 224, 255);

}

LoadingScene::~LoadingScene()
{
	delete m_pJoinServerButton;
	delete m_pServerIpButton;

}

void LoadingScene::UpdateRender()
{
	m_pJoinServerButton->Draw();
	m_pServerIpButton->Draw();
}

void LoadingScene::Update(Proto::ServerResponse* pResponse)
{
	g_pNetworker->ConnectToServer((char*)m_pServerIpButton->m_pMessage->m_stringMessage.c_str());

	UpdateRender();

}

void LoadingScene::OnEvent(const Event * pEvent)
{

}

void LoadingScene::OnJoinButton(Button* pButton, int)
{
	g_pNetworker->ConnectToServer((char*)m_pServerIpButton->m_pMessage->m_stringMessage.c_str());
}

void LoadingScene::OnServerIpButton(Button * pButton, int)
{
	Application::GetApplication()->ChangeTypeStringPtr(&m_pServerIpButton->m_pMessage->m_stringMessage);

}
