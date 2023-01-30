#include <math/vector.hpp>
#include <math/matrix.hpp>



// ----------------------------------------------------------------------------



vec2::vec2(const vec3& v)
{
	x = v.x;
	y = v.y;
}



vec2::vec2(const vec4& v)
{
	x = v.x;
	y = v.y;
}



// ----------------------------------------------------------------------------



vec3::vec3(const vec2& v)
{
	x = v.x;
	y = v.y;
	z = 0.0f;
}



vec3::vec3(const vec4& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}



vec3 vec3::operator * (const mtx& m) const
{
	vec3 temp;

	temp.x = x*m(0, 0) + y*m(1, 0) + z*m(2, 0);
	temp.y = x*m(0, 1) + y*m(1, 1) + z*m(2, 1);
	temp.z = x*m(0, 2) + y*m(1, 2) + z*m(2, 2);

	return temp;
}



vec3& vec3::operator *= (const mtx& m)
{
	*this = *this * m;
	return *this;
}



// ----------------------------------------------------------------------------



vec4::vec4(const vec2& v)
{
	x = v.x;
	y = v.y;
	z = 0.0f;
	w = 1.0f;
}



vec4::vec4(const vec3& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	w = 1.0f;
}



vec4 vec4::operator * (const mtx& m) const
{
	vec4 temp;

	temp.x = x*m(0, 0) + y*m(1, 0) + z*m(2, 0) + w*m(3, 0);
	temp.y = x*m(0, 1) + y*m(1, 1) + z*m(2, 1) + w*m(3, 1);
	temp.z = x*m(0, 2) + y*m(1, 2) + z*m(2, 2) + w*m(3, 2);
	temp.w = x*m(0, 3) + y*m(1, 3) + z*m(2, 3) + w*m(3, 3);

	return temp;
}



vec4& vec4::operator *= (const mtx& m)
{
	*this = *this * m;
	return *this;
}
