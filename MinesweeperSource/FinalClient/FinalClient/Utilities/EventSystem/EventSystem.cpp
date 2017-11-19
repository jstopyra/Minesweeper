// EventSystem.cpp
#include "EventSystem.h"
#include "Event.h"
#include "EventListener.h"
#include "../../Utilities/Macros.h"
#include <Windows.h>
#include <iostream>

void EventSystem::AttachListener(const EventId& eventId, EventListener* pListener)
{
	m_listeners[eventId].push_back(pListener);
}

void EventSystem::RemoveListener(const EventId& eventId, EventListener* pListener)
{
	// find the list we care about
	auto findIt = m_listeners.find(eventId);
	if (findIt == m_listeners.end())
		return;

	// loop through the list and find the listener to remove
	EventListenerList& eventListenerList = findIt->second;
	eventListenerList.remove(pListener);
}

// event management
void EventSystem::TriggerEvent(const Event* pEvent)
{
	// find the list we care about

	auto findIt = m_listeners.find(pEvent->GetEventId());
	if (findIt == m_listeners.end())
		return;

	// send the event to every listener
	for (auto* pEventListener : findIt->second)
	{
		if(pEventListener)
			pEventListener->OnEvent(pEvent);
	}

	SAFE_DELETE(pEvent);
}



