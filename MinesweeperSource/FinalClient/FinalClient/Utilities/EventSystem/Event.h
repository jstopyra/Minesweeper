// Event.h
#ifndef EVENT_H
#define EVENT_H

#include "EventTypes.h"

class Event
{
	 EventId m_guid;

public:
	virtual ~Event() {};
	 Event(const EventId& guid) : m_guid(guid) { }

	 const EventId& GetEventId() const { return m_guid; }
};

#endif
