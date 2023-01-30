#ifndef RENDERER_OGL_VERTEX_BUFFER_H
#define RENDERER_OGL_VERTEX_BUFFER_H

#include "vertex_declaration_ogl.hpp"
#include <common/common.hpp>



class CRenderer;



class CVertexBuffer
{
	friend class CRenderer;

public:
	CVertexBuffer() { id = 0; /*vertexDeclaration = NULL;*/ }
	~CVertexBuffer() { destroy(); }

	void create(uint size, bool dynamic = false, const string& name = "");
	void destroy();

	void map(void*& data, bool discard = false);
	void unmap();

	const uint& getSize() const { return size; }

private:
	uint id;

	uint size;
	bool dynamic;

	string name;
};



#endif
