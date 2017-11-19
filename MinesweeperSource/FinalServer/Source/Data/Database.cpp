#include "Database.h"

#include <iostream>
#include<fstream>
//using namespace std;
const int MAXLINE = 256;
Database::Database()
{
	LoadDatabaseFromFile();
	//AddUser("newUser2", "newPassword2");
	//CheckUserPassword("log1", "pass2");
}

Database::~Database()
{
}

void Database::Init()
{

}

void Database::AddUser(const std::string& login, const std::string& password)
{
	std::string checkPass = m_userList[login];
	if (checkPass.length() <= 0)
	{
		m_userList[login] = password;
		SaveDatabaseToFile();
	}
}

bool Database::CheckUserLoggedIn(const std::string& login)
{
	for (auto user : m_loggedUsers)
	{
		if (user.second->name == login)
		{
			return true;
		}
	}
	return false;
}

bool Database::CheckUserLoggedIn(const int connfd)
{
	return false;
}

void Database::KickUser(const int connfd)
{
	auto it = m_loggedUsers.find(connfd);
	if(it != m_loggedUsers.end())
	{
		if (it->second->state == User::State::k_game)
		{
			printf("user : %s is being saved by being in game state.\n", it->second->name.c_str());
			m_savedUsers[it->second->name] = it->second;
		}
		m_loggedUsers.erase(it);
		printf("user : %d has been logged out.\n", connfd);
	}
	else
	{
		printf("user : %d was not logged in.\n", connfd);

	}

	printf("\n");
}

//Check if the given user name matches its password.
bool Database::CheckUserPassword(const std::string& login, const std::string& password)
{
	printf("CheckingUser: %s for correct password.\n", login.c_str());
	auto it = m_userList.find(login);
	if (it != m_userList.end())
	{
		printf("User: %s found in the database.\n", login.c_str());
		if (it->second == password)
		{
			printf("User's: %s password was correct.\n", login.c_str());
			return true;
		}
		else
		{
			printf("User's: %s password was not correct.\n", login.c_str());
		}
	}
	else
	{
		printf("User: %s was not found in the database.\n", login.c_str());

	}
	return false;
}

User* Database::LogUser(int connfd, const std::string& login, int token)
{
	User* pUser = nullptr;
	printf("Checking for username: %s saved connection.\n", login.c_str());
	auto it = m_savedUsers.find(login);
	if(it != m_savedUsers.end())
	{
		printf("username: %s has saved profile. Using saved profile.\n", login.c_str());
		pUser = it->second;
	}
	else
	{
		printf("username: %s needs new profile generated.\n", login.c_str());
		pUser = new User();
		pUser->name = login.c_str();
		pUser->score = 0;
		pUser->token = token;
		pUser->connfd = connfd;
		pUser->state = User::State::k_game;
	}

	printf("User: %s Is being logged in with token: %d.\n", pUser->name.c_str(), pUser->token);
	m_loggedUsers[connfd] = pUser;
	return pUser;
}

int Database::GetUserToken(const std::string& login)
{
	printf("Gathering User's: %s token.\n", login.c_str());
	auto it = m_savedUsers.find(login);
	if (it != m_savedUsers.end())
	{
		int token = it->second->token;
		printf("User's: %s token is: %d.\n", login.c_str(), token);
		return token;
	}
	printf("User: %s was not found in the logged in players list.\n", login.c_str());
	return 0;
}

void Database::LoadDatabaseFromFile()
{
	//this is our database path file: s_kDatabaseFilePath;
	printf("Loading user database from file.\n");
	std::ifstream inFile(s_kDatabaseFilePath, std::ios::in);
	char oneLine[MAXLINE];

	std::string login;
	std::string password;

	//std::string* currentRead;
	std::string* currentRead;
	currentRead = &login;

	enum ReadingState
	{
		k_login
		,k_password
	};
	while (inFile)
	{
		
		inFile.getline(oneLine, MAXLINE);
		ReadingState readingState = k_login;
		int currentCharIndex;
		char currentChar;
		bool readingLine = false;
		//while we have a character
		while (oneLine[0])
		{
			currentCharIndex = 0;
			currentChar = oneLine[currentCharIndex];
			if (currentChar)
				readingLine = true;
			//while were on the same string
			while (readingLine)
			{
				while (currentChar != ' ' && currentChar != '\0')
				{
					currentRead->push_back(currentChar);
					++currentCharIndex;
					currentChar = oneLine[currentCharIndex];
				}

				++currentCharIndex;
				currentChar = oneLine[currentCharIndex];

				switch (readingState)
				{
				case k_login:
					readingState = k_password;
					currentRead = &password;
					break;
				case k_password:
					readingState = k_login;
					currentRead = &login;
					m_userList[login] = password;
					login = "";
					password = "";
					currentCharIndex = 0;
					inFile.getline(oneLine, MAXLINE);
					readingLine = false;
					break;
				default:
					break;
				}
			}
			
		}
	}
	printf("Loading user database complete.\n\n");
	inFile.close();
}

void Database::SaveDatabaseToFile()
{
	std::ofstream outfile(s_kDatabaseFilePath, std::ios::out);
	std::string stringOut;
	auto listIt = m_userList.begin();
	char* string;
	while (listIt != m_userList.end())
	{
		stringOut.append(listIt->first);
		stringOut.push_back(' ');
		stringOut.append(listIt->second);
		string = nullptr;
		string = new char[stringOut.length() +1];
		strcpy(string, stringOut.c_str());
		outfile << string << std::endl;
		stringOut.clear();
		++listIt;
	}
	outfile.close();

}
