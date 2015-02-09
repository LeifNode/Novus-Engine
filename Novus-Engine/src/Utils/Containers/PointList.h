//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_POINT_LIST_H
#define NOVUS_POINT_LIST_H

#include <vector>
#include <list>

#include "Math/Vector3_t.h"
#include "Utils/Primitives/LineSegment.h"

namespace novus
{

struct PointList
{
	PointList();
	explicit PointList(const std::vector<Vector3>& list);
	~PointList();

	void AddPoint(const Vector3& point);

	void Clear();

	void setPoints(const std::vector<Vector3>& points);
	void setPoints(const std::list<Vector3>& points);
	void AddLine(const LineSegment& line);

	PointList& operator+=(const LineSegment& line);
	PointList& operator+=(const Vector3& point);

	std::vector<Vector3> list;
};

}

#endif