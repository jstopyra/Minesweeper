#pragma once

#include "../GameScene.h"

enum LoginState
{
	k_typingLogin
	,k_typingPassword
	,k_reset
};

class LoginScreen : public GameScene
{
	TextField* m_pMessage;
	TextField* m_pErrorMessage;
	static TextField* m_pLogin;
	static TextField* m_pPassword;

	Button* pLoginButton;
	Button* pPasswordButton;

	bool m_loginFilled;
	bool m_loginInUse;
	bool m_passwordFilled;
	bool m_passwordInUse;
	bool m_errorMessageUp;

//	Button 

	static LoginState m_loginState;
public:
	LoginScreen();
	~LoginScreen();
	virtual void UpdateRender() override;

	virtual void Update(Proto::ServerResponse* pResponse) override;

	virtual void OnEvent(const Event* pEvent);

	static void OnLoginButton(Button* pButton, int);
	static void OnPasswordButton(Button* pButton, int);

private:
	void OnReturnHit();
	void CheckCridentials();
	void OnAnyKeyHit();
	void LoginReceive(Proto::ServerResponse* serverResponse);

};