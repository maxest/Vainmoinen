#ifdef RENDERER_OGL

#include <renderer_ogl/index_buffer_ogl.hpp>
#include <renderer_ogl/renderer_ogl.hpp>
#include <common/logger.hpp>

#include <GL/glew.h>



void CIndexBuffer::create(uint size, bool dynamic, IndexBufferFormat::TYPE format, const string& name)
{
	destroy();

	this->size = size;
	this->dynamic = dynamic;
	this->format = format;

	this->name = name;

	glGenBuffers(1, &id);
	if (!id)
	{
		Logger << "\tERROR: Couldn't create index buffer: " << name << endl;
		exit(1);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, NULL, dynamic ? GL_STREAM_DRAW : GL_STATIC_DRAW);

	Renderer.currentIndexBuffer = NULL;
}



void CIndexBuffer::destroy()
{
	if (id != 0)
	{
		glDeleteBuffers(1, &id);
		id = 0;
	}
}



void CIndexBuffer::map(void*& data, bool discard)
{
	glBindBuffer(GL_ARRAY_BUFFER, id);
	if (discard)
		glBufferData(GL_ARRAY_BUFFER, size, NULL, dynamic ? GL_STREAM_DRAW : GL_STATIC_DRAW);
	data = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

	Renderer.currentIndexBuffer = NULL;
}



void CIndexBuffer::unmap()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	Renderer.currentIndexBuffer = NULL;
}



#endif
