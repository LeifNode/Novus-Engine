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
	return mKeys[key];
}

void KeyboardState::PressKey(novus::KeyboardKey::Type key)
{
	//mKeys[key / 8] |= (1 << (key % 8));
	mKeys[key] = true;
}

void KeyboardState::ReleaseKey(novus::KeyboardKey::Type key)
{
	//mKeys[key / 8] &= ~(1 << (key % 8));
	mKeys[key] = false;
}

}//namespace novus