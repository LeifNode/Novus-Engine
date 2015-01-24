//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_MOUSE_STATE_H
#define NOVUS_MOUSE_STATE_H

namespace novus
{

namespace MouseButton
{

	enum Type
	{
		None,
		Left,
		Middle,
		Right,
		XButton1,
		XButton2,
	};
}

//const std::string MOUSE_BUTTON_NAMES[6] =
//{
//	"MOUSE_NONE",
//	"MOUSE_LEFT",
//	"MOUSE_MIDDLE",
//	"MOUSE_RIGHT",
//	"MOUSE_XBUTTON1",
//	"MOUSE_XBUTTON2",
//};

class MouseState
{
	friend class InputSystem;
public:
	int getX() const { return mX; }
	int getY() const { return mY; }
	bool getLeft() const { return mLeft; }
	bool getMiddle() const { return mMiddle; }
	bool getRight() const { return mRight; }
	int getScrollDelta() const { return mScroll; }

private:
	MouseState();
	~MouseState();

private:
	int mX, mY;
	bool mLeft, mMiddle, mRight;
	int mScroll;
};

}

#endif