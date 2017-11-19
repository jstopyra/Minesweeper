#pragma once

#include "../GameScene.h"
#include <mutex>
#include <condition_variable>

class LobbyScene : public GameScene
{

	TextField* m_pMessage;
	TextField* m_pPlayerList;
	std::vector<TextField*> m_pLobbyPlayerList;
	std::vector<Button*> m_pLobbyRoomList;


	std::mutex m_mtx;
	std::condition_variable m_condition;

	bool m_rendering = false;
	bool m_editingScene = false;
	bool Rendering();

	//static Button* pJoinRoomButton;
	static Button* pExitGameButton;
	static Button* pCreateRoomButton;
	static int numsPressed;
public:
	LobbyScene();
	~LobbyScene();
	virtual void UpdateRender() override;

	virtual void Update(Proto::ServerResponse* pResponse) override;

	virtual void OnEvent(const Event* pEvent);

	static void OnJoinRoomButton(Button* pButton, int roomNum);
	static void OnExitGameButton(Button* pButton, int);
	static void OnCreateRoomButton(Button* pButton, int);

private:

};