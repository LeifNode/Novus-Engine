#include "PointList.h"

namespace novus
{

PointList::PointList()
{
}

PointList::PointList(const std::vector<Vector3>& list)
{
	setPoints(list);
}

PointList::~PointList()
{
}

void PointList::AddPoint(const Vector3& point)
{
	list.push_back(point);
}

void PointList::Clear()
{
	list.clear();
}

void PointList::setPoints(const std::vector<Vector3>& points)
{
	list.clear();

	for (auto it = points.begin(); it != points.end(); ++it)
	{
		list.push_back((*it));
	}
}

void PointList::setPoints(const std::list<Vector3>& points)
{
	list.clear();

	for (auto it = points.begin(); it != points.end(); ++it)
	{
		list.push_back((*it));
	}
}

void PointList::AddLine(const LineSegment& line)
{
	list.push_back(line.point1);
	list.push_back(line.point2);
}

PointList& PointList::operator+=(const LineSegment& line)
{
	list.push_back(line.point1);
	list.push_back(line.point2);

	return *this;
}

PointList& PointList::operator+=(const Vector3& point)
{
	list.push_back(point);

	return *this;
}

}