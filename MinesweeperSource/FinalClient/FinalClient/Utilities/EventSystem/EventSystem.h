// EventSystem.h
#ifndef EVENTSYSTEM_H
#define EVENTSYSTEM_H

#include "EventTypes.h"
#include <unordered_map>

class Event;
class EventListener;


class EventSystem
{
	typedef std::list<EventListener*> EventListenerList;
	std::unordered_map<EventId, EventListenerList> m_listeners;

public:
	// listener management
	void AttachListener(const EventId& eventId, EventListener* pListener);
	void RemoveListener(const EventId& eventId, EventListener* pListener);

	// event management
	void TriggerEvent(const Event* pEvent);

};

#endif

