#pragma once
#include "../GameScene.h"

class LoadingScene : public GameScene
{

	static Button* m_pJoinServerButton;
	static Button* m_pServerIpButton;


public:
	LoadingScene();
	~LoadingScene();

	virtual void UpdateRender() override;

	virtual void Update(Proto::ServerResponse* pResponse) override;

	virtual void OnEvent(const Event* pEvent);

	static void OnJoinButton(Button* pButton, int);
	static void OnServerIpButton(Button* pButton, int);

private:

};