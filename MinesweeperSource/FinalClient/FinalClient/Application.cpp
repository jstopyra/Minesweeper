#include "Application.h"
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL.h>
#include "Texture.h"
#include "SDL_ttf.h"
//#include <sdl_Color>
#include "Utilities\EventSystem\EventSystem.h"
#include "Utilities\EventSystem\Events\InputEvent.h"
#include "Utilities\EventSystem\Events\ServerResponseEvent.h"
#include "Utilities\EventSystem\Events\MouseClickEvent.h"
#include "GameScenes\Scenes\LoginScreen.h"
#include "GameScenes\Scenes\LobbyScene.h"
#include "GameScenes\Scenes\LoadingScene.h"
#include "GameScenes\Scenes\RoomScene.h"
#include "GameScenes\GameScene.h"
#include "Networker.h"
#include<ProtoMessages.pb.h>


Application* Application::m_pApplication = nullptr;
int Application::s_mUserToken = 0;
std::queue<Proto::ServerResponse*> Application::s_pServerResponse;// = std::queue<Application::s_pServerResponse>;

//The window renderer
SDL_Renderer* gRenderer;
Networker* g_pNetworker;

//Globally used font
TTF_Font* gFont;

//Globally used event system
EventSystem* g_pEventSystem;

//Screen dimension constants
//extern const int SCREEN_WIDTH = 640;
//extern const int SCREEN_HEIGHT = 480;
//Screen dimension constants
extern const int SCREEN_WIDTH = 640;
extern const int SCREEN_HEIGHT = 480;

Application::Application()
{

}

void Application::PreInit()
{

	textColor = new SDL_Color();
	inputText = new std::string("");
	e = new SDL_Event();
	m_appRunning = true;
	m_pPromptTextTexture = new Texture();
	m_pInputTextTexture = new Texture();
	m_pCurrentScene = nullptr;
	g_pEventSystem = new EventSystem();
	g_pEventSystem->AttachListener(k_closeClientEvent, this);
	GetApplication()->LoadScene(new LoadingScene());
}

const int Application::GetScreenWidth()
{
	return SCREEN_WIDTH;
}
const int Application::GetScreenHeight()
{
	return SCREEN_HEIGHT;;
}
void Application::LoadScene(SceneId sceneId)
{
	GameScene* pScene = nullptr;
	switch (sceneId)
	{
	case k_loginScene:
	{
		pScene = new LoginScreen();
	}
	break;
	case k_lobbyScene:
	{
		pScene = new LobbyScene();
	}
	break;
	case k_loadingScene:
	{
		pScene = new LoadingScene();
	}
	break;
	default:
		break;
	}
	if (pScene)
	{
		m_pOldScene = m_pCurrentScene;
		m_pCurrentScene = pScene;
		//delete oldScene;
	}
}

void Application::LoadScene(GameScene* pScene)
{
	if (pScene)
	{
		m_pOldScene = m_pCurrentScene;
		m_pCurrentScene = pScene;
	}
}

Application* Application::GetApplication()
{
	if (!m_pApplication)
	{
		m_pApplication = new Application();
		m_pApplication->PreInit();
	}
	return m_pApplication;
}

void Application::ChangeTypeStringPtr(std::string* stringMem)
{
	//if(stringMem)
	inputText = stringMem;
}
 
bool Application::Init()
{
	g_pNetworker = new Networker();
	//g_pNetworker->ConnectToServer("127.0.0.1");

	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		m_pWindow = SDL_CreateWindow("JakubStopyra_NetworkingFinal", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (m_pWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}

				//Initialize SDL_ttf
				if (TTF_Init() == -1)
				{
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
					success = false;
				}
			}
		}
	}
	if (!LoadMedia())
	{
		printf("Failed to load media!\n");
	}
	else
	{
		//Set text color as black
		//*textColor = { 0, 0, 0, 0xFF };

		//The current input text.
		//*inputText = "";

		//m_pInputTextTexture->loadFromRenderedText(inputText->c_str());

		//Enable text input
		SDL_StartTextInput();
	}

	return success;
}

bool Application::LoadMedia()
{
	//Loading success flag
	bool success = true;

	//Open the font
	gFont = TTF_OpenFont("../Source/Includes/Fonts/GeosansLight.ttf", 28);
	if (gFont == NULL)
	{
		printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
		success = false;
	}
	else
	{
		//Render the prompt
		if (!m_pPromptTextTexture->loadFromRenderedText("Enter Text:"))
		{
			printf("Failed to render prompt text!\n");
			success = false;
		}
	}

	return success;
}

bool Application::Update()
{
	
	m_appRunning = true;
	if (m_requestClose)
		return false;

	//The rerender text flag
	bool renderText = false;

	//Handle events on queue
	while (SDL_PollEvent(e) != 0)
	{
		//User requests quit
		if (e->type == SDL_QUIT)
		{
			m_appRunning = false;
		}
		//mouse event
		else if (e->type == SDL_MOUSEBUTTONDOWN)
		{
			//If the left mouse button was pressed
			if (e->button.button == SDL_BUTTON_LEFT)
			{
				//Get the mouse offsets
				g_pEventSystem->TriggerEvent(new MouseClickEvent(e->button.x, e->button.y));
			}
		}
		//Special key input
		else if (e->type == SDL_KEYDOWN)
		{

			//Handle backspace
			if (inputText)
			{
				if (e->key.keysym.sym == SDLK_BACKSPACE && inputText->length() > 0)
				{
					//lop off character
						inputText->pop_back();
					//renderText = true;
				}
				//Handle copy
				else if (e->key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL)
				{
					SDL_SetClipboardText(inputText->c_str());
				}
				//Handle paste
				else if (e->key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL)
				{
					*inputText = SDL_GetClipboardText();
					//renderText = true;
				}
			}
			//Handle quit game
			if (e->key.keysym.sym == SDLK_ESCAPE)
			{
				m_appRunning = false;
			}
			////Trigger an event about a key down
			Event* pEvent = new InputKeyDownEvent(e->key.keysym.sym);
			g_pEventSystem->TriggerEvent(pEvent);
		}
		//Special text input event
		else if (e->type == SDL_TEXTINPUT)
		{
			//Not copy or pasting
			if (!((e->text.text[0] == 'c' || e->text.text[0] == 'C') && (e->text.text[0] == 'v' || e->text.text[0] == 'V') && SDL_GetModState() & KMOD_CTRL))
			{
				//Append character
				if (inputText)
					*inputText += e->text.text;
				//renderText = true;
			}
			//Trigger an event about a key down
			Event* pEvent = new InputKeyDownEvent(e->key.keysym.sym);
			g_pEventSystem->TriggerEvent(pEvent);
		}
	}


	{
		std::lock_guard<std::mutex> lk(m_mutex);
		m_rendering = true;
		if (s_pServerResponse.size() > 0)
		{
			_s_pServerResponse = s_pServerResponse.front();
			s_pServerResponse.pop();
		}
		//Clear screen
		m_rendering = false;
		m_condition.notify_all();
	}

	if (_s_pServerResponse)
	{
		if (_s_pServerResponse->has_loginresponse())
		{
			Proto::LoginResponse loginResponse = _s_pServerResponse->loginresponse();
			s_mUserToken = loginResponse.token();
		}
		if (_s_pServerResponse->gamestate() == Proto::GameState::LOBBY)
		{
			LoadScene(new LobbyScene());
		}
		else if (_s_pServerResponse->gamestate() == Proto::GameState::GAME)
		{
			LoadScene(new RoomScene());
		}
	}

	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(gRenderer);

	if (m_pCurrentScene == m_pOldScene)
		m_pCurrentScene->Update(_s_pServerResponse);
	else
	{
		delete m_pOldScene;
		m_pOldScene = m_pCurrentScene;
	}
	//Update screen
	delete _s_pServerResponse;
	_s_pServerResponse = nullptr;
	SDL_RenderPresent(gRenderer);

	return m_appRunning;
}


void Application::Close()
{
	//Disable text input
	g_pEventSystem->RemoveListener(k_closeClientEvent, this);
	SDL_StopTextInput();

	//Free loaded images
	m_pPromptTextTexture->free();
	m_pInputTextTexture->free();

	//Free global font
	TTF_CloseFont(gFont);
	gFont = NULL;

	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(m_pWindow);
	m_pWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void Application::OnEvent(const Event * pEvent)
{
	if (pEvent->GetEventId() == k_closeClientEvent)
	{
		m_requestClose = true;
	}

}
