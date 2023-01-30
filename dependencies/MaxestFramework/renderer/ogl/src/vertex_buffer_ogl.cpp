#ifdef RENDERER_OGL

#include <renderer_ogl/vertex_buffer_ogl.hpp>
#include <renderer_ogl/renderer_ogl.hpp>
#include <common/logger.hpp>

#include <GL/glew.h>



void CVertexBuffer::create(uint size, bool dynamic, const string& name)
{
	destroy();

	this->size = size;
	this->dynamic = dynamic;

	this->name = name;

	glGenBuffers(1, &id);
	if (!id)
	{
		Logger << "\tERROR: Couldn't create vertex buffer: " << name << endl;
		exit(1);
	}
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glBufferData(GL_ARRAY_BUFFER, size, NULL, dynamic ? GL_STREAM_DRAW : GL_STATIC_DRAW);

	Renderer.currentVertexBuffer = NULL;
}



void CVertexBuffer::destroy()
{
	if (id != 0)
	{
		glDeleteBuffers(1, &id);
		id = 0;
	}
}



void CVertexBuffer::map(void*& data, bool discard)
{
	glBindBuffer(GL_ARRAY_BUFFER, id);
	if (discard)
		glBufferData(GL_ARRAY_BUFFER, size, NULL, dynamic ? GL_STREAM_DRAW : GL_STATIC_DRAW);
	data = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

	Renderer.currentVertexBuffer = NULL;
}



void CVertexBuffer::unmap()
{
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glUnmapBuffer(GL_ARRAY_BUFFER);

	Renderer.currentVertexBuffer = NULL;
}



#endif
