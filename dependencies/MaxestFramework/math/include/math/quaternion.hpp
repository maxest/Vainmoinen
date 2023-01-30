#ifndef MATH_QUATERNION_H
#define MATH_QUATERNION_H

#include "common.hpp"
#include "vector.hpp"
#include "matrix.hpp"

#include <cmath>



class quat
{
public:
	quat() {}

	quat(float w, float x, float y, float z)
	{
		this->w = w;
		this->x = x;
		this->y = y;
		this->z = z;		
	}

	quat(float angle, const vec3& axis)
	{
		loadRotate(angle, axis);
	}

	float getAngle() const
	{
		return 2.0f * acosf_clamped(w);
	}

	vec3 getAxis() const
	{
		float squaredSinAngleOver2 = 1.0f - w*w;

		if (squaredSinAngleOver2 <= 0.0f)
		{
			return vec3(1.0f, 0.0f, 0.0f);
		}
		else
		{
			float oneOverSinAngleOver2 = 1.0f / (sqrtf(squaredSinAngleOver2));
			return vec3(oneOverSinAngleOver2*x, oneOverSinAngleOver2*y, oneOverSinAngleOver2*z);
		}
	}

	float getLength() const
	{
		return sqrtf(*this % *this);
	}

	void normalize()
	{
		float oneOverLength = 1.0f / getLength();

		w *= oneOverLength;
		x *= oneOverLength;
		y *= oneOverLength;
		z *= oneOverLength;
	}

	quat getNormalized() const
	{
		quat temp(w, x, y, z);
		temp.normalize();
		return temp;
	}

	quat getConjugate() const
	{
		quat temp;

		temp.w = w;
		temp.x = -x;
		temp.y = -y;
		temp.z = -z;

		return temp;
	}

	// it's simply a multiplication of angle by exponent
	void pow(float exponent)
	{
		float angleOver2 = acosf(w);
		float newAngleOver2 = angleOver2 * exponent;
		float ratio = sinf(newAngleOver2) / sinf(angleOver2);

		w = cosf(newAngleOver2);
		x *= ratio;
		y *= ratio;
		z *= ratio;
	}

	quat getPowered(float exponent) const
	{
		quat temp(w, x, y, z);
		temp.pow(exponent);
		return temp;
	}

	mtx toMatrix() const
	{
		return mtx::rotate(getAngle(), getAxis());
	}

	void loadIdentity()
	{
		w = 1.0f;
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	void loadRotate(float angle, float x, float y, float z)
	{
		angle = 0.5f * angle;
		float c = cosf(angle);
		float s = sinf(angle);

		this->w = c;
		this->x = x * s;
		this->y = y * s;
		this->z = z * s;
	}

	void loadRotate(float angle, const vec3& axis)
	{
		loadRotate(angle, axis.x, axis.y, axis.z);
	}

	void loadRotateX(float angle)
	{
		angle = 0.5f * angle;

		this->w = cosf(angle);
		this->x = sinf(angle);
		this->y = 0.0f;
		this->z = 0.0f;
	}

	void loadRotateY(float angle)
	{
		angle = 0.5f * angle;

		this->w = cosf(angle);
		this->x = 0.0f;
		this->y = sinf(angle);
		this->z = 0.0f;
	}

	void loadRotateZ(float angle)
	{
		angle = 0.5f * angle;

		this->w = cosf(angle);
		this->x = 0.0f;
		this->y = 0.0f;
		this->z = sinf(angle);
	}

	quat& operator = (const quat& q)
	{
		w = q.w;
		x = q.x;
		y = q.y;
		z = q.z;

		return *this;
	}

	bool operator == (const quat& q) const
	{
		return (
			(fabs(w - q.w) < epsilon) &&
			(fabs(x - q.x) < epsilon) &&
			(fabs(y - q.y) < epsilon) &&
			(fabs(z - q.z) < epsilon)
			);
	}

	bool operator != (const quat& q) const
	{
		return !(*this == q);
	}

	// dot product
	float operator % (const quat& q) const
	{
		return w*q.w + x*q.x + y*q.y + z*q.z;
	}

	quat operator * (const quat& q) const
	{
		quat temp;

		temp.w = w*q.w - x*q.x - y*q.y - z*q.z;
		temp.x = w*q.x + x*q.w + z*q.y - y*q.z;
		temp.y = w*q.y + y*q.w + x*q.z - z*q.x;
		temp.z = w*q.z + z*q.w + y*q.x - x*q.y;

		return temp;
	}

	quat& operator *= (const quat& q)
	{
		*this = *this * q;
		return *this;
	}

public:
	static quat identity()
	{
		quat temp;
		temp.loadIdentity();
		return temp;
	}

	static quat rotate(float angle, const vec3& axis)
	{
		quat temp;
		temp.loadRotate(angle, axis.x, axis.y, axis.z);
		return temp;
	}

	static quat rotateX(float angle)
	{
		quat temp;
		temp.loadRotateX(angle);
		return temp;
	}

	static quat rotateY(float angle)
	{
		quat temp;
		temp.loadRotateY(angle);
		return temp;
	}

	static quat rotateZ(float angle)
	{
		quat temp;
		temp.loadRotateZ(angle);
		return temp;
	}

public:
	float w;
	float x, y, z;
};



inline quat slerp(const quat& q1, const quat q2, float t)
{
	float omega = acosf(q1 % q2);

	float s1 = sinf((1.0f - t)*omega);
	float s2 = sinf(t*omega);
	float s3 = sinf(omega);
	float r1 = s1 / s3;
	float r2 = s2 / s3;

	quat temp;

	temp.w = r1 * q1.w + r2 * q2.w;
	temp.x = r1 * q1.x + r2 * q2.x;
	temp.y = r1 * q1.y + r2 * q2.y;
	temp.z = r1 * q1.z + r2 * q2.z;

	return temp;
}



#endif
