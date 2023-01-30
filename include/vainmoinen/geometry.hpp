#ifndef VAINMOINEN_GEOMETRY_H
#define VAINMOINEN_GEOMETRY_H

#include <math/main.hpp>



struct Vertex
{
	Vertex() {}
	Vertex(const vec4& position, const vec3& color, const vec2& texCoord)
	{
		this->position = position;
		this->color = color;
		this->texCoord = texCoord;
	}

	vec4 position;
	vec3 color;
	vec2 texCoord;
};



struct Triangle
{
	Vertex vertices[3];
};



#endif
