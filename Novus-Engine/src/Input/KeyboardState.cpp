#include "KeyboardState.h"

namespace novus
{

KeyboardState::KeyboardState()
{
	mKeys.reserve(novus::KeyboardKey::KEY_COUNT);

	for (int i = 0; i < novus::KeyboardKey::KEY_COUNT; i++)
		mKeys.push_back(false);
}

KeyboardState::~KeyboardState()
{
}

bool KeyboardState::IsKeyPressed(novus::KeyboardKey::Type key) const
{
	//return ((mKeys[key / 8] >> (key % 8)) & 1) != 0;
	if (key < KeyboardKey::KEY_COUNT)
		return mKeys[key];
	else
		return false;
}

void KeyboardState::PressKey(novus::KeyboardKey::Type key)
{
	//mKeys[key / 8] |= (1 << (key % 8));
	if (key < KeyboardKey::KEY_COUNT)
		mKeys[key] = true;
}

void KeyboardState::ReleaseKey(novus::KeyboardKey::Type key)
{
	//mKeys[key / 8] &= ~(1 << (key % 8));
	if (key < KeyboardKey::KEY_COUNT)
		mKeys[key] = false;
}

}//namespace novus