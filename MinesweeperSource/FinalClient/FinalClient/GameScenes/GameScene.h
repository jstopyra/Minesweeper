#pragma once
#include "../Utilities/EventSystem/EventListener.h"
#include "ProtoMessages.pb.h"
#include <string>
#include <SDL.h>
class Texture;



typedef unsigned long SceneId;
const SceneId k_lobbyScene = 0x55f58298;
const SceneId k_loginScene =	0x381534ae;
const SceneId k_loadingScene =	0x381245a2;


class TextField
{
public:
	Texture* m_pTexture;
	std::string m_string;
	std::string m_stringMessage;
	int x, y;
	TextField();
	~TextField();

	void RenderText(int x, int y);
	void RenderText();
	void LoadText();
};

class Button : public EventListener
{
	void CreateButton(int x, int y, int w, int h, std::string text);
	bool buttonCallbackSet = false;
	int assignedInt;
public:
	TextField* m_pMessage;
	void SetText(std::string text);
	int m_width, m_height, m_x, m_y;
	int m_r, m_g, m_b, m_a;
	void SetColor(int r, int g, int b, int a);
	virtual void OnEvent(const Event* pEvent);
	Button(int x, int y, int w, int h, std::string text, void(*callbackFunc)(Button* pButton, int));
	Button(int x, int y, int w, int h, std::string text, void(*callbackFunc)(Button* pButton, int), int newInt);
	Button(int x, int y, int w, int h, std::string text);
	~Button();

	void Draw();
	void OnClicked();
	//void(*fnPtr)(Button* pButton);
	void(*fnPtr)(Button* pButton, int);
};

class GameScene : public EventListener
{
public:
	GameScene();
	virtual ~GameScene();

	//There always has to be a running scene so forse update to be pure virtual. This way we cannot instantiate this class.
	virtual void UpdateRender() = 0;
	virtual void Update(Proto::ServerResponse* pResponse) = 0;
	
	

private:

};
