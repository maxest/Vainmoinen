#ifndef COMMON_CAMERA_H
#define COMMON_CAMERA_H

#include <math/vector.hpp>



class CCamera
{
public:
	float horizontalAngle, verticalAngle;
	float distanceFromEyeToAt;

public:
	CCamera()
	{
		horizontalAngle = 0.0f;
		verticalAngle = 0.0f;
		distanceFromEyeToAt = 1.0f;
	}

	const vec3& getEye() const { return eye; }
	const vec3& getAt() const { return at; }
	const vec3& getUp() const { return up; }

	const vec3& getForwardVector() const { return forwardVector; }
	const vec3& getRightVector() const { return rightVector; }
	const vec3& getUpVector() const { return upVector; }

	void updateFixed(const vec3& eye, const vec3& at, const vec3& up = vec3(0.0f, 1.0f, 0.0f));
	void updateFree(const vec3& eye, const vec3& up = vec3(0.0f, 1.0f, 0.0f));
	void updateFocused(const vec3& at, const vec3& up = vec3(0.0f, 1.0f, 0.0f));

private:
	vec3 eye, at, up;
	vec3 forwardVector, rightVector, upVector;
};



#endif
