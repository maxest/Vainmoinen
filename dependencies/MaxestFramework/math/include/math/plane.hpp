#ifndef MATH_PLANE_H
#define MATH_PLANE_H

#include "vector.hpp"

#include <cmath>



class mtx;



class plane
{
public:
	plane() {}

	plane(float a, float b, float c, float d)
	{
		this->a = a;
		this->b = b;
		this->c = c;
		this->d = d;
	}

	plane(const vec3& normal, float d = 0.0f)
	{
		this->a = normal.x;
		this->b = normal.y;
		this->c = normal.z;
		this->d = d;
	}

	plane(const vec3& point, const vec3& normal)
	{
		a = normal.x;
		b = normal.y;
		c = normal.z;
		d = -(point % normal);
	}

	plane(const vec3& point1, const vec3& point2, const vec3& point3)
	{
		vec3 v1 = point2 - point1;
		vec3 v2 = point3 - point1;
		vec3 normal = (v1 ^ v2).getNormalized();

		a = normal.x;
		b = normal.y;
		c = normal.z;
		d = -(point1 % normal);
	}

	void normalize()
	{
		float length = sqrtf(a*a + b*b + c*c);

		a /= length;
		b /= length;
		c /= length;
	}

	float getSignedDistanceFromPoint(const vec3& point) const
	{
		return a*point.x + b*point.y + c*point.z + d;
	}

	float getSignedDistanceFromPoint(const vec4& point) const
	{
		return a*point.x + b*point.y + c*point.z + d;
	}

	void transform(mtx transform);

public:
	float a, b, c;
	float d;
};



#endif
