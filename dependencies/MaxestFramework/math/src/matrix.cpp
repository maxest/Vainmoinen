#include <math/matrix.hpp>

#include <cstring>
#include <cmath>



mtx::mtx(float _00_11_22_33)
{
	_[0][0] = _00_11_22_33;	_[0][1] = 0.0f;			_[0][2] = 0.0f;			_[0][3] = 0.0f;
	_[1][0] = 0.0f;			_[1][1] = _00_11_22_33;	_[1][2] = 0.0f;			_[1][3] = 0.0f;
	_[2][0] = 0.0f;			_[2][1] = 0.0f;			_[2][2] = _00_11_22_33;	_[2][3] = 0.0f;
	_[3][0] = 0.0f;			_[3][1] = 0.0f;			_[3][2] = 0.0f;			_[3][3] = _00_11_22_33;
}



mtx::mtx(float _00, float _01, float _02, float _03,
		 float _10, float _11, float _12, float _13,
		 float _20, float _21, float _22, float _23,
		 float _30, float _31, float _32, float _33)
{
	_[0][0] = _00;	_[0][1] = _01;	_[0][2] = _02;	_[0][3] = _03;
	_[1][0] = _10;	_[1][1] = _11;	_[1][2] = _12;	_[1][3] = _13;
	_[2][0] = _20;	_[2][1] = _21;	_[2][2] = _22;	_[2][3] = _23;
	_[3][0] = _30;	_[3][1] = _31;	_[3][2] = _32;	_[3][3] = _33;
}



mtx::mtx(const vec4& row1, const vec4& row2, const vec4& row3, const vec4& row4)
{
	_[0][0] = row1.x;	_[0][1] = row1.y;	_[0][2] = row1.z;	_[0][3] = row1.w;
	_[1][0] = row2.x;	_[1][1] = row2.y;	_[1][2] = row2.z;	_[1][3] = row2.w;
	_[2][0] = row3.x;	_[2][1] = row3.y;	_[2][2] = row3.z;	_[2][3] = row3.w;
	_[3][0] = row4.x;	_[3][1] = row4.y;	_[3][2] = row4.z;	_[3][3] = row4.w;
}



mtx::mtx(const mtx& m)
{
	memcpy(this, &m, 16*sizeof(float));
}



bool mtx::isOrthonormal() const
{
	vec4 row1(_[0][0], _[0][1], _[0][2], _[0][3]);
	vec4 row2(_[1][0], _[1][1], _[1][2], _[1][3]);
	vec4 row3(_[2][0], _[2][1], _[2][2], _[2][3]);
	vec4 row4(_[3][0], _[3][1], _[3][2], _[3][3]);

	if ( (row1 % row2 < epsilon) &&
		 (row1 % row3 < epsilon) &&
		 (row1 % row4 < epsilon) &&
		 (row2 % row3 < epsilon) &&
		 (row2 % row4 < epsilon) &&
		 (row3 % row4 < epsilon) )
	{
		return true;
	}
	else
	{
		return false;
	}
}



void mtx::transpose()
{
	mtx temp = *this;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			_[i][j] = temp._[j][i];
		}
	}
}



mtx mtx::getTransposed() const
{
	mtx temp = *this;
	temp.transpose();
	return temp;
}



// yeah, I know - that's horrible function! That's all Laplace's fault!
void mtx::inverse()
{
	float determinant = + _[0][0]*getDeterminant(_[1][1], _[1][2], _[1][3],
												 _[2][1], _[2][2], _[2][3],
												 _[3][1], _[3][2], _[3][3])
						- _[0][1]*getDeterminant(_[1][0], _[1][2], _[1][3],
												 _[2][0], _[2][2], _[2][3],
												 _[3][0], _[3][2], _[3][3])
						+ _[0][2]*getDeterminant(_[1][0], _[1][1], _[1][3],
												 _[2][0], _[2][1], _[2][3],
												 _[3][0], _[3][1], _[3][3])
						- _[0][3]*getDeterminant(_[1][0], _[1][1], _[1][2],
												 _[2][0], _[2][1], _[2][2],
												 _[3][0], _[3][1], _[3][2]);

	if ( !(fabs(determinant) < epsilon) )
	{
		float adj_[4][4];

		adj_[0][0] = + getDeterminant(_[1][1], _[1][2], _[1][3],
									  _[2][1], _[2][2], _[2][3],
									  _[3][1], _[3][2], _[3][3]);
		adj_[0][1] = - getDeterminant(_[1][0], _[1][2], _[1][3],
									  _[2][0], _[2][2], _[2][3],
									  _[3][0], _[3][2], _[3][3]);
		adj_[0][2] = + getDeterminant(_[1][0], _[1][1], _[1][3],
									  _[2][0], _[2][1], _[2][3],
									  _[3][0], _[3][1], _[3][3]);
		adj_[0][3] = - getDeterminant(_[1][0], _[1][1], _[1][2],
									  _[2][0], _[2][1], _[2][2],
									  _[3][0], _[3][1], _[3][2]);

		adj_[1][0] = - getDeterminant(_[0][1], _[0][2], _[0][3],
									  _[2][1], _[2][2], _[2][3],
									  _[3][1], _[3][2], _[3][3]);
		adj_[1][1] = + getDeterminant(_[0][0], _[0][2], _[0][3],
									  _[2][0], _[2][2], _[2][3],
									  _[3][0], _[3][2], _[3][3]);
		adj_[1][2] = - getDeterminant(_[0][0], _[0][1], _[0][3],
									  _[2][0], _[2][1], _[2][3],
									  _[3][0], _[3][1], _[3][3]);
		adj_[1][3] = + getDeterminant(_[0][0], _[0][1], _[0][2],
									  _[2][0], _[2][1], _[2][2],
									  _[3][0], _[3][1], _[3][2]);

		adj_[2][0] = + getDeterminant(_[0][1], _[0][2], _[0][3],
									 _[1][1], _[1][2], _[1][3],
									 _[3][1], _[3][2], _[3][3]);
		adj_[2][1] = - getDeterminant(_[0][0], _[0][2], _[0][3],
									 _[1][0], _[1][2], _[1][3],
									 _[3][0], _[3][2], _[3][3]);
		adj_[2][2] = + getDeterminant(_[0][0], _[0][1], _[0][3],
									 _[1][0], _[1][1], _[1][3],
									 _[3][0], _[3][1], _[3][3]);
		adj_[2][3] = - getDeterminant(_[0][0], _[0][1], _[0][2],
									 _[1][0], _[1][1], _[1][2],
									 _[3][0], _[3][1], _[3][2]);

		adj_[3][0] = - getDeterminant(_[0][1], _[0][2], _[0][3],
									 _[1][1], _[1][2], _[1][3],
									 _[2][1], _[2][2], _[2][3]);
		adj_[3][1] = + getDeterminant(_[0][0], _[0][2], _[0][3],
									 _[1][0], _[1][2], _[1][3],
									 _[2][0], _[2][2], _[2][3]);
		adj_[3][2] = - getDeterminant(_[0][0], _[0][1], _[0][3],
									 _[1][0], _[1][1], _[1][3],
									 _[2][0], _[2][1], _[2][3]);
		adj_[3][3] = + getDeterminant(_[0][0], _[0][1], _[0][2],
									 _[1][0], _[1][1], _[1][2],
									 _[2][0], _[2][1], _[2][2]);

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				_[i][j] = (1.0f / determinant) * adj_[j][i];
			}
		}
	}
}



mtx mtx::getInversed() const
{
	mtx temp = *this;
	temp.inverse();
	return temp;
}



void mtx::loadIdentity()
{
	_[0][0] = 1.0f;	_[0][1] = 0.0f;	_[0][2] = 0.0f;	_[0][3] = 0.0f;
	_[1][0] = 0.0f;	_[1][1] = 1.0f;	_[1][2] = 0.0f;	_[1][3] = 0.0f;
	_[2][0] = 0.0f;	_[2][1] = 0.0f;	_[2][2] = 1.0f;	_[2][3] = 0.0f;
	_[3][0] = 0.0f;	_[3][1] = 0.0f;	_[3][2] = 0.0f;	_[3][3] = 1.0f;
}



void mtx::loadZeroes()
{
	_[0][0] = 0.0f;	_[0][1] = 0.0f;	_[0][2] = 0.0f;	_[0][3] = 0.0f;
	_[1][0] = 0.0f;	_[1][1] = 0.0f;	_[1][2] = 0.0f;	_[1][3] = 0.0f;
	_[2][0] = 0.0f;	_[2][1] = 0.0f;	_[2][2] = 0.0f;	_[2][3] = 0.0f;
	_[3][0] = 0.0f;	_[3][1] = 0.0f;	_[3][2] = 0.0f;	_[3][3] = 0.0f;
}



void mtx::loadLookAtLH(const vec3& eye, const vec3& at, const vec3& up)
{
	vec3 zAxis = (at - eye).getNormalized();
	vec3 xAxis = (up ^ zAxis).getNormalized();
	vec3 yAxis = (zAxis ^ xAxis);

	_[0][0] = xAxis.x;			_[0][1] = yAxis.x;			_[0][2] = zAxis.x;			_[0][3] = 0.0f;
	_[1][0] = xAxis.y;			_[1][1] = yAxis.y;			_[1][2] = zAxis.y;			_[1][3] = 0.0f;
	_[2][0] = xAxis.z;			_[2][1] = yAxis.z;			_[2][2] = zAxis.z;			_[2][3] = 0.0f;
	_[3][0] = -(xAxis % eye);	_[3][1] = -(yAxis % eye);	_[3][2] = -(zAxis % eye);	_[3][3] = 1.0f;
}



void mtx::loadLookAtRH(const vec3& eye, const vec3& at, const vec3& up)
{
	vec3 zAxis = (eye - at).getNormalized();
	vec3 xAxis = (up ^ zAxis).getNormalized();
	vec3 yAxis = (zAxis ^ xAxis);

	_[0][0] = xAxis.x;			_[0][1] = yAxis.x;			_[0][2] = zAxis.x;			_[0][3] = 0.0f;
	_[1][0] = xAxis.y;			_[1][1] = yAxis.y;			_[1][2] = zAxis.y;			_[1][3] = 0.0f;
	_[2][0] = xAxis.z;			_[2][1] = yAxis.z;			_[2][2] = zAxis.z;			_[2][3] = 0.0f;
	_[3][0] = -(xAxis % eye);	_[3][1] = -(yAxis % eye);	_[3][2] = -(zAxis % eye);	_[3][3] = 1.0f;
}



void mtx::loadPerspectiveFovLH(float fovY, float aspectRatio, float zNear, float zFar)
{
	float yScale = 1.0f / tanf(fovY / 2.0f);
	float xScale = yScale / aspectRatio;

	_[0][0] = xScale;	_[0][1] = 0.0f;		_[0][2] = 0.0f;								_[0][3] = 0.0f;
	_[1][0] = 0.0f;		_[1][1] = yScale;	_[1][2] = 0.0f;								_[1][3] = 0.0f;
	_[2][0] = 0.0f;		_[2][1] = 0.0f;		_[2][2] = zFar / (zFar - zNear);			_[2][3] = 1.0f;
	_[3][0] = 0.0f;		_[3][1] = 0.0f;		_[3][2] = -zNear * zFar / (zFar - zNear);	_[3][3] = 0.0f;
}



void mtx::loadPerspectiveFovRH(float fovY, float aspectRatio, float zNear, float zFar)
{
	float yScale = 1.0f / tanf(fovY / 2.0f);
	float xScale = yScale / aspectRatio;

	_[0][0] = xScale;	_[0][1] = 0.0f;		_[0][2] = 0.0f;								_[0][3] = 0.0f;
	_[1][0] = 0.0f;		_[1][1] = yScale;	_[1][2] = 0.0f;								_[1][3] = 0.0f;
	_[2][0] = 0.0f;		_[2][1] = 0.0f;		_[2][2] = zFar / (zNear - zFar);			_[2][3] = -1.0f;
	_[3][0] = 0.0f;		_[3][1] = 0.0f;		_[3][2] = zNear * zFar / (zNear - zFar);	_[3][3] = 0.0f;
}



void mtx::loadOrthoOffCenterLH(float left, float right, float bottom, float top, float zNear, float zFar)
{
	_[0][0] = 2.0f / (right - left);			_[0][1] = 0.0f;								_[0][2] = 0.0f;						_[0][3] = 0.0f;
	_[1][0] = 0.0f;								_[1][1] = 2.0f / (top - bottom);			_[1][2] = 0.0f;						_[1][3] = 0.0f;
	_[2][0] = 0.0f;								_[2][1] = 0.0f;								_[2][2] = 1.0f / (zFar - zNear);	_[2][3] = 0.0f;
	_[3][0] = (1.0f + right)/(1.0f - right);	_[3][1] = (top + bottom)/(bottom - top);	_[3][2] = -zNear / (zFar - zNear);	_[3][3] = 1.0f;
}



void mtx::loadOrthoOffCenterRH(float left, float right, float bottom, float top, float zNear, float zFar)
{
	_[0][0] = 2.0f / (right - left);			_[0][1] = 0.0f;								_[0][2] = 0.0f;						_[0][3] = 0.0f;
	_[1][0] = 0.0f;								_[1][1] = 2.0f / (top - bottom);			_[1][2] = 0.0f;						_[1][3] = 0.0f;
	_[2][0] = 0.0f;								_[2][1] = 0.0f;								_[2][2] = 1.0f / (zNear - zFar);	_[2][3] = 0.0f;
	_[3][0] = (1.0f + right)/(1.0f - right);	_[3][1] = (top + bottom)/(bottom - top);	_[3][2] = -zNear / (zFar - zNear);	_[3][3] = 1.0f;
}



void mtx::loadOrthoLH(float width, float height, float zNear, float zFar)
{
	_[0][0] = 2.0f / width;	_[0][1] = 0.0f;				_[0][2] = 0.0f;						_[0][3] = 0.0f;
	_[1][0] = 0.0f;			_[1][1] = 2.0f / height;	_[1][2] = 0.0f;						_[1][3] = 0.0f;
	_[2][0] = 0.0f;			_[2][1] = 0.0f;				_[2][2] = 1.0f / (zFar - zNear);	_[2][3] = 0.0f;
	_[3][0] = 0.0f;			_[3][1] = 0.0f;				_[3][2] = -zNear / (zFar - zNear);	_[3][3] = 1.0f;
}



void mtx::loadOrthoRH(float width, float height, float zNear, float zFar)
{
	_[0][0] = 2.0f / width;	_[0][1] = 0.0f;				_[0][2] = 0.0f;						_[0][3] = 0.0f;
	_[1][0] = 0.0f;			_[1][1] = 2.0f / height;	_[1][2] = 0.0f;						_[1][3] = 0.0f;
	_[2][0] = 0.0f;			_[2][1] = 0.0f;				_[2][2] = 1.0f / (zNear - zFar);	_[2][3] = 0.0f;
	_[3][0] = 0.0f;			_[3][1] = 0.0f;				_[3][2] = zNear / (zNear - zFar);	_[3][3] = 1.0f;
}



void mtx::loadTranslate(float tx, float ty, float tz)
{
	_[0][0] = 1.0f;	_[0][1] = 0.0f;	_[0][2] = 0.0f;	_[0][3] = 0.0f;
	_[1][0] = 0.0f;	_[1][1] = 1.0f;	_[1][2] = 0.0f;	_[1][3] = 0.0f;
	_[2][0] = 0.0f;	_[2][1] = 0.0f;	_[2][2] = 1.0f;	_[2][3] = 0.0f;
	_[3][0] = tx;	_[3][1] = ty;	_[3][2] = tz;	_[3][3] = 1.0f;
}



void mtx::loadRotate(float angle, float rx, float ry, float rz)
{
	float s = sinf(angle);
	float c = cosf(angle);

	float length = sqrtf(rx*rx + ry*ry + rz*rz);

	rx /= length;
	ry /= length;
	rz /= length;

	_[0][0] = c+rx*rx*(1-c);	_[0][1] = rx*ry*(1-c)+rz*s;	_[0][2] = rx*rz*(1-c)-ry*s;	_[0][3] = 0.0f;
	_[1][0] = rx*ry*(1-c)-rz*s;	_[1][1] = c+ry*ry*(1-c);	_[1][2] = ry*rz*(1-c)+rx*s;	_[1][3] = 0.0f;
	_[2][0] = rx*rz*(1-c)+ry*s;	_[2][1] = ry*rz*(1-c)-rx*s;	_[2][2] = c+rz*rz*(1-c);	_[2][3] = 0.0f;
	_[3][0] = 0.0f;				_[3][1] = 0.0f;				_[3][2] = 0.0f;				_[3][3] = 1.0f;
}



void mtx::loadRotateX(float angle)
{
	_[0][0] = 1.0f;	_[0][1] = 0.0f;			_[0][2] = 0.0f;			_[0][3] = 0.0f;
	_[1][0] = 0.0f;	_[1][1] = cosf(angle);	_[1][2] = sin(angle);	_[1][3] = 0.0f;
	_[2][0] = 0.0f;	_[2][1] = -sin(angle);	_[2][2] = cos(angle);	_[2][3] = 0.0f;
	_[3][0] = 0.0f;	_[3][1] = 0.0f;			_[3][2] = 0.0f;			_[3][3] = 1.0f;
}



void mtx::loadRotateY(float angle)
{
	_[0][0] = cos(angle);	_[0][1] = 0.0f;	_[0][2] = -sin(angle);	_[0][3] = 0.0f;
	_[1][0] = 0.0f;			_[1][1] = 1.0f;	_[1][2] = 0.0f;			_[1][3] = 0.0f;
	_[2][0] = sin(angle);	_[2][1] = 0.0f;	_[2][2] = cos(angle);	_[2][3] = 0.0f;
	_[3][0] = 0.0f;			_[3][1] = 0.0f;	_[3][2] = 0.0f;			_[3][3] = 1.0f;
}



void mtx::loadRotateZ(float angle)
{
	_[0][0] = cos(angle);	_[0][1] = sin(angle);	_[0][2] = 0.0f;	_[0][3] = 0.0f;
	_[1][0] = -sin(angle);	_[1][1] = cos(angle);	_[1][2] = 0.0f;	_[1][3] = 0.0f;
	_[2][0] = 0.0f;			_[2][1] = 0.0f;			_[2][2] = 1.0f;	_[2][3] = 0.0f;
	_[3][0] = 0.0f;			_[3][1] = 0.0f;			_[3][2] = 0.0f;	_[3][3] = 1.0f;
}



void mtx::loadScale(float sx, float sy, float sz)
{
	_[0][0] = sx;	_[0][1] = 0.0f;	_[0][2] = 0.0f;	_[0][3] = 0.0f;
	_[1][0] = 0.0f;	_[1][1] = sy;	_[1][2] = 0.0f;	_[1][3] = 0.0f;
	_[2][0] = 0.0f;	_[2][1] = 0.0f;	_[2][2] = sz;	_[2][3] = 0.0f;
	_[3][0] = 0.0f;	_[3][1] = 0.0f;	_[3][2] = 0.0f;	_[3][3] = 1.0f;
}



void mtx::loadReflect(const vec3& planePoint, const vec3& planeNormal)
{
	float planeD = -(planePoint % planeNormal);

	_[0][0] = -2.0f*planeNormal.x*planeNormal.x + 1.0f;	_[0][1] = -2.0f*planeNormal.y*planeNormal.x;		_[0][2] = -2.0f*planeNormal.z*planeNormal.x;		_[0][3] = 0.0f;
	_[1][0] = -2.0f*planeNormal.x*planeNormal.y;		_[1][1] = -2.0f*planeNormal.y*planeNormal.y + 1.0f;	_[1][2] = -2.0f*planeNormal.z*planeNormal.y;		_[1][3] = 0.0f;
	_[2][0] = -2.0f*planeNormal.x*planeNormal.z;		_[2][1] = -2.0f*planeNormal.y*planeNormal.z;		_[2][2] = -2.0f*planeNormal.z*planeNormal.z + 1.0f;	_[2][3] = 0.0f;
	_[3][0] = -2.0f*planeNormal.x*planeD;				_[3][1] = -2.0f*planeNormal.y*planeD;				_[3][2] = -2.0f*planeNormal.z*planeD;				_[3][3] = 1.0f;
}



mtx& mtx::operator = (const mtx& m)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			_[i][j] = m._[i][j];
		}
	}

	return *this;
}



bool mtx::operator == (const mtx& m) const
{
	bool equals = true;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if ( fabs(_[i][j] - m._[i][j]) > epsilon )
				equals = false;
		}
	}

	return equals;
}



bool mtx::operator != (const mtx& m) const
{
	return !(*this == m);
}



mtx mtx::operator + (const mtx& m) const
{
	mtx temp;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			temp._[i][j] = _[i][j] + m._[i][j];
		}
	}

	return temp;
}



mtx& mtx::operator += (const mtx& m)
{
	*this = *this + m;
	return *this;
}



mtx mtx::operator * (const mtx& m) const
{
	mtx temp;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			temp._[i][j] =
				_[i][0] * m._[0][j] +
				_[i][1] * m._[1][j] +
				_[i][2] * m._[2][j] +
				_[i][3] * m._[3][j];
		}
	}

	return temp;
}



mtx& mtx::operator *= (const mtx& m)
{
	*this = *this * m;
	return *this;
}



vec3 mtx::operator * (const vec3& v) const
{
	vec3 temp;

	temp.x = _[0][0]*v.x + _[0][1]*v.y + _[0][2]*v.z;
	temp.y = _[1][0]*v.x + _[1][1]*v.y + _[1][2]*v.z;
	temp.z = _[2][0]*v.x + _[2][1]*v.y + _[2][2]*v.z;

	return temp;
}



vec4 mtx::operator * (const vec4& v) const
{
	vec4 temp;

	temp.x = _[0][0]*v.x + _[0][1]*v.y + _[0][2]*v.z + _[0][3]*v.w;
	temp.y = _[1][0]*v.x + _[1][1]*v.y + _[1][2]*v.z + _[1][3]*v.w;
	temp.z = _[2][0]*v.x + _[2][1]*v.y + _[2][2]*v.z + _[2][3]*v.w;
	temp.w = _[3][0]*v.x + _[3][1]*v.y + _[3][2]*v.z + _[3][3]*v.w;

	return temp;
}



mtx mtx::operator * (float f) const
{
	mtx temp;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			temp._[i][j] = _[i][j] * f;
		}
	}

	return temp;
}



mtx& mtx::operator *= (float f)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			_[i][j] *= f;
		}
	}

	return *this;
}



mtx operator * (float f, const mtx& m)
{
	return m * f;
}
