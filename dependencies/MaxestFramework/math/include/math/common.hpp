#ifndef MATH_COMMON_H
#define MATH_COMMON_H

#include "constants.hpp"
#include "vector.hpp"
#include "plane.hpp"

#include <cstdlib>



#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))



inline bool isPowerOfTwo(int x)
{
	return !(x & (x-1));
}

inline int _round(float x)
{
	return (int)(x + 0.5f);
}

inline float _frac(float x)
{
	return x - (int)x;
}

template <class TYPE>
inline TYPE _clamp(TYPE x, TYPE a, TYPE b)
{
	return MAX(MIN(x, b), a);
}

inline float _saturate(float x)
{
	return _clamp(x, 0.0f, 1.0f);
}

template <class TYPE>
inline TYPE sqr(TYPE x)
{
	return x*x;
}

inline float _log2(float x)  
{  
	return logf(x) / logf(2.0f);  
}

inline float deg2rad(float degrees)
{
	return (degrees * (PI / 180.0f));
}

inline float rad2deg(float radians)
{
	return (radians * (180.0f / PI));
}

inline int rand(int from, int to)
{
	return from + (rand() % (to - from + 1));
}

inline float randf()
{
	return (float)rand() / (float)RAND_MAX;
}

inline float randf(float from, float to)
{
	return from + (randf() * (to - from));
}

inline float acosf_clamped(float x)
{
	if (x <= -1.0f)
		return PI;
	else if (x >= 1.0f)
		return 0.0f;
	else
		return acosf(x);
}

inline vec2 solve2x2(const vec2& p1, const vec2& p2)
{
	float a = (p1.y - p2.y) / (p1.x - p2.x);
	float b = p1.y - a*p1.x;

	return vec2(a, b);
}



inline vec3 getNormalForTriangle(
	const vec3& v1,
	const vec3& v2,
	const vec3& v3)
{
	return ( (v2 - v1) ^ (v3 - v1) ).getNormalized();
}

void computeTangentBasisForTriangle(
	const vec3& v1, const vec2& uv1,
	const vec3& v2, const vec2& uv2,
	const vec3& v3, const vec2& uv3,
	vec3& tangent, vec3& bitangent, vec3& normal);

// checks wheter a point is bounded by three planes forming "triangle"
bool isPointInsideTriangle(
	const vec3& point,
	const vec3& v1,
	const vec3& v2,
	const vec3& v3);

bool doesTriangleOverlapTriangle(
	const vec3& t1_v1,
	const vec3& t1_v2,
	const vec3& t1_v3,
	const vec3& t2_v1,
	const vec3& t2_v2,
	const vec3& t2_v3);

bool doesQuadOverlapQuad(
	const vec3& q1_v1,
	const vec3& q1_v2,
	const vec3& q1_v3,
	const vec3& q1_v4,
	const vec3& q2_v1,
	const vec3& q2_v2,
	const vec3& q2_v3,
	const vec3& q2_v4);

inline void computeIntersectionPointBetweenLineAndPlane(
	const vec3& startPoint,
	const vec3& direction,
	const plane& plane,
	vec3& intersectionPoint,
	float& t)
{
	t = - (plane.a*startPoint.x + plane.b*startPoint.y + plane.c*startPoint.z + plane.d) / 
		  (plane.a*direction.x + plane.b*direction.y + plane.c*direction.z);
	intersectionPoint = startPoint + t*direction;
}

void computeBarycentricWeightsForPointWithRespectToTriangle(
	const vec3& point,
	const vec3& v1,
	const vec3& v2,
	const vec3& v3,
	float& w1, float& w2, float& w3);

// direction
//
//     ^
//     |
//     |
//     - - - >  rightVector
//
// imagine you're looking along the direction vector
//
void computeCoordinateFrameForDirectionVector(
	vec3 direction,
	vec3& rightVector,
	vec3& upVector,
	bool rightHanded = true);

//               orientation
//
//                    ^
//                    |
//                    |
// rightVector  < - - - 
//
// imagine you're looking against the orientation vector
//
inline void computeCoordinateFrameForOrientationVector(
	vec3 orientation,
	vec3& rightVector,
	vec3& upVector,
	bool rightHanded = true)
{
	computeCoordinateFrameForDirectionVector(-orientation, rightVector, upVector, rightHanded);
}



#endif
