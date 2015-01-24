//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_EVENTS_H
#define NOVUS_EVENTS_H

#include <Windows.h>

#include "EventSystem.h"
#include "Input/MouseState.h"
#include "Input/KeyboardState.h"

namespace novus
{

//Mouse events
class EventData_MouseDown : public BaseEventData
{
public:
	EventData_MouseDown(int mouseX, int mouseY, MouseButton::Type mouseButton) : x(mouseX), y(mouseY), mButton(mouseButton) { }
	virtual ~EventData_MouseDown() { }

	virtual const EventType& getEventType() const { return skEventType; }
	virtual const char* getName() const { return "EventData_MouseDown"; }
	virtual IEventDataPtr copy() const
	{
		return IEventDataPtr(new EventData_MouseDown(x, y, mButton));
	}

	int getX() const { return x; }
	int getY() const { return y; }
	MouseButton::Type getButton() const { return mButton; }

public:
	static const EventType skEventType;

private:
	int x, y;
	MouseButton::Type mButton;
};

class EventData_MouseUp : public BaseEventData
{
public:
	EventData_MouseUp(int mouseX, int mouseY, MouseButton::Type mouseButton) : x(mouseX), y(mouseY), mButton(mouseButton) { }
	virtual ~EventData_MouseUp() { }

	virtual const EventType& getEventType() const { return skEventType; }
	virtual const char* getName() const { return "EventData_MouseUp"; }
	virtual IEventDataPtr copy() const
	{
		return IEventDataPtr(new EventData_MouseUp(x, y, mButton));
	}

	int getX() const { return x; }
	int getY() const { return y; }
	MouseButton::Type getButton() const { return mButton; }

public:
	static const EventType skEventType;

private:
	int x, y;
	MouseButton::Type mButton;
};

class EventData_MouseMove : public BaseEventData
{
public:
	EventData_MouseMove(int mouseX, int mouseY) : x(mouseX), y(mouseY) { }
	virtual ~EventData_MouseMove() { }

	virtual const EventType& getEventType() const { return skEventType; }
	virtual const char* getName() const { return "EventData_MouseUp"; }
	virtual IEventDataPtr copy() const
	{
		return IEventDataPtr(new EventData_MouseMove(x, y));
	}

	int getX() const { return x; }
	int getY() const { return y; }

public:
	static const EventType skEventType;

private:
	int x, y;
};

//Keyboard events
class EventData_KeyboardDown : public BaseEventData
{
public:
	explicit EventData_KeyboardDown(KeyboardKey::Type key) : key(key) { }
	explicit EventData_KeyboardDown(WPARAM key) : key((KeyboardKey::Type)key) { }
	virtual ~EventData_KeyboardDown() { }

	virtual const EventType& getEventType() const { return skEventType; }
	virtual const char* getName() const { return "EventData_KeyboardDown"; }
	virtual IEventDataPtr copy() const
	{
		return IEventDataPtr(new EventData_KeyboardDown(key));
	}

	KeyboardKey::Type getKey() const { return key; }

public:
	static const EventType skEventType;

private:
	KeyboardKey::Type key;
};

class EventData_KeyboardUp : public BaseEventData
{
public:
	explicit EventData_KeyboardUp(KeyboardKey::Type key) : key(key) { }
	explicit EventData_KeyboardUp(WPARAM key) : key((KeyboardKey::Type)key) { }
	virtual ~EventData_KeyboardUp() { }

	virtual const EventType& getEventType() const { return skEventType; }
	virtual const char* getName() const { return "EventData_KeyboardUp"; }
	virtual IEventDataPtr copy() const
	{
		return IEventDataPtr(new EventData_KeyboardUp(key));
	}

	KeyboardKey::Type getKey() const { return key; }

public:
	static const EventType skEventType;

private:
	KeyboardKey::Type key;
};

}

#endif