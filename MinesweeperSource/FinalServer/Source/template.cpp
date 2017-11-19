// DaytimeClient.cpp : Defines the entry point for the console application.
//
#include "VisStud/stdafx.h"

#include "ProtoMessages.pb.h"
#include "Network/Networker.h"
#include "Data\Database.h"
using namespace std;
int _tmain(int argc, _TCHAR *argv[])
{
	//string serializedString;
	//ProtoBufTest::Login_Player loginPlayer;
	Networker networker;
	networker.Update();


	// cleanup
	//WSACleanup();

    return 0;
}
