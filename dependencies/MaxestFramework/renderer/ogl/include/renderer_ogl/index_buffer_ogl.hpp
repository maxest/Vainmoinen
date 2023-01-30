#ifndef RENDERER_OGL_INDEX_BUFFER_H
#define RENDERER_OGL_INDEX_BUFFER_H

#include <common/common.hpp>



class CRenderer;



namespace IndexBufferFormat
{
	enum TYPE
	{
		BIT_16,
		BIT_32
	};
}



class CIndexBuffer
{
	friend class CRenderer;

public:
	CIndexBuffer() { id = 0; }
	~CIndexBuffer() { destroy(); }

	void create(uint size, bool dynamic = false, IndexBufferFormat::TYPE format = IndexBufferFormat::BIT_16, const string& name = "");
	void destroy();

	void map(void*& data, bool discard = false);
	void unmap();

	const uint&  getSize() const { return size; }

private:
	uint id;

	uint size;
	bool dynamic;
	IndexBufferFormat::TYPE format;

	string name;
};



#endif
