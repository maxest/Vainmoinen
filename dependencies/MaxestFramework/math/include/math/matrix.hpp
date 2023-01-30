#ifndef MATH_MATRIX_H
#define MATH_MATRIX_H

#include "vector.hpp"
#include <common/common.hpp>

#include <iomanip>



class mtx
{
public:
	mtx() {}
	mtx(float _00_11_22_33);
	mtx(float _00, float _01, float _02, float _03,
		float _10, float _11, float _12, float _13,
		float _20, float _21, float _22, float _23,
		float _30, float _31, float _32, float _33);
	mtx(const vec4& row1, const vec4& row2, const vec4& row3, const vec4& row4);
	mtx(const mtx& m);

	bool isOrthonormal() const;

	void transpose();
	mtx getTransposed() const;

	void inverse();
	mtx getInversed() const;

	void loadIdentity();
	void loadZeroes();

	void loadLookAtLH(const vec3& eye, const vec3& at, const vec3& up);
	void loadLookAtRH(const vec3& eye, const vec3& at, const vec3& up);
	void loadPerspectiveFovLH(float fovY, float aspectRatio, float zNear, float zFar);
	void loadPerspectiveFovRH(float fovY, float aspectRatio, float zNear, float zFar);
	void loadOrthoOffCenterLH(float left, float right, float bottom, float top, float zNear, float zFar);
	void loadOrthoOffCenterRH(float left, float right, float bottom, float top, float zNear, float zFar);
	void loadOrthoLH(float width, float height, float zNear, float zFar);
	void loadOrthoRH(float width, float height, float zNear, float zFar);

	void loadTranslate(float tx, float ty, float tz);
	void loadTranslate(const vec3& v) { loadTranslate(v.x, v.y, v.z); }
	void loadRotate(float angle, float rx, float ry, float rz);
	void loadRotate(float angle, const vec3& v) { loadRotate(angle, v.x, v.y, v.z); }
	void loadRotateX(float angle);
	void loadRotateY(float angle);
	void loadRotateZ(float angle);
	void loadScale(float sx, float sy, float sz);
	void loadScale(const vec3& v) { loadScale(v.x, v.y, v.z); }
	void loadReflect(const vec3& planePoint, const vec3& planeNormal);

	float operator () (int row, int col) const { return _[row][col]; }
	float& operator () (int row, int col) { return _[row][col]; }

	mtx& operator = (const mtx& m);
	bool operator == (const mtx& m) const;
	bool operator != (const mtx& m) const;

	mtx operator + (const mtx& m) const;
	mtx& operator += (const mtx& m);

	mtx operator * (const mtx& m) const;
	mtx& operator *= (const mtx& m);

	vec3 operator * (const vec3& v) const;
	vec4 operator * (const vec4& v) const;

	mtx operator * (float f) const;
	mtx& operator *= (float f);

	friend mtx operator * (float f, const mtx& m);

public:
	static mtx identity()
	{
		mtx temp;
		temp.loadIdentity();
		return temp;
	}

	static mtx zeroes()
	{
		mtx temp;
		temp.loadZeroes();
		return temp;
	}

	static mtx lookAtLH(const vec3& eye, const vec3& at, const vec3& up)
	{
		mtx temp;
		temp.loadLookAtLH(eye, at, up);
		return temp;
	}

	static mtx lookAtRH(const vec3& eye, const vec3& at, const vec3& up)
	{
		mtx temp;
		temp.loadLookAtRH(eye, at, up);
		return temp;
	}

	static mtx perspectiveFovLH(float fovY, float aspectRatio, float zNear, float zFar)
	{
		mtx temp;
		temp.loadPerspectiveFovLH(fovY, aspectRatio, zNear, zFar);
		return temp;
	}

	static mtx perspectiveFovRH(float fovY, float aspectRatio, float zNear, float zFar)
	{
		mtx temp;
		temp.loadPerspectiveFovRH(fovY, aspectRatio, zNear, zFar);
		return temp;
	}

	static mtx orthoOffCenterLH(float left, float right, float bottom, float top, float zNear, float zFar)
	{
		mtx temp;
		temp.loadOrthoOffCenterLH(left, right, bottom, top, zNear, zFar);
		return temp;
	}

	static mtx orthoOffCenterRH(float left, float right, float bottom, float top, float zNear, float zFar)
	{
		mtx temp;
		temp.loadOrthoOffCenterRH(left, right, bottom, top, zNear, zFar);
		return temp;
	}

	static mtx orthoLH(float width, float height, float zNear, float zFar)
	{
		mtx temp;
		temp.loadOrthoLH(width, height, zNear, zFar);
		return temp;
	}

	static mtx orthoRH(float width, float height, float zNear, float zFar)
	{
		mtx temp;
		temp.loadOrthoRH(width, height, zNear, zFar);
		return temp;
	}

	static mtx translate(float tx, float ty, float tz)
	{
		mtx temp;
		temp.loadTranslate(tx, ty, tz);
		return temp;
	}

	static mtx translate(const vec3& v)
	{
		return translate(v.x, v.y, v.z);
	}

	static mtx rotate(float angle, float rx, float ry, float rz)
	{
		mtx temp;
		temp.loadRotate(angle, rx, ry, rz);
		return temp;
	}

	static mtx rotate(float angle, const vec3& v)
	{
		return rotate(angle, v.x, v.y, v.z);
	}

	static mtx rotateX(float angle)
	{
		mtx temp;
		temp.loadRotateX(angle);
		return temp;
	}

	static mtx rotateY(float angle)
	{
		mtx temp;
		temp.loadRotateY(angle);
		return temp;
	}

	static mtx rotateZ(float angle)
	{
		mtx temp;
		temp.loadRotateZ(angle);
		return temp;
	}

	static mtx scale(float sx, float sy, float sz)
	{
		mtx temp;
		temp.loadScale(sx, sy, sz);
		return temp;
	}

	static mtx scale(const vec3& v)
	{
		return scale(v.x, v.y, v.z);
	}

	static mtx reflect(const vec3& planePoint, vec3 planeNormal)
	{
		mtx temp;
		temp.loadReflect(planePoint, planeNormal);
		return temp;
	}

public:
	float _[4][4];
};



mtx operator * (float f, const mtx& m);



inline ostream & operator << (ostream &stream, const mtx& m)
{
	int longestLength = 0;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			int l = toString(m(i, j)).length();
			
			if (l > longestLength)
				longestLength = l;
		}
	}

	int n = longestLength;

	stream << "[ " << left << setw(n) << m(0, 0) << "  " << left << setw(n) << m(0, 1) << "  " << left << setw(n) << m(0, 2) << "  " << m(0, 3) << " ]" << endl;
	stream << "[ " << left << setw(n) << m(1, 0) << "  " << left << setw(n) << m(1, 1) << "  " << left << setw(n) << m(1, 2) << "  " << m(1, 3) << " ]" << endl;
	stream << "[ " << left << setw(n) << m(2, 0) << "  " << left << setw(n) << m(2, 1) << "  " << left << setw(n) << m(2, 2) << "  " << m(2, 3) << " ]" << endl;
	stream << "[ " << left << setw(n) << m(3, 0) << "  " << left << setw(n) << m(3, 1) << "  " << left << setw(n) << m(3, 2) << "  " << m(3, 3) << " ]";
	
	return stream;
}



inline float getDeterminant(float a, float b, float c,
							float d, float e, float f,
							float g, float h, float i)
{
	return ( (a*e*i + d*h*c + g*b*f) - (c*e*g + f*h*a + i*b*d) );
}



struct MatrixArray
{
	byte count;
	mtx *matrices;

	MatrixArray()
	{
		count = 0;
		matrices = NULL;
	}

	MatrixArray(byte count)
	{
		this->count = count;

		if (count > 0)
			matrices = new mtx[count];
		else
			matrices = NULL;
	}

	~MatrixArray()
	{
		destroy();
	}

	void destroy()
	{
		if (matrices != NULL)
		{
			count = 0;
			delete[] matrices;
			matrices = NULL;
		}
	}

	MatrixArray& operator = (const MatrixArray& ma)
	{
		destroy();

		count = ma.count;
		matrices = new mtx[count];

		for (byte i = 0; i < count; i++)
			matrices[i] = ma.matrices[i];

		return *this;
	}

	bool operator == (const MatrixArray& ma) const
	{
		for (byte i = 0; i < count; i++)
		{
			if (matrices[i] != ma.matrices[i])
				return false;
		}

		return true;
	}

	bool operator != (const MatrixArray& ma) const
	{
		return !(*this == ma);
	}
};



#endif
