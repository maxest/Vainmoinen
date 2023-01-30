#ifndef MATH_VECTOR_H
#define MATH_VECTOR_H

#include "constants.hpp"

#include <cmath>
#include <ostream>

using namespace std;



class vec2;
class vec3;
class vec4;

class mtx;



// ----------------------------------------------------------------------------



class vec2
{
public:
	vec2() {}

	vec2(float xy)
	{
		x = xy;
		y = xy;
	}

	vec2(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	vec2(const vec2& v)
	{
		x = v.x;
		y = v.y;
	}

	explicit vec2(const vec3& v);

	explicit vec2(const vec4& v);

	void setLength(float length)
	{
		vec2 temp(x, y);
		temp.normalize();

		x = temp.x * length;
		y = temp.y * length;
	}

	float getLength() const
	{
		return sqrtf(*this % *this);
	}

	void normalize()
	{
		float oneOverLength = 1.0f / getLength();

		x *= oneOverLength;
		y *= oneOverLength;
	}

	vec2 getNormalized() const
	{
		vec2 temp(x, y);
		temp.normalize();
		return temp;
	}

	vec2& operator = (const vec2& v)
	{
		x = v.x;
		y = v.y;

		return *this;
	}

	bool operator == (const vec2& v) const
	{
		return (
			(fabs(x - v.x) < epsilon) &&
			(fabs(y - v.y) < epsilon)
			);
	}

	bool operator != (const vec2& v) const
	{
		return !(*this == v);
	}

	// dot product
	float operator % (const vec2& v) const
	{
		return (x*v.x + y*v.y);
	}

	vec2& operator += (const vec2& v)
	{
		x += v.x;
		y += v.y;

		return *this;
	}

	vec2 operator + (const vec2& v) const
	{
		vec2 temp(*this);
		temp += v;
		return temp;
	}

	vec2 operator - () const
	{
		return vec2(-x, -y);
	}

	vec2& operator -= (const vec2& v)
	{
		x -= v.x;
		y -= v.y;

		return *this;
	}

	vec2 operator - (const vec2& v) const
	{
		vec2 temp(*this);
		temp -= v;
		return temp;
	}

	vec2& operator *= (const vec2& v)
	{
		x *= v.x;
		y *= v.y;

		return *this;
	}

	vec2 operator * (const vec2& v) const
	{
		vec2 temp(*this);
		temp *= v;
		return temp;
	}

	vec2& operator *= (float s)
	{
		x *= s;
		y *= s;

		return *this;
	}

	vec2 operator * (float s) const
	{
		vec2 temp(*this);
		temp *= s;
		return temp;
	}

	friend vec2 operator * (float s, const vec2& v)
	{
		return v * s;
	}

public:
	union
	{
		struct
		{
			float x, y;
		};

		float _[2];
	};
};



inline float getDistanceBetweenPoints(const vec2& v1, const vec2& v2)
{
    float dx = v1.x - v2.x;
    float dy = v1.y - v2.y;

    return sqrtf(dx*dx + dy*dy);
}

inline float getAngleBetweenVectors(const vec2& v1, const vec2& v2)
{
	return acosf(v1 % v2);
}

inline vec2 getReflectedVector(const vec2& input, const vec2& normal)
{
    return (input - 2.0f*normal * (input % normal));
}

inline vec2 slerp(const vec2& v1, const vec2& v2, float t)
{
	float omega = getAngleBetweenVectors(v1, v2);

	float s1 = sinf((1.0f - t)*omega);
	float s2 = sinf(t*omega);
	float s3 = sinf(omega);

	return s1/s3 * v1 + s2/s3 * v2;
}

inline ostream & operator << (ostream &stream, const vec2& v)
{
	stream << "[" << v.x << ", " << v.y << "]";
	return stream;
}



// ----------------------------------------------------------------------------



class vec3
{
public:
	vec3() {}

	vec3(float xyz)
	{
		x = xyz;
		y = xyz;
		z = xyz;
	}

	vec3(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	vec3(const vec3& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}

	explicit vec3(const vec2& v);

	explicit vec3(const vec4& v);

	void setLength(float length)
	{
		vec3 temp(x, y, z);
		temp.normalize();

		x = temp.x * length;
		y = temp.y * length;
		z = temp.z * length;
	}

	float getLength() const
	{
		return sqrtf(*this % *this);
	}

	void normalize()
	{
		float oneOverLength = 1.0f / getLength();

		x *= oneOverLength;
		y *= oneOverLength;
		z *= oneOverLength;
	}

	vec3 getNormalized() const
	{
		vec3 temp(x, y, z);
		temp.normalize();
		return temp;
	}

	vec3& operator = (const vec3& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;

		return *this;
	}

	bool operator == (const vec3& v) const
	{
		return ( 
			(fabs(x - v.x) < epsilon) &&
			(fabs(y - v.y) < epsilon) &&
			(fabs(z - v.z) < epsilon)
			);
	}

	bool operator != (const vec3& v) const
	{
		return !(*this == v);
	}

	// dot product
	float operator % (const vec3& v) const
	{
		return (x*v.x + y*v.y + z*v.z);
	}

	// cross product
	vec3 operator ^ (const vec3& v) const
	{
		return vec3(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
	}

	vec3& operator += (const vec3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;

		return *this;
	}

	vec3 operator + (const vec3& v) const
	{
		vec3 temp(*this);
		temp += v;
		return temp;
	}

	vec3 operator - () const
	{
		return vec3(-x, -y, -z);
	}

	vec3& operator -= (const vec3& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;

		return *this;
	}

	vec3 operator - (const vec3& v) const
	{
		vec3 temp(*this);
		temp -= v;
		return temp;
	}

	vec3& operator *= (const vec3& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;

		return *this;
	}

	vec3 operator * (const vec3& v) const
	{
		vec3 temp(*this);
		temp *= v;
		return temp;
	}

	vec3& operator *= (float s)
	{
		x *= s;
		y *= s;
		z *= s;

		return *this;
	}

	vec3 operator * (float s) const
	{
		vec3 temp(*this);
		temp *= s;
		return temp;
	}

	friend vec3 operator * (float s, const vec3& v)
	{
		return v * s;
	}

	vec3 operator * (const mtx& m) const;
	vec3& operator *= (const mtx& m);

public:
	union
	{
		struct
		{
			float x, y, z;
		};

		float _[3];
	};
};



inline float getDistanceBetweenPoints(const vec3& v1, const vec3& v2)
{
    float dx = v1.x - v2.x;
    float dy = v1.y - v2.y;
    float dz = v1.z - v2.z;

    return sqrtf(dx*dx + dy*dy + dz*dz);
}

inline float getAngleBetweenVectors(const vec3& v1, const vec3& v2)
{
	return acosf(v1 % v2);
}

inline vec3 getReflectedVector(const vec3& input, const vec3& normal)
{
    return (input - 2.0f*normal * (input % normal));
}

inline vec3 slerp(const vec3& v1, const vec3& v2, float t)
{
	float omega = getAngleBetweenVectors(v1, v2);

	float s1 = sinf((1.0f - t)*omega);
	float s2 = sinf(t*omega);
	float s3 = sinf(omega);

	return s1/s3 * v1 + s2/s3 * v2;
}

inline ostream & operator << (ostream &stream, const vec3& v)
{
	stream << "[" << v.x << ", " << v.y << ", " << v.z << "]";
	return stream;
}



// ----------------------------------------------------------------------------



class vec4
{
public:
	vec4() {}

	vec4(float xyzw)
	{
		x = xyzw;
		y = xyzw;
		z = xyzw;
		w = xyzw;
	}

	vec4(float xyz, float w = 1.0f)
	{
		this->x = xyz;
		this->y = xyz;
		this->z = xyz;
		this->w = w;
	}

	vec4(float x, float y, float z, float w = 1.0f)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	vec4(const vec4& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
	}

	explicit vec4(const vec2& v);

	explicit vec4(const vec3& v);

	void divideByW()
	{
		float oneOverW = 1.0f / w;
		x *= oneOverW;
		y *= oneOverW;
		z *= oneOverW;
		w = 1.0f;
	}

	vec4 getDividedByW() const
	{
		float oneOverW = 1.0f / w;
		return vec4(x*oneOverW, y*oneOverW, z*oneOverW, 1.0f);
	}

	vec4& operator = (const vec4& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;

		return *this;
	}

	bool operator == (const vec4& v) const
	{
		return ( 
			(fabs(x - v.x) < epsilon) &&
			(fabs(y - v.y) < epsilon) &&
			(fabs(z - v.z) < epsilon) &&
			(fabs(w - v.w) < epsilon)
			);
	}

	bool operator != (const vec4& v) const
	{
		return !(*this == v);
	}

	// dot product
	float operator % (const vec4& v) const
	{
		return (x*v.x + y*v.y + z*v.z + w*v.w);
	}

	vec4& operator += (const vec4& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;

		return *this;
	}

	vec4 operator + (const vec4& v) const
	{
		vec4 temp(*this);
		temp += v;
		return temp;
	}

	vec4 operator - () const
	{
		return vec4(-x, -y, -z, -w);
	}

	vec4& operator -= (const vec4& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;

		return *this;
	}

	vec4 operator - (const vec4& v) const
	{
		vec4 temp(*this);
		temp -= v;
		return temp;
	}

	vec4& operator *= (const vec4& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		w *= v.w;

		return *this;
	}

	vec4 operator * (const vec4& v) const
	{
		vec4 temp(*this);
		temp *= v;
		return temp;
	}

	vec4& operator *= (float s)
	{
		x *= s;
		y *= s;
		z *= s;
		w *= s;

		return *this;
	}

	vec4 operator * (float s) const
	{
		vec4 temp(*this);
		temp *= s;
		return temp;
	}

	friend vec4 operator * (float s, const vec4& v)
	{
		return v * s;
	}

	vec4 operator * (const mtx& m) const;
	vec4& operator *= (const mtx& m);

public:
	union
	{
		struct
		{
			float x, y, z, w;
		};

		float _[4];
	};
};



inline ostream & operator << (ostream &stream, const vec4& v)
{
	stream << "[" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << "]";
	return stream;
}



// ----------------------------------------------------------------------------



#endif
