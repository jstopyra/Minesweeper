#include "GameScene.h"
#include "../Texture.h"
#include "../Utilities/EventSystem/EventSystem.h"
#include "../Utilities/EventSystem/Event.h"
#include "../Utilities/EventSystem/Events/MouseClickEvent.h"
#include <SDL.h>

extern EventSystem* g_pEventSystem;
extern SDL_Renderer* gRenderer;


GameScene::GameScene()
{

}

GameScene::~GameScene()
{

}

void TextField::RenderText()
{
	RenderText(x, y);
}

void TextField::RenderText(int x, int y)
{
	LoadText();
	m_pTexture->render(x, y);
}

void TextField::LoadText()
{
	m_pTexture->loadFromRenderedText(m_stringMessage);
}

TextField::TextField()
{
	m_pTexture = new Texture();
	m_string = "";
}

TextField::~TextField()
{
	delete m_pTexture;
}

//Button::Button(int x, int y, int w, int h, std::string text, void(*callbackFunc)(Button* pButton))
//{
//	fnPtr = callbackFunc;
//	buttonCallbackSet = true;
//	CreateButton(x, y, w, h, text);
//}

Button::Button(int x, int y, int w, int h, std::string text, void(*callbackFunc)(Button* pButton, int))
{
	//assignedInt = newInt;
	fnPtr = callbackFunc;
	buttonCallbackSet = true;
	CreateButton(x, y, w, h, text);
}

Button::Button(int x, int y, int w, int h, std::string text, void(*callbackFunc)(Button* pButton, int), int newInt)
{
	assignedInt = newInt;
	fnPtr = callbackFunc;
	buttonCallbackSet = true;
	CreateButton(x, y, w, h, text);
}

Button::Button(int x, int y, int w, int h, std::string text)
{
	CreateButton(x, y, w, h, text);
}

Button::~Button()
{
	g_pEventSystem->RemoveListener(k_mouseClickEvent, this);
	delete m_pMessage;
	fnPtr = nullptr;
}

void Button::CreateButton(int x, int y, int w, int h, std::string text)
{
	m_x = x;
	m_y = y;
	m_width = w;
	m_height = h;
	g_pEventSystem->AttachListener(k_mouseClickEvent, this);
	m_pMessage = new TextField();
	m_pMessage->m_stringMessage = text;
}


void Button::Draw()
{
	SDL_Rect rect;
	rect.x = m_x;
	rect.y = m_y;
	rect.w = m_width;
	rect.h = m_height;
	Uint8 r, g, b, a;
	SDL_GetRenderDrawColor(gRenderer, &r, &g, &b, &a);
	SDL_SetRenderDrawColor(gRenderer, m_r, m_g, m_b, m_a);
	SDL_RenderFillRect(gRenderer, &rect);
	SDL_SetRenderDrawColor(gRenderer, r, g, b, a);
	m_pMessage->RenderText(m_x, m_y);
}

void Button::OnEvent(const Event* pEvent)
{
	if (pEvent->GetEventId() == k_mouseClickEvent)
	{
		MouseClickEvent* pClick = (MouseClickEvent*)pEvent;
		if (pClick)
		{
			if (pClick->GetXCord() > m_x && pClick->GetXCord() < m_x + m_width &&
				pClick->GetYCord() > m_y && pClick->GetYCord() < m_y + m_height)
			{
				OnClicked();
			}
		}
	}
}

void Button::OnClicked()
{
	//m_callbackFunc();
	if(buttonCallbackSet)
		fnPtr(this, assignedInt);
	SDL_Log("Button was clicked");
}

void Button::SetColor(int r, int g, int b, int a)
{
	m_r = r;
	m_g = g;
	m_b = b;
	m_a = a;
}

void Button::SetText(std::string text)
{
	m_pMessage->m_stringMessage = text;
}