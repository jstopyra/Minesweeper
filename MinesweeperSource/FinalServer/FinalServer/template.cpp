// DaytimeClient.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include <iostream>
#include <string>
#include <sstream>
#include "ProtoMessages.pb.h"

using namespace std;
#include "Networker.h"
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
