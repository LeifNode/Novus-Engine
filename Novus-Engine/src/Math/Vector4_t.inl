namespace novus
{
	template <typename T>
	Vector4_t<T>::Vector4_t()
		: x(0), y(0), z(0), w(0)
	{}


	template <typename T>
	Vector4_t<T>::Vector4_t(const Vector4_t<T>& v)
		: x(v.x), y(v.y), z(v.z), w(v.w)
	{}

	template <typename T>
	Vector4_t<T>::Vector4_t(const T& s)
		: x(s), y(s), z(s), w(w)
	{}

	template <typename T>
	Vector4_t<T>::Vector4_t(const T& a, const T& b, const T& c, const T& d)
		: x(a), y(b), z(c), w(d)
	{}

	//Conversion constructors
	template <typename T>
	template <typename B>
	Vector4_t<T>::Vector4_t(const Vector2_t<B>& v)
		: x(static_cast<T>(v.x)), y(static_cast<T>(v.y)), z(0), w(0)
	{}

	template <typename T>
	template <typename B>
	Vector4_t<T>::Vector4_t(const Vector3_t<B>& v)
		: x(static_cast<T>(v.x)), y(static_cast<T>(v.y)), z(static_cast<T>(v.z)), w(0)
	{}

	template <typename T>
	template <typename B>
	Vector4_t<T>::Vector4_t(const Vector4_t<B>& v)
		: x(static_cast<T>(v.x)), y(static_cast<T>(v.y)), z(static_cast<T>(v.z)), w(static_cast<T>(v.w))
	{}
	
	template <typename T>
	template <typename A, typename B, typename C, typename D>
	Vector4_t<T>::Vector4_t(const A& a, const B& b, const C& c, const D& d)
		: x(static_cast<T>(a)), y(static_cast<T>(b)), z(static_cast<T>(c)), w(static_cast<T>(d))
	{}

	template <typename T>
	template <typename A, typename B, typename C>
	Vector4_t<T>::Vector4_t(const Vector2_t<A>& a, const B& b, const C& c)
		: x(static_cast<T>(a.x)), y(static_cast<T>(a.y)), z(static_cast<T>(b)), w(static_cast<T>(c))
	{}

	template <typename T>
	template <typename A, typename B, typename C>
	Vector4_t<T>::Vector4_t(const A& a, const B& b, const Vector2_t<C>& c)
		: x(static_cast<T>(a)), y(static_cast<T>(b)), z(static_cast<T>(c.x)), w(static_cast<T>(c.y))
	{}

	template <typename T>
	template <typename A, typename B, typename C>
	Vector4_t<T>::Vector4_t(const A& a, const Vector2_t<B>& b, const C& c)
		: x(static_cast<T>(a)), y(static_cast<T>(b.x)), z(static_cast<T>(b.y)), w(static_cast<T>(c))
	{}

	template <typename T>
	template <typename A, typename B>
	Vector4_t<T>::Vector4_t(const Vector2_t<A>& a, const Vector2_t<B>& b)
		: x(static_cast<T>(a.x)), y(static_cast<T>(a.y)), z(static_cast<T>(b.x)), w(static_cast<T>(b.y))
	{}

	template <typename T>
	template <typename A, typename B>
	Vector4_t<T>::Vector4_t(const Vector3_t<A>& a, const B& b)
		: x(static_cast<T>(a.x)), y(static_cast<T>(a.y)), z(static_cast<T>(a.z)), w(static_cast<T>(b))
	{}

	template <typename T>
	template <typename A, typename B>
	Vector4_t<T>::Vector4_t(const A& a, const Vector3_t<B>& b)
		: x(static_cast<T>(a)), y(static_cast<T>(b.x)), z(static_cast<T>(b.y)), w(static_cast<T>(b.z))
	{}

	template <typename T>
	size_t Vector4_t<T>::size() const
	{
		return 4;
	}

	template <typename T>
	T& Vector4_t<T>::operator[] (size_t i)
	{
		assert(i < size());

		return (&x)[i];
	}

	template <typename T>
	const T& Vector4_t<T>::operator[] (size_t i) const
	{
		assert(i < size());

		return (&x)[i];
	}

	//Arithmetic operators
	template <typename T>
	template <typename U>
	Vector4_t<T>& Vector4_t<T>::operator= (const Vector4_t<U>& v)
	{
		this->x = static_cast<T>(v.x);
		this->y = static_cast<T>(v.y);
		this->z = static_cast<T>(v.z);
		this->w = static_cast<T>(v.w);
		return *this;
	}

	template <typename T>
	template <typename U>
	Vector4_t<T>& Vector4_t<T>::operator+= (const Vector4_t<U>& v)
	{
		this->x += static_cast<T>(v.x);
		this->y += static_cast<T>(v.y);
		this->z += static_cast<T>(v.z);
		this->w += static_cast<T>(v.w);
		return *this;
	}

	template <typename T>
	template <typename U>
	Vector4_t<T>& Vector4_t<T>::operator-= (const Vector4_t<U>& v)
	{
		this->x -= static_cast<T>(v.x);
		this->y -= static_cast<T>(v.y);
		this->z -= static_cast<T>(v.z);
		this->w -= static_cast<T>(v.w);
		return *this;
	}

	template <typename T>
	template <typename U>
	Vector4_t<T>& Vector4_t<T>::operator*= (const Vector4_t<U>& v)
	{
		this->x *= static_cast<T>(v.x);
		this->y *= static_cast<T>(v.y);
		this->z *= static_cast<T>(v.z);
		this->w *= static_cast<T>(v.w);
		return *this;
	}

	template <typename T>
	template <typename U>
	Vector4_t<T>& Vector4_t<T>::operator/= (const Vector4_t<U>& v)
	{
		this->x /= static_cast<T>(v.x);
		this->y /= static_cast<T>(v.y);
		this->z /= static_cast<T>(v.z);
		this->w /= static_cast<T>(v.w);
		return *this;
	}


	template <typename T>
	template <typename U>
	Vector4_t<T>& Vector4_t<T>::operator+= (const U& s)
	{
		this->x += static_cast<T>(s);
		this->y += static_cast<T>(s);
		this->z += static_cast<T>(s);
		this->w += static_cast<T>(s);
		return *this;
	}
	
	template <typename T>
	template <typename U>
	Vector4_t<T>& Vector4_t<T>::operator-= (const U& s)
	{
		this->x -= static_cast<T>(s);
		this->y -= static_cast<T>(s);
		this->z -= static_cast<T>(s);
		this->w -= static_cast<T>(s);
		return *this;
	}
	
	template <typename T>
	template <typename U>
	Vector4_t<T>& Vector4_t<T>::operator*= (const U& s)
	{
		this->x *= static_cast<T>(s);
		this->y *= static_cast<T>(s);
		this->z *= static_cast<T>(s);
		this->w *= static_cast<T>(s);
		return *this;
	}

	template <typename T>
	template <typename U>
	Vector4_t<T>& Vector4_t<T>::operator/= (const U& s)
	{
		this->x /= static_cast<T>(s);
		this->y /= static_cast<T>(s);
		this->z /= static_cast<T>(s);
		this->w /= static_cast<T>(s);
		return *this;
	}

	template <typename T>
	Vector4_t<T> operator+ (const Vector4_t<T>& a, const Vector4_t<T>& b)
	{
		return Vector4_t<T>(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
	}

	template <typename T>
	Vector4_t<T> operator- (const Vector4_t<T>& a, const Vector4_t<T>& b)
	{
		return Vector4_t<T>(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
	}

	template <typename T>
	Vector4_t<T> operator* (const Vector4_t<T>& a, const Vector4_t<T>& b)
	{
		return Vector4_t<T>(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
	}

	template <typename T>
	Vector4_t<T> operator/ (const Vector4_t<T>& a, const Vector4_t<T>& b)
	{
		return Vector4_t<T>(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
	}

	template <typename T>
	Vector4_t<T> operator+ (const Vector4_t<T>& v, const T& s)
	{
		return Vector4_t<T>(v.x + s, v.y + s, v.z + s, v.w + s);
	}

	template <typename T>
	Vector4_t<T> operator- (const Vector4_t<T>& v, const T& s)
	{
		return Vector4_t<T>(v.x - s, v.y - s, v.z - s, v.w - s);
	}

	template <typename T>
	Vector4_t<T> operator* (const Vector4_t<T>& v, const T& s)
	{
		return Vector4_t<T>(v.x * s, v.y * s, v.z * s, v.w * s);
	}

	template <typename T>
	Vector4_t<T> operator/ (const Vector4_t<T>& v, const T& s)
	{
		return Vector4_t<T>(v.x / s, v.y / s, v.z / s, v.w / s);
	}

	template <typename T>
	Vector4_t<T> operator+ (const T& s, const Vector4_t<T>& v)
	{
		return Vector4_t<T>(s + v.x, s + v.y, s + v.z, s + v.w);
	}

	template <typename T>
	Vector4_t<T> operator- (const T& s, const Vector4_t<T>& v)
	{
		return Vector4_t<T>(s - v.x, s - v.y, s - v.z, s - v.w);
	}

	template <typename T>
	Vector4_t<T> operator* (const T& s, const Vector4_t<T>& v)
	{
		return Vector4_t<T>(s * v.x, s * v.y, s * v.z, s * v.w);
	}

	template <typename T>
	Vector4_t<T> operator/ (const T& s, const Vector4_t<T>& v)
	{
		return Vector4_t<T>(s / v.x, s / v.y, s / v.z, s / v.w);
	}

	template <typename T>
	Vector4_t<T> operator- (const Vector4_t<T>& v)
	{
		return Vector4_t<T>(-v.x, -v.y, -v.z, -v.w);
	}

	template <typename T>
	bool operator== (const Vector4_t<T>& v1, const Vector4_t<T>& v2)
	{
		return (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z && v1.w == v2.w);
	}

	template <typename T>
	bool operator!= (const Vector4_t<T>& v1, const Vector4_t<T>& v2)
	{
		return (v1.x != v2.x || v1.y != v2.y || v1.z != v2.z || v1.w != v2.w);
	}
}