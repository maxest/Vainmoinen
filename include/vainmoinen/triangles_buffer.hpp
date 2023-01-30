#ifndef VAINMOINEN_TRIANGLES_BUFFER_H
#define VAINMOINEN_TRIANGLES_BUFFER_H

#include "geometry.hpp"
#include <common/common.hpp>



class CVRenderer;



class CTrianglesBuffer
{
	friend class CVRenderer;

public:
	CTrianglesBuffer() { triangles = NULL; }
	~CTrianglesBuffer() { destroy(); }

	void create(int trianglesNum)
	{
		this->trianglesNum = trianglesNum;
		this->triangles = new Triangle[trianglesNum];
	}
	void destroy()
	{
		if (triangles != NULL)
		{
			delete[] triangles;
			triangles = NULL;
		}
	}

	Triangle* lockData() { return triangles; }
	void unlockData() {}

	const uint& getTrianglesNum() const { return trianglesNum; }

private:
	uint trianglesNum;
	Triangle *triangles;
};



#endif
