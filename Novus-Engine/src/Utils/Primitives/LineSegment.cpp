#include "LineSegment.h"

namespace novus
{

LineSegment::LineSegment()
{}

LineSegment::LineSegment(float x1, float y1, float z1, float x2, float y2, float z2)
	: point1(x1, y1, z1), point2(x2, y2, z2)
{}

LineSegment::LineSegment(const Vector3& p1, const Vector3& p2)
	: point1(p1), point2(p2)
{}

}