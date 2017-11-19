#pragma once
#include <sstream>
#include "Utilities\EventSystem\EventListener.h"
#include <mutex>
#include <condition_variable>
#include <queue>
//#include <ProtoMessages.pb.h>

typedef unsigned long SceneId;

struct SDL_Window;
class Texture;
union SDL_Event;
struct SDL_Color;

class GameScene;



namespace Proto
{
	class ServerResponse;
}

class Application : public EventListener
{
	//SDL_ttf* font;
	//color we will use to render text with
	SDL_Color* textColor;

	GameScene* m_pCurrentScene;
	GameScene* m_pOldScene;
	bool m_requestClose = false;
	static Application* m_pApplication;

	//text string that our input will be written into
	std::string* inputText;

	//Event handler
	SDL_Event* e;

	//The window we'll be rendering to
	SDL_Window* m_pWindow;

	//is our application running
	bool m_appRunning = false;

	//Scene textures
	Texture* m_pPromptTextTexture;
	Texture* m_pInputTextTexture;
	Application();
	void PreInit();
	Proto::ServerResponse* _s_pServerResponse;
public:

	static int s_mUserToken;

	//Starts up SDL and creates window
	bool Init();

	//Loads media
	bool LoadMedia();
	void ChangeTypeStringPtr(std::string* stringMem);
	static Application* GetApplication();
	void LoadScene(SceneId sceneId);
	void LoadScene(GameScene* pScene);

	//Update the entire rendering side
	bool Update();

	//Is the application Running?
	const bool IsRunning() { return (m_appRunning && !m_requestClose); }
	bool IsRendering() { return m_rendering; }

	std::mutex m_mutex;
	std::condition_variable m_condition;
	bool m_rendering = false;
	bool IsNotRendering() { return !IsRendering(); }

	//Frees media and shuts down SDL
	void Close();

	virtual void OnEvent(const Event* pEvent);

	const int GetScreenWidth();
	const int GetScreenHeight();
	static std::queue<Proto::ServerResponse*> s_pServerResponse;

};