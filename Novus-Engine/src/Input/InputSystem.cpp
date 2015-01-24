#include "InputSystem.h"

#include "InputSystem.h"
#include <Windows.h>
#include <WindowsX.h>
#include "MouseState.h"
#include "KeyboardState.h"
#include "Events/Events.h"
#include "Utils/Memory/Memory.h"
#include "Application/EngineStatics.h"

using novus::InputSystem;
using novus::KeyboardState;
using novus::MouseState;

InputSystem::InputSystem()
{
	//mpHydraManager = NE_NEW HydraManager();
	//mpHydraManager->Initialize();
}

InputSystem::~InputSystem()
{
	//NE_DELETE(mpHydraManager);
}

void InputSystem::Update(float dt)
{
	//mpHydraManager->Update(dt);
	mPreviousKeyboardState = mKeyboardState;
	mPreviousMouseState = mMouseState;
}

bool InputSystem::ProcessWindowsMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	MouseButton::Type button = MouseButton::None;

	switch (msg)
	{
	case WM_MOUSEMOVE:
		mMouseState.mX = GET_X_LPARAM(lParam);
		mMouseState.mY = GET_Y_LPARAM(lParam);
		EnqueueEvent(IEventDataPtr(new EventData_MouseMove(mMouseState.mX, mMouseState.mY)));
		break;
	case WM_LBUTTONDOWN:
		mMouseState.mLeft = true;
		EnqueueEvent(IEventDataPtr(new EventData_MouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), MouseButton::Left)));
		break;
	case WM_LBUTTONUP:
		mMouseState.mLeft = false;
		EnqueueEvent(IEventDataPtr(new EventData_MouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), MouseButton::Left)));
		break;
	case WM_MBUTTONDOWN:
		mMouseState.mMiddle = true;
		EnqueueEvent(IEventDataPtr(new EventData_MouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), MouseButton::Middle)));
		break;
	case WM_MBUTTONUP:
		mMouseState.mMiddle = false;
		EnqueueEvent(IEventDataPtr(new EventData_MouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), MouseButton::Middle)));
		break;
	case WM_RBUTTONDOWN:
		mMouseState.mRight = true;
		EnqueueEvent(IEventDataPtr(new EventData_MouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), MouseButton::Right)));
		break;
	case WM_RBUTTONUP:
		mMouseState.mRight = false;
		EnqueueEvent(IEventDataPtr(new EventData_MouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam),  MouseButton::Right)));
		break;
	case WM_XBUTTONDOWN:
		if (GET_XBUTTON_WPARAM(wParam) == 1)
		{
			button = MouseButton::XButton1;
		}
		else if (GET_XBUTTON_WPARAM(wParam) == 2)
		{
			button = MouseButton::XButton2;
		}

		EnqueueEvent(IEventDataPtr(new EventData_MouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), button)));
		break;
	case WM_XBUTTONUP:
		if (GET_XBUTTON_WPARAM(wParam) == 1)
		{
			button = MouseButton::XButton1;
		}
		else if (GET_XBUTTON_WPARAM(wParam) == 2)
		{
			button = MouseButton::XButton2;
		}

		EnqueueEvent(IEventDataPtr(new EventData_MouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), button)));
		break;
	case WM_MOUSEWHEEL:
		mMouseState.mScroll = GET_WHEEL_DELTA_WPARAM(wParam);
		break;
	case WM_KEYDOWN:
		mKeyboardState.PressKey((KeyboardKey::Type)wParam);
		EnqueueEvent(IEventDataPtr(new EventData_KeyboardDown(wParam)));
		break;
	case WM_KEYUP:
		mKeyboardState.ReleaseKey((KeyboardKey::Type)wParam);
		EnqueueEvent(IEventDataPtr(new EventData_KeyboardUp(wParam)));
		break;
	default:
		return false;
	}

	return true;
}

void InputSystem::EnqueueEvent(novus::IEventDataPtr ev)
{
	EngineStatics::getEventSystem()->QueueEvent(ev);
}