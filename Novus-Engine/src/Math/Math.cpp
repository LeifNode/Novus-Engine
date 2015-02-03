#include "Math.h"

namespace novus
{

const float Math::Infinity = FLT_MAX;
const float Math::Pi = 3.1415926535f;
const float Math::TwoPi = Math::Pi * 2.0f;
const float Math::PiOver2 = Math::Pi / 2.0f;
const float Math::PiOver4 = Math::Pi / 4.0f;

int Math::MipMapCount(int width, int height)
{
	int mipmaps = 0;

	while (width >= 1 && height >= 1)
	{
		mipmaps++;
		width /= 2;
		height /= 2;
	}

	return mipmaps;
}

}//namespace novus