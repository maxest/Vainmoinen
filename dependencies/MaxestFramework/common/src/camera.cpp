#include <common/camera.hpp>
#include <math/matrix.hpp>



void CCamera::updateFixed(const vec3& eye, const vec3& at, const vec3& up)
{
	forwardVector = (at - eye).getNormalized();

	this->eye = eye;
	this->at = at;
	this->up = up;
}



void CCamera::updateFree(const vec3& eye, const vec3& up)
{
	mtx transformMatrix = mtx::rotateX(verticalAngle) * mtx::rotateY(horizontalAngle);

	forwardVector = vec3(0.0f, 0.0f, -1.0f) * transformMatrix;
	rightVector = vec3(1.0f, 0.0f, 0.0f) * transformMatrix;
	upVector = rightVector ^ forwardVector;

	this->eye = eye;
	this->at = eye + forwardVector;
	this->up = up;
}



void CCamera::updateFocused(const vec3& at, const vec3& up)
{
	mtx transformMatrix = mtx::rotateX(verticalAngle) * mtx::rotateY(horizontalAngle);

	forwardVector = vec3(0.0f, 0.0f, -1.0f) * transformMatrix;
	rightVector = vec3(1.0f, 0.0f, 0.0f) * transformMatrix;
	upVector = rightVector ^ forwardVector;

	this->eye = at - forwardVector*distanceFromEyeToAt;
	this->at = at;
	this->up = up;
}
