// EventListener.h
#ifndef EVENTLISTENER_H
#define EVENTLISTENER_H

class Event;

class EventListener
{
public:
    virtual ~EventListener() { }
    virtual void OnEvent(const Event* pEvent) = 0;
};

#endif
