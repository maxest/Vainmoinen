#ifdef RENDERER_OGL

#include <renderer_ogl/texture_ogl.hpp>
#include <renderer_ogl/renderer_ogl.hpp>
#include <common/logger.hpp>
#include <common/image.hpp>



void CTexture::create(ushort width, ushort height, byte mipmapsNum, uint usage, TextureFormat::TYPE format, const string& name)
{
	destroy();

	this->width = width;
	this->height = height;
	this->mipmapsNum = (mipmapsNum == 0 ? getMaxPossibleMipmapsNum(width, height) : mipmapsNum);
	this->usage = usage;
	this->format = format;
	this->type = TextureType::Standard2D;

	this->name = name;

	this->mag = SamplerFiltering::Linear;
	this->min = SamplerFiltering::Point;
	this->mip = SamplerFiltering::Linear;
	this->addressing = SamplerAddressing::Wrap;
	this->borderColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);

	this->samplerName = "";

	glGenTextures(1, &id);
	if (!id)
	{
		Logger << "\tERROR: Couldn't create texture: " << name << endl;
		exit(1);
	}
	glActiveTexture(GL_TEXTURE31);
	glBindTexture(type, id);

	glTexParameteri(type, GL_GENERATE_MIPMAP, ((usage & TextureUsage::AutoGenMipmaps) ? true : false));
	glTexParameteri(type, GL_TEXTURE_MAX_LEVEL, ((usage & TextureUsage::AutoGenMipmaps) ? 1000 : this->mipmapsNum - 1));

	for (int i = 0; i < this->mipmapsNum; i++)
	{
		glTexImage2D(type, i, getInternalFormat_ogl(), width, height, 0, getFormat_ogl(), GL_UNSIGNED_BYTE, NULL);

		if (width > 1)
			width >>= 1;
		if (height > 1)
			height >>= 1;
	}
}



void CTexture::createCube(ushort size, byte mipmapsNum, uint usage, TextureFormat::TYPE format, const string& name)
{
	destroy();

	this->width = size;
	this->height = size;
	this->mipmapsNum = (mipmapsNum == 0 ? getMaxPossibleMipmapsNum(width, height) : mipmapsNum);
	this->usage = usage;
	this->format = format;
	this->type = TextureType::CubeMap;

	this->name = name;

	this->mag = SamplerFiltering::Linear;
	this->min = SamplerFiltering::Point;
	this->mip = SamplerFiltering::Linear;
	this->addressing = SamplerAddressing::Wrap;
	this->borderColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);

	this->samplerName = "";

	glGenTextures(1, &id);
	if (!id)
	{
		Logger << "\tERROR: Couldn't create cube texture: " << name << endl;
		exit(1);
	}
	glActiveTexture(GL_TEXTURE31);
	glBindTexture(type, id);

	glTexParameteri(type, GL_GENERATE_MIPMAP, ((usage & TextureUsage::AutoGenMipmaps) ? true : false));
	glTexParameteri(type, GL_TEXTURE_MAX_LEVEL, ((usage & TextureUsage::AutoGenMipmaps) ? 1000 : this->mipmapsNum - 1));

	for (int i = 0; i < this->mipmapsNum; i++)
	{
		for (int j = 0; j < 6; j++)
			glTexImage2D(CubeMapFace::PositiveX + j, i, getInternalFormat_ogl(), size, size, 0, getFormat_ogl(), GL_UNSIGNED_BYTE, NULL);

		if (size > 1)
			size >>= 1;
	}
}



void CTexture::createFromFile(const string& fileName, byte mipmapsNum)
{
	CImageByte image;

	image.createFromFile(fileName, mipmapsNum);
	image.updateMipmaps();

	create(image.getWidth(), image.getHeight(), image.getMipmapsNum(), 0, TextureFormat::ARGB8, fileName);
	setImageData(image);
}



void CTexture::destroy()
{
	if (id != 0)
	{
		glDeleteTextures(1, &id);
		id = 0;
	}
}



void CTexture::setImageData(const CImageByte& image, CubeMapFace::TYPE cubeMapFace)
{
	if ( (usage & TextureUsage::RenderTarget) || (usage & TextureUsage::DepthStencil) )
	{
		Logger << "\tERROR: Cannot set image data for texture (it's a render target or depth-stencil surface): " << name << endl;
		exit(1);
	}

	glActiveTexture(GL_TEXTURE31);
	glBindTexture(type, id);

	for (int i = 0; i < MIN(mipmapsNum, image.getMipmapsNum()); i++)
	{
		if (type == TextureType::Standard2D)
			glTexSubImage2D(type, i, 0, 0, image.getMipmaps()[i].width, image.getMipmaps()[i].height, getFormat_ogl(), GL_UNSIGNED_BYTE, image.getMipmaps()[i].data);
		else if (type == TextureType::CubeMap)
			glTexSubImage2D(cubeMapFace, i, 0, 0, image.getMipmaps()[i].width, image.getMipmaps()[i].height, getFormat_ogl(), GL_UNSIGNED_BYTE, image.getMipmaps()[i].data);
	}
}



void CTexture::setFiltering_ogl(SamplerFiltering::TYPE mag, SamplerFiltering::TYPE min, SamplerFiltering::TYPE mip)
{
	if ( (this->mag != mag) || (this->min != min) || (this->mip != mip) )
	{
		glActiveTexture(GL_TEXTURE31);
		glBindTexture(type, id);
	}

	if (this->mag != mag)
	{
		if (mag == SamplerFiltering::Point)
			glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		else if (mag == SamplerFiltering::Linear)
			glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		this->mag = mag;
	}

	if ( (this->min != min) || (this->mip != mip) )
	{
		if (min == SamplerFiltering::Point && mip == SamplerFiltering::None)
			glTexParameteri(this->type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		else if (min == SamplerFiltering::Linear && mip == SamplerFiltering::None)
			glTexParameteri(this->type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		else if (min == SamplerFiltering::Point && mip == SamplerFiltering::Point)
			glTexParameteri(this->type, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		else if (min == SamplerFiltering::Linear && mip == SamplerFiltering::Point)
			glTexParameteri(this->type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

		else if (min == SamplerFiltering::Point && mip == SamplerFiltering::Linear)
			glTexParameteri(this->type, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		else if (min == SamplerFiltering::Linear && mip == SamplerFiltering::Linear)
			glTexParameteri(this->type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		this->min = min;
		this->mip = mip;
	}
}



void CTexture::setAddressing_ogl(SamplerAddressing::TYPE addressing)
{
	if (this->addressing != addressing)
	{
		glActiveTexture(GL_TEXTURE31);
		glBindTexture(type, id);

		glTexParameteri(this->type, GL_TEXTURE_WRAP_S, addressing);
		glTexParameteri(this->type, GL_TEXTURE_WRAP_T, addressing);

		this->addressing = addressing;
	}
}



void CTexture::setBorderColor_ogl(const vec4& borderColor)
{
	if (this->borderColor != borderColor)
	{
		glActiveTexture(GL_TEXTURE31);
		glBindTexture(type, id);

		float borderColorArray[] = { borderColor.x, borderColor.y, borderColor.z, borderColor.w };
		glTexParameterfv(this->type, GL_TEXTURE_BORDER_COLOR, borderColorArray);

		this->borderColor = borderColor;
	}
}



#endif
