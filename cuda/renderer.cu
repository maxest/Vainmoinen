#ifdef VRENDERER_CUDA

#include <vainmoinen/renderer.hpp>



cudaStream_t stream1, stream2;

cudaGraphicsResource* colorBuffer_cudaResource;
uchar4 *colorBuffer;

float *depthBuffer;

#ifndef USE_CONSTANT_MEMORY
	TriangleToRasterize_CUDA *trianglesToRasterizeBuffer;
#else
	__constant__ TriangleToRasterize_CUDA trianglesToRasterizeBuffer[256];
#endif
uint *indicesToTrianglesToRasterizeBuffer_CPU;
uint *indicesToTrianglesToRasterizeBuffer;



void CVRenderer::create_CUDA()
{
	cudaDeviceProp prop;
	int device;

	memset(&prop, 0, sizeof(cudaDeviceProp));
	prop.major = 1;
	prop.minor = 0;
	cudaChooseDevice(&device, &prop);

	cudaGLSetGLDevice(device);

	glewInit();

	//

	cudaStreamCreate(&stream1);
	cudaStreamCreate(&stream2);

	glGenBuffers(1, &colorBuffer_GL_pbo);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, colorBuffer_GL_pbo);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, 4 * screenWidth * screenHeight, NULL, GL_STREAM_DRAW);

	cudaGraphicsGLRegisterBuffer(&colorBuffer_cudaResource, colorBuffer_GL_pbo, cudaGraphicsMapFlagsNone);

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &colorBuffer_GL_texture);
	glBindTexture(GL_TEXTURE_2D, colorBuffer_GL_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, screenWidth, screenHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	cudaMalloc((void**)&depthBuffer, 4 * screenWidth * screenHeight);

	#ifndef USE_CONSTANT_MEMORY
		cudaMalloc((void**)&trianglesToRasterizeBuffer, TRIANGLES_PER_PACK * sizeof(TriangleToRasterize));
	#endif
	cudaHostAlloc((void**)&indicesToTrianglesToRasterizeBuffer_CPU, screenWidth_tiles * screenHeight_tiles * TRIANGLES_PER_PACK * sizeof(uint), cudaHostAllocDefault);
	cudaMalloc((void**)&indicesToTrianglesToRasterizeBuffer, screenWidth_tiles * screenHeight_tiles * TRIANGLES_PER_PACK * sizeof(uint));
}



void CVRenderer::destroy_CUDA()
{
	#ifndef USE_CONSTANT_MEMORY
		cudaFree(trianglesToRasterizeBuffer);
	#endif
	cudaFreeHost(indicesToTrianglesToRasterizeBuffer_CPU);
	cudaFree(indicesToTrianglesToRasterizeBuffer);

	cudaFree(depthBuffer);

	glBindTexture(GL_TEXTURE_2D, colorBuffer_GL_texture);
	glDeleteTextures(1, &colorBuffer_GL_texture);

	cudaGraphicsUnregisterResource(colorBuffer_cudaResource);

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, colorBuffer_GL_pbo);
	glDeleteBuffers(1, &colorBuffer_GL_pbo);

	cudaStreamDestroy(stream1);
	cudaStreamDestroy(stream2);
}



__global__ void clearBuffers(int screenWidth, uchar4 *colorBuffer, float *depthBuffer)
{
	int x = threadIdx.x + blockIdx.x*blockDim.x;
	int y = threadIdx.y + blockIdx.y*blockDim.y;
	int index = x + y*screenWidth;

	colorBuffer[index].x = 0;
	colorBuffer[index].y = 0;
	colorBuffer[index].z = 0;
	colorBuffer[index].w = 255;

	depthBuffer[index] = 1.0f;
}



#ifndef USE_CONSTANT_MEMORY
	__global__ void rasterize(int screenWidth, int screenWidth_tiles, int y_offset, uchar4 *colorBuffer, float *depthBuffer, TriangleToRasterize_CUDA *trianglesToRasterizeBuffer, uint *indicesToTrianglesToRasterizeBuffer)
#else
	__global__ void rasterize(int screenWidth, int screenWidth_tiles, int y_offset, uchar4 *colorBuffer, float *depthBuffer, uint *indicesToTrianglesToRasterizeBuffer)
#endif
{
	int pixelX = threadIdx.x + blockIdx.x*blockDim.x;
	int pixelY = threadIdx.y + blockIdx.y*blockDim.y + y_offset;
	int pixelIndex = pixelX + pixelY*screenWidth;
	int tileIndex = blockIdx.x + screenWidth_tiles*(blockIdx.y + y_offset/16);

	if (pixelX >= screenWidth)
		return;

	for (int i = 0; i < TRIANGLES_PER_PACK; i++)
	{
		int triangleIndex = indicesToTrianglesToRasterizeBuffer[TRIANGLES_PER_PACK*tileIndex + i];

		if (triangleIndex == 0)
			break;

		TriangleToRasterize_CUDA& t = trianglesToRasterizeBuffer[triangleIndex - 1];

		// we want centers of pixels to be used in computations
		float x = (float)pixelX + 0.5f;
		float y = (float)pixelY + 0.5f;

		// affine barycentric weights
		float alpha = implicitLine_device(x, y, t.v1.position, t.v2.position) * t.one_over_v0ToLine12;
		float beta = implicitLine_device(x, y, t.v2.position, t.v0.position) * t.one_over_v1ToLine20;
		float gamma = implicitLine_device(x, y, t.v0.position, t.v1.position) * t.one_over_v2ToLine01;

		// if pixel (x, y) is inside the triangle or on one of its edges
		if (alpha >= 0 && beta >= 0 && gamma >= 0)
		{
			float z_affine = alpha*t.v0.position.z + beta*t.v1.position.z + gamma*t.v2.position.z;

			if (z_affine < depthBuffer[pixelIndex] && z_affine <= 1.0f)
			{
				// perspective-correct barycentric weights
				float l = alpha*t.one_over_z0 + beta*t.one_over_z1 + gamma*t.one_over_z2;
				l = 1.0f / l;
				alpha *= l*t.one_over_z0;
				beta *= l*t.one_over_z1;
				gamma *= l*t.one_over_z2;

				// attributes interpolation
				vec3_CUDA color_persp = alpha*t.v0.color + beta*t.v1.color + gamma*t.v2.color;

				// run pixel shader
				vec3_CUDA pixelColor = color_persp;

				// clamp bytes to 255
				byte red = (byte)(255.0f * MIN(pixelColor.x, 1.0f));
				byte green = (byte)(255.0f * MIN(pixelColor.y, 1.0f));
				byte blue = (byte)(255.0f * MIN(pixelColor.z, 1.0f));

				// update buffers
				colorBuffer[pixelIndex].x = red;
				colorBuffer[pixelIndex].y = green;
				colorBuffer[pixelIndex].z = blue;
				depthBuffer[pixelIndex] = z_affine;
			}
		}
	}
}



void CVRenderer::runPixelProcessor_CUDA()
{
	size_t size;
	cudaGraphicsMapResources(1, &colorBuffer_cudaResource, NULL);
	cudaGraphicsResourceGetMappedPointer((void**)&colorBuffer, &size, colorBuffer_cudaResource);

	// clear buffers
	dim3 blocks(screenWidth_tiles, screenHeight_tiles);
	dim3 threads(16, 16);
	clearBuffers<<<blocks, threads>>>(screenWidth, colorBuffer, depthBuffer);

	if (trianglesToRasterize.size() == 0)
	{
		cudaGraphicsUnmapResources(1, &colorBuffer_cudaResource, NULL);
		return;
	}

	int packsNum = (trianglesToRasterize.size() - 1) / TRIANGLES_PER_PACK + 1;
	int remainingTrianglesToRasterizeNum = trianglesToRasterize.size();

	for (int k = 0; k < packsNum; k++)
	{
		int trianglesToRasterizeNumInCurrentPack;

		if (remainingTrianglesToRasterizeNum >= TRIANGLES_PER_PACK)
		{
			trianglesToRasterizeNumInCurrentPack = TRIANGLES_PER_PACK;
			remainingTrianglesToRasterizeNum -= TRIANGLES_PER_PACK;
		}
		else
		{
			trianglesToRasterizeNumInCurrentPack = remainingTrianglesToRasterizeNum;
		}

		memset(indicesToTrianglesToRasterizeBuffer_CPU, 0, screenWidth_tiles * screenHeight_tiles * TRIANGLES_PER_PACK * sizeof(uint));

		for (int i = 0; i < screenWidth_tiles * screenHeight_tiles; i++)
		{
			int tileX = i % screenWidth_tiles;
			int tileY = i / screenWidth_tiles;
			int offset = 0;

			for (int j = 0; j < trianglesToRasterizeNumInCurrentPack; j++)
			{
				if (trianglesToRasterize[TRIANGLES_PER_PACK*k + j].coversTile(tileX, tileY))
				{
					indicesToTrianglesToRasterizeBuffer_CPU[TRIANGLES_PER_PACK*i + offset] = j + 1;
					offset++;
				}
			}
		}

		//

		// copy trianglesToRasterizeBuffer to GPU
		#ifndef USE_CONSTANT_MEMORY
			cudaMemcpy(trianglesToRasterizeBuffer, &trianglesToRasterize[TRIANGLES_PER_PACK*k], trianglesToRasterizeNumInCurrentPack * sizeof(TriangleToRasterize), cudaMemcpyHostToDevice);
		#else
			cudaMemcpyToSymbol(trianglesToRasterizeBuffer, &trianglesToRasterize[TRIANGLES_PER_PACK*k], trianglesToRasterizeNumInCurrentPack * sizeof(TriangleToRasterize));
		#endif

		#ifndef STREAMS_FREQUENCY_DIVIDER
			// copy indicesToTrianglesToRasterizeBuffer to GPU
			cudaMemcpy(indicesToTrianglesToRasterizeBuffer, indicesToTrianglesToRasterizeBuffer_CPU, screenWidth_tiles * screenHeight_tiles * TRIANGLES_PER_PACK * sizeof(uint), cudaMemcpyHostToDevice);

			// clear buffers and rasterize
			dim3 blocks(screenWidth_tiles, screenHeight_tiles);
			dim3 threads(16, 16);
			#ifndef USE_CONSTANT_MEMORY
				rasterize<<<blocks, threads>>>(screenWidth, screenWidth_tiles, 0, colorBuffer, depthBuffer, trianglesToRasterizeBuffer, indicesToTrianglesToRasterizeBuffer);
			#else
				rasterize<<<blocks, threads>>>(screenWidth, screenWidth_tiles, 0, colorBuffer, depthBuffer, indicesToTrianglesToRasterizeBuffer);
			#endif
		#else
			for (int i = 0; i < screenHeight_tiles/STREAMS_FREQUENCY_DIVIDER; i += 2)
			{
				int offset1 = i * screenWidth_tiles * TRIANGLES_PER_PACK * STREAMS_FREQUENCY_DIVIDER;
				int offset2 = (i+1) * screenWidth_tiles * TRIANGLES_PER_PACK * STREAMS_FREQUENCY_DIVIDER;

				// copy indicesToTrianglesToRasterizeBuffer to GPU
				cudaMemcpyAsync(indicesToTrianglesToRasterizeBuffer + offset1, indicesToTrianglesToRasterizeBuffer_CPU + offset1, STREAMS_FREQUENCY_DIVIDER * screenWidth_tiles * TRIANGLES_PER_PACK * sizeof(uint), cudaMemcpyHostToDevice, stream1);
				cudaMemcpyAsync(indicesToTrianglesToRasterizeBuffer + offset2, indicesToTrianglesToRasterizeBuffer_CPU + offset2, STREAMS_FREQUENCY_DIVIDER * screenWidth_tiles * TRIANGLES_PER_PACK * sizeof(uint), cudaMemcpyHostToDevice, stream2);

				// rasterize
				dim3 blocks(screenWidth_tiles, STREAMS_FREQUENCY_DIVIDER);
				dim3 threads(16, 16);
				#ifndef USE_CONSTANT_MEMORY
					rasterize<<<blocks, threads, 0, stream1>>>(screenWidth, screenWidth_tiles, STREAMS_FREQUENCY_DIVIDER * 16 * i, colorBuffer, depthBuffer, trianglesToRasterizeBuffer, indicesToTrianglesToRasterizeBuffer);
					rasterize<<<blocks, threads, 0, stream2>>>(screenWidth, screenWidth_tiles, STREAMS_FREQUENCY_DIVIDER * 16 * (i+1), colorBuffer, depthBuffer, trianglesToRasterizeBuffer, indicesToTrianglesToRasterizeBuffer);
				#else
					rasterize<<<blocks, threads, 0, stream1>>>(screenWidth, screenWidth_tiles, STREAMS_FREQUENCY_DIVIDER * 16 * i, colorBuffer, depthBuffer, indicesToTrianglesToRasterizeBuffer);
					rasterize<<<blocks, threads, 0, stream2>>>(screenWidth, screenWidth_tiles, STREAMS_FREQUENCY_DIVIDER * 16 * (i+1), colorBuffer, depthBuffer, indicesToTrianglesToRasterizeBuffer);
				#endif
			}

			cudaStreamSynchronize(stream1);
			cudaStreamSynchronize(stream2);
		#endif
	}

	cudaGraphicsUnmapResources(1, &colorBuffer_cudaResource, NULL);
}



#endif
