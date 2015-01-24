//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_EVENT_SYSTEM_H
#define NOVUS_EVENT_SYSTEM_H

//Implmentation adapted from GCC4
//Stripped multithreading code and multiple Event queue support since they were not needed

#include <strstream>
#include <ostream>
#include <istream>
#include <memory>
#include <list>
#include <map>
#include "3rdParty/FastDelegate.h"

#ifndef EVENT_CAST
#define EVENT_CAST(type, ptr) static_pointer_cast<type>(ptr)
#endif

namespace novus
{

class IEventData;

typedef unsigned long EventType;
typedef std::shared_ptr<IEventData> IEventDataPtr;
typedef fastdelegate::FastDelegate1<IEventDataPtr> EventListenerDelegate;

class IEventData
{
public:
	virtual ~IEventData() {}
	virtual const EventType& getEventType() const = 0;
	virtual float getTimeStamp() const = 0;
	//virtual void Serialize(std::ostrstream& out) const = 0;
	//virtual void Deserialize(std::istrstream& in) = 0;
	virtual IEventDataPtr copy() const = 0;
	virtual const char* getName() const = 0;
};

class BaseEventData : public IEventData
{
	const float m_timeStamp;

public:
	explicit BaseEventData(const float timeStamp = 0.0f) : m_timeStamp(timeStamp) { }

	// Returns the type of the event
	virtual const EventType& getEventType() const = 0;

	virtual float getTimeStamp() const { return m_timeStamp; }

	// Serializing for network input / output
	//virtual void Serialize(std::ostrstream &out) const	{ }
	//virtual void Deserialize(std::istrstream& in) { }
};

class EventSystem
{
	typedef std::list<EventListenerDelegate> EventListenerList;
	typedef std::map<EventType, EventListenerList> EventListenerMap;
	typedef std::list<IEventDataPtr> EventQueue;

public:
	EventSystem();
	~EventSystem();

	bool AddListener(const EventType& type, const EventListenerDelegate& eventDelegate);
	bool RemoveListener(const EventType& type, const EventListenerDelegate& eventDelegate);

	bool TriggerEvent(const IEventDataPtr& pEvent) const;
	bool QueueEvent(const IEventDataPtr& pEvent);
	//bool abortEvent(const EventType& type, bool allOfType = false);

	void Clear();

	bool Update(unsigned long maxMillis = 0xffffffff);

private:
	EventListenerMap mEventListeners;
	EventQueue mQueue;
};

}

#endif