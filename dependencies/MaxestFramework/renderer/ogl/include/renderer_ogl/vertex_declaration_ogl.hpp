#ifndef RENDERER_OGL_VERTEX_DECLARATION_H
#define RENDERER_OGL_VERTEX_DECLARATION_H

#include <common/common.hpp>

#include <gl/glew.h>



class CRenderer;



namespace VertexElementType
{
	enum TYPE
	{
		Float1 = 0,
		Float2 = 1,
		Float3 = 2,
		Float4 = 3,
		Byte4 = 4
	};

	inline int getComponentsNum(TYPE type)
	{
		if (type >= Float1 && type <= Float4)
			return type + 1;

		return 4; // Byte4
	}

	inline int getSize(TYPE type)
	{
		if (type >= Float1 && type <= Float4)
			return sizeof(float)*(type + 1);

		return 4; // Byte4
	}

	inline GLenum getGLType(TYPE type)
	{
		if (type >= Float1 && type <= Float4)
			return GL_FLOAT;

		return GL_UNSIGNED_BYTE; // Byte4
	}
}

namespace VertexElementSemantic
{
	enum TYPE
	{
		Position,
		Color,
		Normal,
		TexCoord
	};

	inline string toString(TYPE semantic, byte semanticIndex)
	{
		if (semantic == Position)
			return "POSITION";
		else if (semantic == Color)
			return "COLOR";
		else if (semantic == Normal)
			return "NORMAL";
		else if (semantic == TexCoord)
			return "TEXCOORD" + ::toString((int)semanticIndex);
		return "";
	}
}



class CVertexDeclaration
{
	friend class CRenderer;

	struct VertexElement
	{
		byte offset;
		VertexElementType::TYPE type;
		VertexElementSemantic::TYPE semantic;
		byte semanticIndex;
	};

public:
	CVertexDeclaration() {}
	~CVertexDeclaration() { destroy(); }

	void create(const string& name = "");
	void destroy();

	void addVertexElement(byte offset, VertexElementType::TYPE type, VertexElementSemantic::TYPE semantic, byte semanticIndex);
	void build();

	void setSize(byte size) { this->size = size; }
	const byte& getSize() const { return size; }
	const string& getHash() const { return hash; }

private:
	vector<VertexElement> vertexElements;

	byte size;
	string hash;

	string name;
};



#endif
