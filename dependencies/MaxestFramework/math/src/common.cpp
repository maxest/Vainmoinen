#include <math/common.hpp>



// algorithm comes from Fernando's and Kilgard's "The Cg Tutorial"
void computeTangentBasisForTriangle(
	const vec3& v1, const vec2& uv1,
	const vec3& v2, const vec2& uv2,
	const vec3& v3, const vec2& uv3,
	vec3& tangent, vec3& bitangent, vec3& normal)
{
	vec3 delta1_xuv = vec3(v2.x - v1.x, uv2.x - uv1.x, uv2.y - uv1.y);
	vec3 delta2_xuv = vec3(v3.x - v1.x, uv3.x - uv1.x, uv3.y - uv1.y);
	vec3 cross_xuv = delta1_xuv ^ delta2_xuv;

	vec3 delta1_yuv = vec3(v2.y - v1.y, uv2.x - uv1.x, uv2.y - uv1.y);
	vec3 delta2_yuv = vec3(v3.y - v1.y, uv3.x - uv1.x, uv3.y - uv1.y);
	vec3 cross_yuv = delta1_yuv ^ delta2_yuv;

	vec3 delta1_zuv = vec3(v2.z - v1.z, uv2.x - uv1.x, uv2.y - uv1.y);
	vec3 delta2_zuv = vec3(v3.z - v1.z, uv3.x - uv1.x, uv3.y - uv1.y);
	vec3 cross_zuv = delta1_zuv ^ delta2_zuv;

	tangent.x = - cross_xuv.y / cross_xuv.x;
	tangent.y = - cross_yuv.y / cross_yuv.x;
	tangent.z = - cross_zuv.y / cross_zuv.x;

	bitangent.x = - cross_xuv.z / cross_xuv.x;
	bitangent.y = - cross_yuv.z / cross_yuv.x;
	bitangent.z = - cross_zuv.z / cross_zuv.x;

	normal = getNormalForTriangle(v1, v2, v3);
}



bool isPointInsideTriangle(
	const vec3& point,
	const vec3& v1,
	const vec3& v2,
	const vec3& v3)
{
	vec3 triangleNormal = getNormalForTriangle(v1, v2, v3);

	plane plane1(v1, v2, v2 + triangleNormal);
	plane plane2(v2, v3, v3 + triangleNormal);
	plane plane3(v3, v1, v1 + triangleNormal);
	
	if (plane1.getSignedDistanceFromPoint(point) <= 0.0f &&
		plane2.getSignedDistanceFromPoint(point) <= 0.0f &&
		plane3.getSignedDistanceFromPoint(point) <= 0.0f)
	{
		return true;
	}
	else
	{
		return false;
	}
}



bool doesTriangleOverlapTriangle(
	const vec3& t1_v1,
	const vec3& t1_v2,
	const vec3& t1_v3,
	const vec3& t2_v1,
	const vec3& t2_v2,
	const vec3& t2_v3)
{
	if (isPointInsideTriangle(t1_v1, t2_v1, t2_v2, t2_v3) ||
		isPointInsideTriangle(t1_v2, t2_v1, t2_v2, t2_v3) ||
		isPointInsideTriangle(t1_v3, t2_v1, t2_v2, t2_v3) ||
		isPointInsideTriangle(t2_v1, t1_v1, t1_v2, t1_v3) ||
		isPointInsideTriangle(t2_v2, t1_v1, t1_v2, t1_v3) ||
		isPointInsideTriangle(t2_v3, t1_v1, t1_v2, t1_v3))
	{
		return true;
	}
	else
	{
		return false;
	}
}



bool doesQuadOverlapQuad(
	const vec3& q1_v1,
	const vec3& q1_v2,
	const vec3& q1_v3,
	const vec3& q1_v4,
	const vec3& q2_v1,
	const vec3& q2_v2,
	const vec3& q2_v3,
	const vec3& q2_v4)
{
	if (doesTriangleOverlapTriangle(q1_v1, q1_v2, q1_v3, q2_v1, q2_v2, q2_v3) ||
		doesTriangleOverlapTriangle(q1_v1, q1_v2, q1_v3, q2_v1, q2_v3, q2_v4) ||
		doesTriangleOverlapTriangle(q1_v1, q1_v3, q1_v4, q2_v1, q2_v2, q2_v3) ||
		doesTriangleOverlapTriangle(q1_v1, q1_v3, q1_v4, q2_v1, q2_v3, q2_v4))
	{
		return true;
	}
	else
	{
		return false;
	}
}



// algorithm comes from "3D Math Primer for Graphics and Game Development", chapter 12.
void computeBarycentricWeightsForPointWithRespectToTriangle(
	const vec3& point,
	const vec3& v1,
	const vec3& v2,
	const vec3& v3,
	float& w1, float& w2, float& w3)
{
	vec3 e1 = v3 - v2;
	vec3 e2 = v1 - v3;
	vec3 e3 = v2 - v1;

	vec3 d1 = point - v1;
	vec3 d2 = point - v2;
	vec3 d3 = point - v3;

	vec3 normal = getNormalForTriangle(v1, v2, v3);

	float A_T = 0.5f * (e1 ^ e2) % normal;
	float A_T1 = 0.5f * (e1 ^ d3) % normal;
	float A_T2 = 0.5f * (e2 ^ d1) % normal;
	float A_T3 = 0.5f * (e3 ^ d2) % normal;

	w1 = A_T1 / A_T;
	w2 = A_T2 / A_T;
	w3 = A_T3 / A_T;
}



void computeCoordinateFrameForDirectionVector(
	vec3 direction,
	vec3& rightVector,
	vec3& upVector,
	bool rightHanded)
{
	direction.normalize();

	vec3 up(0.0f, 1.0f, 0.0f);
	if (fabs(direction % up) >= 1.0f - epsilon) // if direction and up are almost colinear then direction must be tilted a bit along Z axis
		direction.z = 0.01f;

	direction.normalize();

	if (rightHanded)
	{
		rightVector = (direction ^ up).getNormalized();
		upVector = (rightVector ^ direction).getNormalized();
	}
	else
	{
		rightVector = (up ^ direction).getNormalized();
		upVector = (direction ^ rightVector).getNormalized();
	}
}
