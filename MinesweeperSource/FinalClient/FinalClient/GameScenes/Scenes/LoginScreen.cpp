#include "LoginScreen.h"
#include "SDL.h"
#include "../../Texture.h"
#include "../../Application.h"
#include "../../Networker.h"
#include "../../Utilities/EventSystem/Event.h"
#include "../../Utilities/EventSystem/Events/InputEvent.h"
#include "../../Utilities/EventSystem/Events/ServerResponseEvent.h"
#include "../../Utilities/EventSystem/EventSystem.h"
#include "../../Networker.h"
#include "../../protobuf/ProtoMessages.pb.h"

extern EventSystem* g_pEventSystem;
extern Networker* g_pNetworker;

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

TextField* LoginScreen::m_pPassword = nullptr;
TextField* LoginScreen::m_pLogin = nullptr;
LoginState LoginScreen::m_loginState;

LoginScreen::LoginScreen()
	: m_loginFilled(false)
	, m_pMessage(nullptr)
	, m_pErrorMessage(nullptr)
	, m_passwordFilled(false)
	, m_errorMessageUp(false)
	, m_loginInUse(false)
	, m_passwordInUse(false)
{
	g_pEventSystem->AttachListener(k_inputKeyEvent, this);

	m_pMessage = new TextField();
	m_pErrorMessage = new TextField();
	m_pLogin = new TextField();
	m_pPassword = new TextField();

	m_pMessage->m_stringMessage = "Login and Password:";
	m_pLogin->m_stringMessage = "login";
	m_pPassword->m_stringMessage = "password";
	m_pErrorMessage->m_stringMessage = "";
	Application::GetApplication()->ChangeTypeStringPtr(&m_pLogin->m_string);
	pLoginButton = new Button(150, 30, 300, 30, "Login:", OnLoginButton);
	pLoginButton->SetColor(116, 157, 224, 255);

	pPasswordButton = new Button(150, 62, 300, 30, "Password:", OnPasswordButton);
	pPasswordButton->SetColor(116, 157, 224, 255);
}

LoginScreen::~LoginScreen()
{
	Application::GetApplication()->ChangeTypeStringPtr(nullptr);
	g_pEventSystem->RemoveListener(k_inputKeyEvent, this);
	delete m_pMessage;
	delete m_pErrorMessage;
	delete m_pLogin;
	delete pLoginButton;
	delete pPasswordButton;
	delete m_pPassword;
}

void LoginScreen::Update(Proto::ServerResponse* pResponse)
{
	if(pResponse)
		LoginReceive(pResponse);
	UpdateRender();

}

void LoginScreen::UpdateRender()
{
	pLoginButton->Draw();
	pPasswordButton->Draw();
	if(m_pMessage)
	m_pMessage->RenderText((SCREEN_WIDTH - m_pMessage->m_pTexture->getWidth()) / 2, 0);

	if(m_pErrorMessage)
	m_pErrorMessage->RenderText((SCREEN_WIDTH - m_pErrorMessage->m_pTexture->getWidth()) / 2, 3*m_pErrorMessage->m_pTexture->getHeight());

	if(m_pLogin)
	m_pLogin->RenderText((SCREEN_WIDTH - m_pLogin->m_pTexture->getWidth()) / 2, m_pLogin->m_pTexture->getHeight());

	if(m_pPassword)
	m_pPassword->RenderText((SCREEN_WIDTH - m_pPassword->m_pTexture->getWidth()) / 2, 2 * m_pPassword->m_pTexture->getHeight());

}


void LoginScreen::OnEvent(const Event* pEvent)
{
	EventId eventId = pEvent->GetEventId();
	switch (eventId)
	{
	//if its keyboard event:
	case k_inputKeyEvent:
	{
		InputKeyEvent* pInput = (InputKeyEvent*)(pEvent);
		if (pInput->KeyTypePressed() == SDLK_RETURN)
			OnReturnHit();
		else if (pInput->KeyTypePressed() == SDLK_TAB)
		{
			if (m_loginState == LoginState::k_typingPassword)
			{
				m_loginState = LoginState::k_typingLogin;
				Application::GetApplication()->ChangeTypeStringPtr(&m_pLogin->m_string);
			}
			else
			{
				m_loginState = LoginState::k_typingPassword;
				Application::GetApplication()->ChangeTypeStringPtr(&m_pPassword->m_string);
			}
		}
		OnAnyKeyHit();
	}
	break;
	default:
		break;
	}
}

void LoginScreen::OnLoginButton(Button * pButton, int)
{
	m_loginState = LoginState::k_typingLogin;
	Application::GetApplication()->ChangeTypeStringPtr(&m_pLogin->m_string);
	m_pLogin->m_stringMessage = "";
}

void LoginScreen::OnPasswordButton(Button * pButton, int)
{
	m_loginState = LoginState::k_typingPassword;
	Application::GetApplication()->ChangeTypeStringPtr(&m_pPassword->m_string);
	m_pPassword->m_stringMessage = "";
}

void LoginScreen::OnReturnHit()
{
		CheckCridentials();
}

void LoginScreen::CheckCridentials()
{
	//Ask the server if the password and the login are correct.
	Proto::ClientRequest* clientRequest = new Proto::ClientRequest();
	Proto::LoginRequest* loginRequest = new Proto::LoginRequest();
	loginRequest->set_login(m_pLogin->m_string);
	loginRequest->set_password(m_pPassword->m_string);
	clientRequest->set_allocated_loginrequest(loginRequest);

	g_pNetworker->SendMessage(*clientRequest);
	
	clientRequest->clear_loginrequest();

	
}

void LoginScreen::LoginReceive(Proto::ServerResponse* serverResponse)
{

	//serverResponse.
	if (serverResponse->has_loginresponse())
	{
		Proto::LoginResponse loginResponse = serverResponse->loginresponse();
		//token -1 means invalid login
		//token 0 means already logged in.
		//token > 0 means correct login.
		switch (loginResponse.token())
		{
			//invalid login
		case -1:
			{
				m_pErrorMessage->m_stringMessage = "Wrong login or password, Please try again.";

			}
			break;
				//already logged in.
		case 0:
			{
				m_pErrorMessage->m_stringMessage = "This user already logged in.";

			}
			break;
			//Logging in approved
		default:
			{

				m_pErrorMessage->m_stringMessage = "Welcome to the server.";
				return;
			}
			break;
		}
		m_loginFilled = false;
		m_passwordFilled = false;
		m_pLogin->m_stringMessage = "login";
		m_pPassword->m_stringMessage = "password";
		m_errorMessageUp = !m_errorMessageUp;
		m_pLogin->m_string = "";
		m_pPassword->m_string = "";
		Application::GetApplication()->ChangeTypeStringPtr(&m_pLogin->m_string);
		m_loginState = LoginState::k_typingLogin;
	}
}

void LoginScreen::OnAnyKeyHit()
{
	if (m_loginState == LoginState::k_typingLogin)
	{
		m_pLogin->m_stringMessage = m_pLogin ->m_string;
	}
	if (m_loginState == LoginState::k_typingPassword)
	{
		size_t passwordSize = m_pPassword->m_string.size();
		m_pPassword->m_stringMessage = "";
		for (int i = 0; i < passwordSize; ++i)
			m_pPassword->m_stringMessage += "*";
		
	}
	if (m_errorMessageUp)
	{
		m_errorMessageUp = !m_errorMessageUp;
		m_pErrorMessage->m_stringMessage = "";
	}
	
}