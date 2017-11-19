#pragma once

#include "../Event.h"
#include <SDL.h>
#include <ProtoMessages.pb.h>

class ServerResponseEvent : public Event
{
public:


private:
	Proto::ServerResponse* m_pResponse;

public:
	ServerResponseEvent(Proto::ServerResponse* pResponse)
		: Event(k_serverResponseEvent)
		, m_pResponse(pResponse)
	{

	};
	~ServerResponseEvent() {};

	//list of keys that we can choose from.

	//-------------------------------------------------------------------------------------------------------------------
	//Getters/Setters
	//-------------------------------------------------------------------------------------------------------------------
	Proto::ServerResponse* GetResponse() {return m_pResponse ;}

private:

};

