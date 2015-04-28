#pragma once

#ifndef NOVUS_VECTOR3_T_INL
#define NOVUS_VECTOR3_T_INL

#include <cassert>
#include "Vector_t.h"
#include "Vector3_t.h"

namespace novus
{
	template <typename T>
	Vector3_t<T>::Vector3_t()
		: x(0), y(0), z(0)
	{}

	template <typename T>
	Vector3_t<T>::Vector3_t(const Vector3_t<T>& v)
		: x(v.x), y(v.y), z(v.z)
	{}

	template <typename T>
	Vector3_t<T>::Vector3_t(const T& s)
		: x(s), y(s), z(s)
	{}

	template <typename T>
	Vector3_t<T>::Vector3_t(const T& a, const T& b, const T& c)
		: x(a), y(b), z(c)
	{}

	template <typename T>
	template <typename B>
	Vector3_t<T>::Vector3_t(const Vector2_t<B>& v)
		: x(static_cast<T>(v.x)), y(static_cast<T>(v.y)), z(0)
	{}

	template <typename T>
	template <typename B>
	Vector3_t<T>::Vector3_t(const Vector3_t<B>& v)
		: x(static_cast<T>(v.x)), y(static_cast<T>(v.y)), z(static_cast<T>(v.z))
	{}

	template <typename T>
	template <typename B>
	Vector3_t<T>::Vector3_t(const Vector4_t<B>& v)
		: x(v.x), y(v.y), z(v.z)
	{}

	template <typename T>
	template <typename A, typename B, typename C>
	Vector3_t<T>::Vector3_t(const A& a, const B& b, const C& c)
		: x(static_cast<T>(a)), y(static_cast<T>(b)), z(static_cast<T>(c))
	{}

	template <typename T>
	template <typename A, typename B>
	Vector3_t<T>::Vector3_t(const Vector2_t<A>& a, const B& b)
		: x(static_cast<T>(a.x)), y(static_cast<T>(a.y)), z(static_cast<T>(b))
	{}

	template <typename T>
	template <typename A, typename B>
	Vector3_t<T>::Vector3_t(const A& a, const Vector2_t<B>& b)
		: x(static_cast<T>(a)), y(static_cast<T>(b.x)), z(static_cast<T>(b.y))
	{}

	template <typename T>
	size_t Vector3_t<T>::size() const
	{
		return 3;
	}

	template <typename T>
	T& Vector3_t<T>::operator[] (size_t i)
	{
		assert(i < this->size());

		return (&x)[i];
	}

	template <typename T>
	const T& Vector3_t<T>::operator[] (size_t i) const
	{
		assert(i < this->size());

		return (&x)[i];
	}

	//Arithmetic operators
	template <typename T>
	template <typename U>
	Vector3_t<T>& Vector3_t<T>::operator= (const Vector3_t<U>& v)
	{
		this->x = static_cast<T>(v.x);
		this->y = static_cast<T>(v.y);
		this->z = static_cast<T>(v.z);
		return *this;
	}

	template <typename T>
	template <typename U>
	Vector3_t<T>& Vector3_t<T>::operator+= (const Vector3_t<U>& v)
	{
		this->x += static_cast<T>(v.x);
		this->y += static_cast<T>(v.y);
		this->z += static_cast<T>(v.z);
		return *this;
	}

	template <typename T>
	template <typename U>
	Vector3_t<T>& Vector3_t<T>::operator-= (const Vector3_t<U>& v)
	{
		this->x -= static_cast<T>(v.x);
		this->y -= static_cast<T>(v.y);
		this->z -= static_cast<T>(v.z);
		return *this;
	}

	template <typename T>
	template <typename U>
	Vector3_t<T>& Vector3_t<T>::operator*= (const Vector3_t<U>& v)
	{
		this->x *= static_cast<T>(v.x);
		this->y *= static_cast<T>(v.y);
		this->z *= static_cast<T>(v.z);
		return *this;
	}

	template <typename T>
	template <typename U>
	Vector3_t<T>& Vector3_t<T>::operator/= (const Vector3_t<U>& v)
	{
		this->x /= static_cast<T>(v.x);
		this->y /= static_cast<T>(v.y);
		this->z /= static_cast<T>(v.z);
		return *this;
	}

	template <typename T>
	template <typename U>
	Vector3_t<T>& Vector3_t<T>::operator+= (const U& s)
	{
		this->x += static_cast<T>(s);
		this->y += static_cast<T>(s);
		this->z += static_cast<T>(s);
		return *this;
	}

	template <typename T>
	template <typename U>
	Vector3_t<T>& Vector3_t<T>::operator-= (const U& s)
	{
		this->x -= static_cast<T>(s);
		this->y -= static_cast<T>(s);
		this->z -= static_cast<T>(s);
		return *this;
	}

	template <typename T>
	template <typename U>
	Vector3_t<T>& Vector3_t<T>::operator*= (const U& s)
	{
		this->x *= static_cast<T>(s);
		this->y *= static_cast<T>(s);
		this->z *= static_cast<T>(s);
		return *this;
	}

	template <typename T>
	template <typename U>
	Vector3_t<T>& Vector3_t<T>::operator/= (const U& s)
	{
		this->x /= static_cast<T>(s);
		this->y /= static_cast<T>(s);
		this->z /= static_cast<T>(s);
		return *this;
	}

	template <typename T>
	Vector3_t<T> operator+ (const Vector3_t<T>& a, const Vector3_t<T>& b)
	{
		return Vector3_t<T>(a.x + b.x, a.y + b.y, a.z + b.z);
	}

	template <typename T>
	Vector3_t<T> operator- (const Vector3_t<T>& a, const Vector3_t<T>& b)
	{
		return Vector3_t<T>(a.x - b.x, a.y - b.y, a.z - b.z);
	}

	template <typename T>
	Vector3_t<T> operator* (const Vector3_t<T>& a, const Vector3_t<T>& b)
	{
		return Vector3_t<T>(a.x * b.x, a.y * b.y, a.z * b.z);
	}

	template <typename T>
	Vector3_t<T> operator/ (const Vector3_t<T>& a, const Vector3_t<T>& b)
	{
		return Vector3_t<T>(a.x / b.x, a.y / b.y, a.z / b.z);
	}

	template <typename T>
	Vector3_t<T> operator+ (const T& s, const Vector3_t<T>& v)
	{
		return Vector3_t<T>(s + v.x, s + v.y, s + v.z);
	}

	template <typename T>
	Vector3_t<T> operator- (const T& s, const Vector3_t<T>& v)
	{
		return Vector3_t<T>(s - v.x, s - v.y, s - v.z);
	}

	template <typename T>
	Vector3_t<T> operator* (const T& s, const Vector3_t<T>& v)
	{
		return Vector3_t<T>(s * v.x, s * v.y, s * v.z);
	}

	template <typename T>
	Vector3_t<T> operator/ (const T& s, const Vector3_t<T>& v)
	{
		return Vector3_t<T>(s / v.x, s / v.y, s / v.z);
	}

	template <typename T>
	Vector3_t<T> operator+ (const Vector3_t<T>& v, const T& s)
	{
		return Vector3_t<T>(v.x + s, v.y + s, v.z + s);
	}

	template <typename T>
	Vector3_t<T> operator- (const Vector3_t<T>& v, const T& s)
	{
		return Vector3_t<T>(v.x - s, v.y - s, v.z - s);
	}

	template <typename T>
	Vector3_t<T> operator* (const Vector3_t<T>& v, const T& s)
	{
		return Vector3_t<T>(v.x * s, v.y * s, v.z * s);
	}

	template <typename T>
	Vector3_t<T> operator/ (const Vector3_t<T>& v, const T& s)
	{
		return Vector3_t<T>(v.x / s, v.y / s, v.z / s);
	}

	template <typename T>
	Vector3_t<T> operator- (const Vector3_t<T>& v)
	{
		return Vector3_t<T>(-v.x, -v.y, -v.z);
	}

	template <typename T>
	bool operator== (const Vector3_t<T>& v1, const Vector3_t<T>& v2)
	{
		return (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z);
	}

	template <typename T>
	bool operator!= (const Vector3_t<T>& v1, const Vector3_t<T>& v2)
	{
		return (v1.x != v2.x || v1.y != v2.y || v1.z != v2.z);
	}

	template <typename T>
	T Length(const Vector3_t<T>& v)
	{
		return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	}

	template <typename T>
	T LengthSq(const Vector3_t<T>& v)
	{
		return v.x * v.x + v.y * v.y + v.z * v.z;
	}

	template <typename T>
	Vector3_t<T> Normalize(const Vector3_t<T>& v)
	{
		T length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);

		return v / length;
	}

	template <typename T>
	T Dot(const Vector3_t<T>& v1, const Vector3_t<T>& v2)
	{
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	template <typename T>
	Vector3_t<T> Cross(const Vector3_t<T>& v1, const Vector3_t<T>& v2)
	{
		return Vector3_t<T>(v1.y * v2.z - v1.z * v2.y,
							v1.z * v2.x - v1.x * v2.z,
							v1.x * v2.y - v1.y * v2.x);
	}

	template <typename T>
	Vector3_t<T> Project(const Vector3_t<T>& v, const Vector3_t<T>& n)
	{
		return Dot(v, n) * n;
	}

	template <typename T>
	Vector3_t<T> Reflect(const Vector3_t<T>& v, const Vector3_t<T>& n)
	{
		return v - 2.0f * Dot(v, n) * n;
	}
}

#endif