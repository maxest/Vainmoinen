#include <common/image.hpp>
#include <common/logger.hpp>

#include <FreeImage/FreeImage.h>



void FreeImage_errorFunction(FREE_IMAGE_FORMAT fif, const char *msg)
{
	Logger << "\tERROR (FreeImage error function): " << msg << endl;
	exit(1);
}



void CImage_createSystem()
{
	FreeImage_Initialise();
	FreeImage_SetOutputMessage(FreeImage_errorFunction);
}



void CImage_destroySystem()
{
	FreeImage_DeInitialise();
}



template<>
void CImage<byte>::createFromFile(const string& fileName, byte mipmapsNum)
{
	FREE_IMAGE_FORMAT fif;
	FIBITMAP* fib;

	fif = FreeImage_GetFileType(fileName.c_str());
	if (fif == FIF_UNKNOWN) 
		fif = FreeImage_GetFIFFromFilename(fileName.c_str());

	if (fif != FIF_BMP && fif != FIF_JPEG && fif != FIF_PNG)
	{
		Logger << "\tERROR: Couldn't recognize file's image format: " << fileName << endl;
		exit(1);
	}

	if (FreeImage_FIFSupportsReading(fif))
		fib = FreeImage_Load(fif, fileName.c_str());

	if (!fib)
	{
		Logger << "\tERROR: Couldn't load image data from file: " << fileName << endl;
		exit(1);
	}

	create(
		FreeImage_GetWidth(fib),
		FreeImage_GetHeight(fib),
		FreeImage_GetBPP(fib) / 8,
		mipmapsNum,
		fileName);
	memcpy(this->mipmaps[0].data, FreeImage_GetBits(fib), getDataSize());

	FreeImage_Unload(fib);

	Logger << "\tOK: Image loaded from file: " << fileName << endl;
}



template<>
void CImage<byte>::saveToFile(const string& fileName)
{
	FREE_IMAGE_FORMAT fif;
	FIBITMAP* fib;

	fif = FreeImage_GetFIFFromFilename(fileName.c_str());

	if (fif != FIF_BMP && fif != FIF_JPEG && fif != FIF_PNG)
	{
		Logger << "\tERROR: Couldn't recognize file's image format: " << fileName << endl;
		exit(1);
	}

	fib = FreeImage_Allocate(getWidth(), getHeight(), 8*componentsNum);

	byte *data = FreeImage_GetBits(fib);
	memcpy(data, this->mipmaps[0].data, getDataSize());

	FreeImage_Save((FREE_IMAGE_FORMAT)fif, fib, fileName.c_str());

	FreeImage_Unload(fib);
}



//!
void scale(byte *input, ushort inputWidth, ushort inputHeight, byte *output, ushort outputWidth, ushort outputHeight, byte componentsNum)
{
	float stepX = (float)inputWidth / (float)outputWidth;
	float stepY = (float)inputHeight / (float)outputHeight;

	for (int j = 0; j < outputHeight; j++)
	{
		for (int i = 0; i < outputWidth; i++)
		{
			uint outputPixelIndex = j*outputWidth + i;
			uint inputPixelIndex = (uint)(stepY*j)*inputWidth + (uint)(stepX*i);

			for (byte k = 0; k < componentsNum; k++)
			{
				output[outputPixelIndex * componentsNum + k] = input[inputPixelIndex * componentsNum + k];
			}
		}
	}

/*	float stepX = (float)inputWidth / (float)outputWidth;
	float stepY = (float)inputHeight / (float)outputHeight;

	for (int j = 0; j < outputHeight; j++)
	{
		for (int i = 0; i < outputWidth; i++)
		{
			float minX = i*stepX;
			float maxX = (i + 1)*stepX;
			float minY = j*stepY;
			float maxY = (j + 1)*stepY;
		}
	}*/
/*
	for (int j = 0; j < outputHeight; j++)
	{
		for (int i = 0; i < outputWidth; i++)
		{
			uint outputPixelIndex = j*outputWidth + i;
			uint inputPixel00Index = (2*j)*inputWidth + (2*i);
			uint inputPixel10Index = (2*j)*inputWidth + (2*i + 1);
			uint inputPixel01Index = (2*j + 1)*inputWidth + (2*i);
			uint inputPixel11Index = (2*j + 1)*inputWidth + (2*i + 1);
			
			for (int k = 0; k < componentsNum; k++)
			{
				output[outputPixelIndex * componentsNum + k] = (byte)(
					(int)input[inputPixel00Index * componentsNum + k] +
					(int)input[inputPixel10Index * componentsNum + k] +
					(int)input[inputPixel01Index * componentsNum + k] +
					(int)input[inputPixel11Index * componentsNum + k]);
			}
		}
	}*/
}

template<>
void CImage<byte>::updateMipmaps()
{
	for (int i = 1; i < mipmapsNum; i++)
	{
		scale(mipmaps[i-1].data, mipmaps[i-1].width, mipmaps[i-1].height, mipmaps[i].data, mipmaps[i].width, mipmaps[i].height, componentsNum);
	}
}
