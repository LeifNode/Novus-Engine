//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_RECTANGLE_H
#define NOVUS_RECTANGLE_H

namespace novus
{
struct Rectangle
{
	float x;
	float y;
	float width;
	float height;

	Rectangle()
		:x(0.0f), 
		y(0.0f), 
		width(0.0f), 
		height(0.0f)
	{
	}
};
}


#endif