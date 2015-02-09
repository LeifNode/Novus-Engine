//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_LINE_SEGMENT_H
#define NOVUS_LINE_SEGMENT_H

#include "Math/Vector3_t.h"

namespace novus
{
struct LineSegment
{
	LineSegment();
	LineSegment(float x1, float y1, float z1, float x2, float y2, float z2);
	LineSegment(const Vector3& p1, const Vector3& p2);

	Vector3 point1;
	Vector3 point2;
};
}

#endif