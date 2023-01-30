#ifndef RENDERER_OGL_TEXTURE_H
#define RENDERER_OGL_TEXTURE_H

#include <common/common.hpp>
#include <common/image.hpp>
#include <math/common.hpp>

#include <GL/glew.h>



class CRenderer;



namespace SamplerFiltering
{
	enum TYPE
	{
		None,
		Point,
		Linear
	};
}

namespace SamplerAddressing
{
	enum TYPE
	{
		Wrap = GL_REPEAT,
		Clamp = GL_CLAMP_TO_EDGE,
		Border = GL_CLAMP_TO_BORDER
	};
}



namespace TextureUsage
{
	enum TYPE
	{
		RenderTarget,
		DepthStencil,
		AutoGenMipmaps // hardware generated mipmaps
	};
}

namespace TextureFormat
{
	enum TYPE
	{
		L8,
		ARGB8
	};
}

namespace TextureType
{
	enum TYPE
	{
		Standard2D = GL_TEXTURE_2D,
		CubeMap = GL_TEXTURE_CUBE_MAP
	};
};

namespace CubeMapFace
{
	enum TYPE
	{
		PositiveX = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		NegativeX = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		PositiveY = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		NegativeY = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		PositiveZ = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		NegativeZ = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};
}



class CTexture
{
	friend class CRenderer;

public:
	CTexture() { id = 0; }
	~CTexture() { destroy(); }

	void create(ushort width, ushort height, byte mipmapsNum, uint usage, TextureFormat::TYPE format, const string& name = "");
	void createCube(ushort size, byte mipmapsNum, uint usage, TextureFormat::TYPE format, const string& name = "");
	void createFromFile(const string& fileName, byte mipmapsNum);

	void destroy();

	void setImageData(const CImageByte& image, CubeMapFace::TYPE cubeMapFace = CubeMapFace::PositiveX);

	void setFiltering_ogl(SamplerFiltering::TYPE mag, SamplerFiltering::TYPE min, SamplerFiltering::TYPE mip);
	void setAddressing_ogl(SamplerAddressing::TYPE addressing);
	void setBorderColor_ogl(const vec4& borderColor);

	const ushort& getWidth() const { return width; }
	const ushort& getHeight() const { return height; }
	const byte& getMipmapsNum() const { return mipmapsNum; }

private:
	GLint getInternalFormat_ogl()
	{
		if (format == TextureFormat::ARGB8)
			return GL_RGBA8;
		else if (format == TextureFormat::L8)
			return GL_LUMINANCE8;
		return 0;
	}

	GLenum getFormat_ogl()
	{
		if (format == TextureFormat::ARGB8)
			return GL_BGRA;
		else if (format == TextureFormat::L8)
			return GL_LUMINANCE;
		return 0;
	}

private:
	uint id;

	ushort width, height;
	byte mipmapsNum;
	uint usage;
	TextureFormat::TYPE format;
	TextureType::TYPE type;

	string name;

	SamplerFiltering::TYPE mag, min, mip;
	SamplerAddressing::TYPE addressing;
	vec4 borderColor;

	// used by CRenderer to get uniform's location (see CRenderer::setTexture and CRenderer::preDraw)
	mutable string samplerName;
};



typedef CTexture CRenderTarget;
typedef CTexture CDepthStencilSurface;



#endif
