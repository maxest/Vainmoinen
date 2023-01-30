#ifndef VAINMOINEN_TEXTURE_H
#define VAINMOINEN_TEXTURE_H

#include <common/main.hpp>
#include <math/main.hpp>



class CVRenderer;



class CTexture
{
	friend class CVRenderer;

public:
	CTexture() { mipmaps = NULL; }
	~CTexture() { destroy(); }

	void create(ushort width, ushort height, byte mipmapsNum = 0)
	{
		destroy();

		this->mipmapsNum = (mipmapsNum == 0 ? getMaxPossibleMipmapsNum(width, height) : mipmapsNum);
		this->mipmaps = new Mipmap<byte>[this->mipmapsNum];

		for (int i = 0; i < this->mipmapsNum; i++)
		{
			mipmaps[i].width = width;
			mipmaps[i].height = height;
			mipmaps[i].data = new byte[4*width*height];

			if (width > 1)
				width >>= 1;
			if (height > 1)
				height >>= 1;
		}

		this->widthLog = (byte)(_log2(mipmaps[0].width) + epsilon);
		this->heightLog = (byte)(_log2(mipmaps[0].height) + epsilon);
	}

	void CTexture::createFromFile(const string& fileName)
	{
		CImageByte image;

		image.createFromFile(fileName, 0);
		image.swapChannels(0, 2);
		image.updateMipmaps();

		create(image.getWidth(), image.getHeight());
		setImageData(image);
	}

	void destroy()
	{
		if (mipmaps != NULL)
		{
			for (int i = 0; i < this->mipmapsNum; i++)
				delete[] mipmaps[i].data;

			delete[] mipmaps;
			mipmaps = NULL;
		}
	}

	void setImageData(const CImageByte& image)
	{
	//	byte reds[]   = { 255, 255, 0,   0,   255, 0,   255, 128, 0 };
	//	byte greens[] = { 255, 0,   255, 0,   255, 255, 0,   128, 0 };
	//	byte blues[]  = { 255, 0,   0,   255, 0,   255, 255, 128, 0 };

		for (int i = 0; i < image.getMipmapsNum(); i++)
		{
			mipmaps[i].width = image.getMipmaps()[i].width;
			mipmaps[i].height = image.getMipmaps()[i].height;
			memcpy(mipmaps[i].data, image.getMipmaps()[i].data, 4*mipmaps[i].width*mipmaps[i].height);

		/*	for (int x = 0; x < mipmaps[i].width; x++)
			{
				for (int y = 0; y < mipmaps[i].height; y++)
				{
					int index = y*mipmaps[i].width + x;

					mipmaps[i].data[4*index + 0] = reds[i];
					mipmaps[i].data[4*index + 1] = greens[i];
					mipmaps[i].data[4*index + 2] = blues[i];
					mipmaps[i].data[4*index + 3] = 255;
				}
			}*/
		}
	}

private:
	byte mipmapsNum;
	Mipmap<byte> *mipmaps;
	byte widthLog, heightLog; // log2 of width and height of the base mip-level
};



#endif
