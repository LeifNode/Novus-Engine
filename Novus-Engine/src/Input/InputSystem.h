//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_INPUT_SYSTEM_H
#define NOVUS_INPUT_SYSTEM_H

#include "Application/Common.h"
#include "Events/EventSystem.h"
#include "KeyboardState.h"
#include "MouseState.h"


namespace novus
{

class InputSystem
{
	friend class NovusApplication;

public:
	void Update(float dt);

	const MouseState* getMouseState() const { return &mMouseState; }
	const MouseState* getPreviousMouseState() const { return &mPreviousMouseState; }
	const KeyboardState* getKeyboardState() const { return &mKeyboardState; }
	const KeyboardState* getPreviousKeyboardState() const { return &mPreviousKeyboardState; }

	//HydraManager* getHydra() const { return mpHydraManager; }

private:
	InputSystem();
	~InputSystem();

	bool ProcessWindowsMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	///<summary>
	///Adds an input event that will get dispatched upon update()
	///</summary>
	///<returns></returns>
	void EnqueueEvent(IEventDataPtr);

private:
	//HydraManager* mpHydraManager;

	MouseState mPreviousMouseState;
	MouseState mMouseState;

	KeyboardState mPreviousKeyboardState;
	KeyboardState mKeyboardState;
};

}

#endif