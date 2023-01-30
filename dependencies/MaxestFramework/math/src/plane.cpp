#include <math/plane.hpp>
#include <math/matrix.hpp>



void plane::transform(mtx transform)
{
	vec4 v(a, b, c, d);
	transform.inverse();
	transform.transpose();

	v *= transform;

	a = v.x;
	b = v.y;
	c = v.z;
	d = v.w;
	
	normalize();
}
