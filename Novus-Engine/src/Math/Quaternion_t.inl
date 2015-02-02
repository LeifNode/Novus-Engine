#include "Quaternion_t.h"
#include "Vector3_t.h"
#include "Vector4_t.h"

namespace novus
{
	template <typename T>
	Quaternion_t<T>::Quaternion_t()
		: x(0), y(0), z(0), w(1)
	{}

	template <typename T>
	Quaternion_t<T>::Quaternion_t(const T& a, const T& b, const T& c, const T& d)
		: x(a), y(b), z(c), w(d)
	{}

	template <typename T>
	template <typename B>
	Quaternion_t<T>::Quaternion_t(const Quaternion_t<B>& q)
		: x(static_cast<T>(q.x)), y(static_cast<T>(q.y)), z(static_cast<T>(q.z)), w(static_cast<T>(q.w))
	{}

	template <typename T>
	template <typename B>
	Quaternion_t<T>::Quaternion_t(const Vector4_t<B>& v)
		: x(static_cast<T>(v.x)), y(static_cast<T>(v.y)), z(static_cast<T>(v.z)), w(static_cast<T>(v.w))
	{}

	template <typename T>
	Quaternion_t<T> operator- (const Quaternion_t<T>& q)
	{
		return Quaternion_t<T>(-q.x, -q.y, -q.z, -q.w);
	}

	template <typename T>
	Quaternion_t<T> operator+ (const Quaternion_t<T>& q1, const Quaternion_t<T>& q2)
	{
		return Quaternion_t<T>(q1.x + q2.x, q1.y + q2.y, q1.z + q2.z, q1.w + q2.w);
	}

	template <typename T>
	Quaternion_t<T> operator- (const Quaternion_t<T>& q1, const Quaternion_t<T>& q2)
	{
		return Quaternion_t<T>(q1.x - q2.x, q1.y - q2.y, q1.z - q2.z, q1.w - q2.w);
	}

	template <typename T>
	Quaternion_t<T> operator* (const Quaternion_t<T>& q1, const Quaternion_t<T>& q2)
	{
		return Quaternion_t<T>(
			q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
			q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x, 
			q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w, 
			q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z);
	}

	template <typename T>
	Quaternion_t<T> operator* (const Quaternion_t<T>& q, const T& s)
	{
		return Quaternion_t<T>(q.x * s, q.y * s, q.z * s, q.w * s);
	}

	template <typename T>
	Quaternion_t<T> operator* (const T& s, const Quaternion_t<T>& q)
	{
		return Quaternion_t<T>(q.x * s, q.y * s, q.z * s, q.w * s);
	}

	template <typename T>
	Quaternion_t<T> operator/ (const Quaternion_t<T>& q, const T& s)
	{
		return Quaternion_t<T>(q.x / s, q.y / s, q.z / s, q.w / s);
	}

	template <typename T>
	T Quaternion_t<T>::Magnitude(const Quaternion_t<T>& q)
	{
		return sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
	}

	template <typename T>
	Quaternion_t<T> Quaternion_t<T>::Normalize(const Quaternion_t<T>& q)
	{
		return q / sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
	}

	template <typename T>
	T Quaternion_t<T>::Dot(const Quaternion_t<T>& q1, const Quaternion_t<T>& q2)
	{
		return q1.x * q2.x + q1.y *q2.y + q1.z * q2.z + q1.w * q2.w;
	}

	template <typename T>
	Quaternion_t<T> Quaternion_t<T>::AxisAngle(const Vector3_t<T>& axis, const T& radians)
	{
		T radOver2 = radians / 2;
		T radSin = sin(radOver2);

		return Quaternion_t<T>(
			axis.x * radSin,
			axis.y * radSin,
			axis.z * radSin,
			cos(radOver2));
	}

	template <typename T>
	Matrix4x4_t<T> Quaternion_t<T>::ToMatrix(const Quaternion_t<T>& q)
	{
		return Matrix4x4_t<T>(
			1 - 2*q.y*q.y - 2*q.z*q.z, 2*q.x*q.y - 2*q.w*q.z, 2*q.x*q.z + 2*q.w*q.y, 0,
			2*q.x*q.y + 2*q.w*q.z, 1 - 2*q.x*q.x - 2*q.z*q.z, 2*q.y*q.z - 2*q.w*q.x, 0,
			2*q.x*q.z - 2*q.w*q.y, 2*q.y*q.z + 2*q.w*q.x, 1 - 2*q.x*q.x - 2*q.y*q.y, 0,
			0, 0, 0, 1);
	}

	template <typename T>
	Quaternion_t<T> Quaternion_t<T>::Slerp(const Quaternion_t<T>& q1, const Quaternion_t<T>& q2, const T& t)
	{
		T cosAngle = Dot(q1, q2);
		T sinAngle = sqrt(1 - cosAngle * cosAngle);

		return (sinAngle * (1 - t) / sinAngle) * q1 + (sinAngle * t / sinAngle) * q2;
	}
}