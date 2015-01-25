#include "Math.h"

using namespace novus;

const float Infinity = FLT_MAX;
const float Pi = 3.1415926535f;
const float TwoPi = math::Pi * 2.0f;
const float PiOver2 = math::Pi / 2.0f;
const float PiOver4 = math::Pi / 4.0f;

int math::MipMapCount(int width, int height)
{
	int mipmaps = 0;

	while (width > 1 && height > 1)
	{
		mipmaps++;
		width /= 2;
		height /= 2;
	}

	return mipmaps;
}