#pragma once

const static char* s_kDatabaseFilePath = "../Source/Data/Database.txt";

#include <unordered_map>
struct User
{
	enum State
	{
		k_lobby
		,k_game
	};
	int token;
	int score;
	int connfd;
	std::string name;
	State state;
};

class Database
{
	//login, password list
	std::unordered_map<std::string, std::string> m_userList;

	//connfd, user classlist
	std::unordered_map<int, User*> m_loggedUsers;

	//login name, user class*
	std::unordered_map<std::string, User*> m_savedUsers;

public:
	Database();
	~Database();

	void Init();
	//void AddUser(std::string& login, std::string& password);
	void AddUser(const std::string& login, const std::string& password);

	bool CheckUserPassword(const std::string& login, const std::string& password);

	int GetUserToken(const std::string& login);
	User* LogUser(int connfd, const std::string& login, int token);
	bool CheckUserLoggedIn(const std::string& login);
	bool CheckUserLoggedIn(const int connfd);

	void KickUser(const int connfd);


private:


	void LoadDatabaseFromFile();
	void SaveDatabaseToFile();

};