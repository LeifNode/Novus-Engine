//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_KEYBOARD_STATE_H
#define NOVUS_KEYBOARD_STATE_H

#include <vector>

namespace novus
{

namespace KeyboardKey
{
enum Type
{
	KEY_BACKSPACE = 8,
	KEY_TAB,
	KEY_RETURN = 13,
	KEY_SHIFT = 16,
	KEY_CONTROL,
	KEY_ALT,

	KEY_ESC = 27,

	KEY_SPACE = 32,

	KEY_LEFT = 37,
	KEY_UP,
	KEY_RIGHT,
	KEY_DOWN,

	KEY_0 = 48,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,

	KEY_A = 65,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,

	KEY_NUMPAD0 = 96,
	KEY_NUMPAD1,
	KEY_NUMPAD2,
	KEY_NUMPAD3,
	KEY_NUMPAD4,
	KEY_NUMPAD5,
	KEY_NUMPAD6,
	KEY_NUMPAD7,
	KEY_NUMPAD8,
	KEY_NUMPAD9,
	KEY_MULTIPLY,
	KEY_ADD,
	KEY_SEPARATOR,
	KEY_SUBTRACT,
	KEY_DECIMAL,
	KEY_DIVIDE,

	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,

	KEY_LSHIFT = 160,
	KEY_RSHIFT,
	KEY_LCONTROL,
	KEY_RCONTROL,

	KEY_TILDE = 192,
	KEY_COUNT,
};
}

class KeyboardState
{
	friend class InputSystem;
public:
	bool IsKeyPressed(KeyboardKey::Type key) const;

private:
	KeyboardState();
	~KeyboardState();

	void PressKey(KeyboardKey::Type key);
	void ReleaseKey(KeyboardKey::Type key);

private:
	//char mKeys[21]; //Bitmap of keys
	std::vector<bool> mKeys; //Looks like vector handles bools in the same way internally
};

}

#endif