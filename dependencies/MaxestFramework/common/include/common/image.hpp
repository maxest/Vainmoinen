#ifndef COMMON_IMAGE_H
#define COMMON_IMAGE_H

#include "common.hpp"
#include <math/common.hpp>



template <class TYPE>
struct Mipmap
{
	ushort width, height;
	TYPE *data;
};



extern void CImage_createSystem();
extern void CImage_destroySystem();



template <class TYPE>
class CImage
{
public:
	CImage() { mipmaps = NULL; }
	~CImage() { destroy(); }

	void create(ushort width, ushort height, byte componentsNum, byte mipmapsNum, const string& name = "");
	void createFromFile(const string& fileName, byte mipmapsNum); // mipmapsNum is responsible only for creating the mipmaps, but the data is not filled (excluding the base level of course, which is filled)
	void destroy();

	void saveToFile(const string& fileName);

	void swapChannels(byte firstChannel, byte secondChannel);
	void shift(short xOffset, short yOffset);

	void updateMipmaps();

	const byte& getComponentsNum() const { return componentsNum; }
	const byte& getMipmapsNum() const { return mipmapsNum; }
	const Mipmap<TYPE> *getMipmaps() const { return mipmaps; }

	const ushort& getWidth(byte mipmapIndex = 0) const { return mipmaps[mipmapIndex].width; }
	const ushort& getHeight(byte mipmapIndex = 0) const { return mipmaps[mipmapIndex].height; }
	const TYPE *getData(byte mipmapIndex = 0) const { return mipmaps[mipmapIndex].data; }
	uint getDataComponentsNum(byte mipmapIndex = 0) const { return componentsNum * mipmaps[mipmapIndex].width * mipmaps[mipmapIndex].height; }
	uint getDataSize(byte mipmapIndex = 0) const;

	uint getPixelIndex(ushort x, ushort y, byte mipmapIndex = 0) const { return (y * mipmaps[mipmapIndex].width + x); }
	uint getPixelFirstComponentIndex(ushort x, ushort y, byte mipmapIndex = 0) const { return componentsNum * getPixelIndex(x, y, mipmapIndex); }

	TYPE& operator () (ushort x, ushort y, byte component = 0, byte mipmapIndex = 0)
	{
		return mipmaps[mipmapIndex].data[getPixelFirstComponentIndex(x, y, mipmapIndex) + component];
	}

private:
	byte componentsNum;
	byte mipmapsNum;
	Mipmap<TYPE> *mipmaps;

	string name;
};

typedef CImage<byte> CImageByte;
typedef CImage<float> CImageFloat;

template<> inline uint CImage<byte>::getDataSize(byte mipmapIndex) const
{
	return 1 * getDataComponentsNum(mipmapIndex);
}
template<> inline uint CImage<float>::getDataSize(byte mipmapIndex) const
{
	return 4 * getDataComponentsNum(mipmapIndex);
}

template<> void CImage<byte>::createFromFile(const string& fileName, byte mipmapsNum);
template<> void CImage<byte>::saveToFile(const string& fileName);
template<> void CImage<byte>::updateMipmaps();



template <class TYPE>
void CImage<TYPE>::create(ushort width, ushort height, byte componentsNum, byte mipmapsNum, const string& name)
{
	destroy();

	this->componentsNum = componentsNum;
	this->mipmapsNum = (mipmapsNum == 0 ? getMaxPossibleMipmapsNum(width, height) : mipmapsNum);
	this->mipmaps = new Mipmap<TYPE>[this->mipmapsNum];

	if (this->mipmapsNum > 1)
	{
		if (!isPowerOfTwo(width) || !isPowerOfTwo(height))
		{
			Logger << "\tERROR: Couldn't generate mipmaps for image: " << name << endl;
			exit(1);
		}
	}

	for (int i = 0; i < this->mipmapsNum; i++)
	{
		mipmaps[i].width = width;
		mipmaps[i].height = height;
		mipmaps[i].data = new TYPE[componentsNum * width * height];

		if (width > 1)
			width >>= 1;
		if (height > 1)
			height >>= 1;
	}

	this->name = name;
}



template <class TYPE>
void CImage<TYPE>::destroy()
{
	if (mipmaps != NULL)
	{
		for (int i = 0; i < this->mipmapsNum; i++)
			delete[] mipmaps[i].data;

		delete[] mipmaps;
		mipmaps = NULL;
	}
}



template <class TYPE>
void CImage<TYPE>::swapChannels(byte firstChannel, byte secondChannel)
{
	for (uint i = 0; i < getDataComponentsNum(); i += componentsNum)
	{
		byte temp = mipmaps[0].data[i + firstChannel];
		mipmaps[0].data[i + firstChannel] = mipmaps[0].data[i + secondChannel];
		mipmaps[0].data[i + secondChannel] = temp;
	}
}



template <class TYPE>
void CImage<TYPE>::shift(short xOffset, short yOffset)
{
	// normalize offsets

	if (xOffset >= 0)
	{
		xOffset = xOffset % getWidth();
	}
	else
	{
		while (xOffset < 0)
			xOffset += getWidth();
	}

	if (yOffset >= 0)
	{
		yOffset = yOffset % getHeight();
	}
	else
	{
		while (yOffset < 0)
			yOffset += getHeight();
	}

	// shift

	TYPE *shiftedData = new TYPE[getDataComponentsNum()];

	for (ushort j = 0; j < getHeight(); j++)
	{
		for (ushort i = 0; i < getWidth(); i++)
		{
			int x = (i + xOffset) % getWidth();
			int y = (j + yOffset) % getHeight();

			int index1 = getPixelFirstComponentIndex(i, j);
			int index2 = getPixelFirstComponentIndex(x, y);

			for (byte k = 0; k < componentsNum; k++)
			{
				shiftedData[index2 + k] = mipmaps[0].data[index1 + k];
			}
		}
	}

	memcpy(mipmaps[0].data, shiftedData, getDataSize());

	delete[] shiftedData;
}



inline byte getMaxPossibleMipmapsNum(ushort width, ushort height)
{
	byte mipmapsNum = 1;
	ushort size = MAX(width, height);

	if (size == 1)
		return 1;

	do {
		size >>= 1;
		mipmapsNum++;
	} while (size != 1);

	return mipmapsNum;
}



#endif
