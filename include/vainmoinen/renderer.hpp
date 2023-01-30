#ifndef VAINMOINEN_RENDERER_H
#define VAINMOINEN_RENDERER_H

#include "geometry.hpp"
#include "triangles_buffer.hpp"
#include "texture.hpp"
#include <common/common.hpp>
#include <math/main.hpp>
#ifdef VRENDERER_DIRECT_COMPUTE
	#include <renderer_d3d11/renderer_d3d11.hpp>
#endif

#ifdef VRENDERER_CUDA
	#include <GL/glew.h>
	#include <cuda.h>
	#include <cuda_gl_interop.h>
#endif

#define TRIANGLES_PER_PACK	256
#ifdef VRENDERER_CUDA
//	#define STREAMS_FREQUENCY_DIVIDER	1
//	#define USE_CONSTANT_MEMORY
#endif



// ----------------------------------------------------------------------------



struct TriangleToRasterize
{
	Vertex v0;
	Vertex v1;
	Vertex v2;
	const CTexture* texture;
	float one_over_z0;
	float one_over_z1;
	float one_over_z2;
	int minX;
	int maxX;
	int minY;
	int maxY;
	float one_over_v0ToLine12;
	float one_over_v1ToLine20;
	float one_over_v2ToLine01;
	plane alphaPlane;
	plane betaPlane;
	plane gammaPlane;
	float one_over_alpha_c;
	float one_over_beta_c;
	float one_over_gamma_c;
	float alpha_ffx;
	float beta_ffx;
	float gamma_ffx;
	float alpha_ffy;
	float beta_ffy;
	float gamma_ffy;
	int tileMinX;
	int tileMaxX;
	int tileMinY;
	int tileMaxY;
	int padding[1];

	bool coversTile(int tileX, int tileY)
	{
		return ( (tileX >= tileMinX) && (tileX <= tileMaxX) && (tileY >= tileMinY) && (tileY <= tileMaxY) );
	}
};



// ----------------------------------------------------------------------------



#ifdef VRENDERER_CUDA
	class vec2_CUDA
	{
	public:
		__device__ vec2_CUDA operator + (const vec2_CUDA& v) const
		{
			vec2_CUDA temp;

			temp.x = x + v.x;
			temp.y = y + v.y;

			return temp;
		}

		__device__ vec2_CUDA operator * (float s) const
		{
			vec2_CUDA temp;

			temp.x = x * s;
			temp.y = y * s;

			return temp;
		}

		__device__ friend vec2_CUDA operator * (const float& s, const vec2_CUDA& v)
		{
			return v * s;
		}

	public:
		float x, y;
	};



	class vec3_CUDA
	{
	public:
		__device__ vec3_CUDA operator + (const vec3_CUDA& v) const
		{
			vec3_CUDA temp;

			temp.x = x + v.x;
			temp.y = y + v.y;
			temp.z = z + v.z;

			return temp;
		}

		__device__ vec3_CUDA operator * (float s) const
		{
			vec3_CUDA temp;

			temp.x = x * s;
			temp.y = y * s;
			temp.z = z * s;
			
			return temp;
		}

		__device__ friend vec3_CUDA operator * (const float& s, const vec3_CUDA& v)
		{
			return v * s;
		}

	public:
		float x, y, z;
	};



	class vec4_CUDA
	{
	public:
		__device__ vec4_CUDA operator + (const vec4_CUDA& v) const
		{
			vec4_CUDA temp;

			temp.x = x + v.x;
			temp.y = y + v.y;
			temp.z = z + v.z;
			temp.w = w + v.w;

			return temp;
		}

		__device__ vec4_CUDA operator * (float s) const
		{
			vec4_CUDA temp;

			temp.x = x * s;
			temp.y = y * s;
			temp.z = z * s;
			temp.w = w * s;
			
			return temp;
		}

		__device__ friend vec4_CUDA operator * (const float& s, const vec4_CUDA& v)
		{
			return v * s;
		}

	public:
		float x, y, z, w;
	};



	class plane_CUDA
	{
	public:
		float a, b, c, d;
	};



	inline __device__ float implicitLine_device(float x, float y, const vec4_CUDA& v1, const vec4_CUDA& v2)
	{
		return (v1.y - v2.y)*x + (v2.x - v1.x)*y + v1.x*v2.y - v2.x*v1.y;
	}



	struct Vertex_CUDA
	{
		vec4_CUDA position;
		vec3_CUDA color;
		vec2_CUDA texCoord;
	};



	struct TriangleToRasterize_CUDA
	{
		Vertex_CUDA v0;
		Vertex_CUDA v1;
		Vertex_CUDA v2;
		int textureAddress;
		float one_over_z0;
		float one_over_z1;
		float one_over_z2;
		int minX;
		int maxX;
		int minY;
		int maxY;
		float one_over_v0ToLine12;
		float one_over_v1ToLine20;
		float one_over_v2ToLine01;
		plane_CUDA alphaPlane;
		plane_CUDA betaPlane;
		plane_CUDA gammaPlane;
		float one_over_alpha_c;
		float one_over_beta_c;
		float one_over_gamma_c;
		float alpha_ffx;
		float beta_ffx;
		float gamma_ffx;
		float alpha_ffy;
		float beta_ffy;
		float gamma_ffy;
		int tileMinX;
		int tileMaxX;
		int tileMinY;
		int tileMaxY;
		int padding[1];
	};
#endif



// ----------------------------------------------------------------------------



class CVRenderer
{
	struct DrawCall
	{
		const CTrianglesBuffer* trianglesBuffer;
		uint trianglesNum;
		mtx transform;
		const CTexture* texture;
	};

public:
	#ifdef VRENDERER_DIRECT_COMPUTE
		void create(const string& shadersDir = "");
	#else
		void create();
	#endif
	void destroy();

	void setViewport(ushort screenWidth, ushort screenHeight);
	void setTrianglesBuffer(const CTrianglesBuffer& trianglesBuffer);
	void setTransform(const mtx& transform);
	void setTexture(const CTexture& texture);

	void draw(ushort trianglesNum = 0);

	void begin();
	void end();

	#ifdef VRENDERER_CUDA
		void create_CUDA();
		void destroy_CUDA();
	#endif

private:
	void processProspectiveTriangleToRasterize(const Vertex& _v0, const Vertex& _v1, const Vertex& _v2, const CTexture* _texture);
	vector<Vertex> clipPolygonToPlaneIn4D(const vector<Vertex>& vertices, const vec4& planeNormal);
	Vertex runVertexShader(const mtx& transform, const Vertex& input);
	void runVertexProcessor();

	vec3 tex2D_point(const CTexture* texture, const vec2& imageTexCoord, byte mipmapIndex);
	vec3 tex2D_linear(const CTexture* texture, const vec2& imageTexCoord, byte mipmapIndex);
	vec3 tex2D_linear_mipmap_point(const CTexture* texture, const vec2& texCoord);
	vec3 tex2D_linear_mipmap_linear(const CTexture* texture, const vec2& texCoord);
	vec3 tex2D(const CTexture* texture, const vec2& texCoord);
	vec3 runPixelShader(const CTexture* texture, const vec3& color, const vec2& texCoord);
	void runPixelProcessor();

	#ifdef VRENDERER_CUDA
		void runPixelProcessor_CUDA();
	#endif

private:
	ushort screenWidth, screenHeight;
	int screenWidth_tiles, screenHeight_tiles;

	#ifdef VRENDERER_SOFTWARE
		SDL_Surface* screenSurface;
		byte *colorBuffer;
		float *depthBuffer;
	#elif VRENDERER_DIRECT_COMPUTE
		ID3D11Device* D3D11Device;
		ID3D11DeviceContext* D3D11DeviceContext;

		ID3D11Texture2D* colorBufferTexture;
		ID3D11ShaderResourceView* colorBufferSRV;
		ID3D11UnorderedAccessView* colorBufferUAV;
		ID3D11Texture2D* depthBufferTexture;
		ID3D11UnorderedAccessView* depthBufferUAV;

		ID3D11Buffer* trianglesToRasterizeBuffer_CPU; // helper buffer
		ID3D11Buffer* trianglesToRasterizeBuffer;
		ID3D11Buffer* indicesToTrianglesToRasterizeBuffer_CPU;
		ID3D11Buffer* indicesToTrianglesToRasterizeBuffer;
		ID3D11UnorderedAccessView* trianglesToRasterizeBufferUAV;
		ID3D11UnorderedAccessView* indicesToTrianglesToRasterizeBufferUAV;

		ID3D11VertexShader* screenQuadVertexShader;
		ID3D11PixelShader* screenPixelShader;
		ID3D11ComputeShader* clearBuffersComputeShader;
		ID3D11ComputeShader* rasterizeComputeShader;

		ID3D11Buffer* rasterizeComputeShader_constantBuffer;

		ID3D11SamplerState* pointSampler;
	#elif VRENDERER_CUDA
		GLuint colorBuffer_GL_pbo;
		GLuint colorBuffer_GL_texture;
	#endif

	mtx windowTransform;

	mtx currentTransform;
	const CTrianglesBuffer* currentTrianglesBuffer;
	const CTexture* currentTexture;

	vector<DrawCall> drawCalls;
	vector<TriangleToRasterize> trianglesToRasterize; // holds copies of all triangles from called triangles buffers that are to be rendered so the pipeline can work on them instead of on the original triangles

	float dudx, dudy, dvdx, dvdy; // partial derivatives of U/V coordinates with respect to X/Y pixel's screen coordinates
};
extern CVRenderer VRenderer;



inline mtx constructViewTransform(const vec3& position, const vec3& rightVector, const vec3& upVector, const vec3& forwardVector)
{
	mtx translate = mtx::translate(-position);

	mtx rotate(rightVector.x,	upVector.x,		forwardVector.x,	0.0f,
			   rightVector.y,	upVector.y,		forwardVector.y,	0.0f,
			   rightVector.z,	upVector.z,		forwardVector.z,	0.0f,
			   0.0f,			0.0f,			0.0f,				1.0f);

	return translate * rotate;
}

inline mtx constructProjTransform(float left, float right, float bottom, float top, float zNear, float zFar)
{
	mtx perspective(zNear,	0.0f,	0.0f,			0.0f,
					0.0f,	zNear,	0.0f,			0.0f,
					0.0f,	0.0f,	zNear + zFar,	-1.0f,
					0.0f,	0.0f,	zNear * zFar,	0.0f);

	mtx orthogonal(2.0f / (right - left),				0.0f,								0.0f,								0.0f,
				   0.0f,								2.0f / (top - bottom),				0.0f,								0.0f,
				   0.0f,								0.0f,								-2.0f / (zFar - zNear),				0.0f,
				   -(right + left) / (right - left),	-(top + bottom) / (top - bottom),	-(zNear + zFar) / (zFar - zNear),	1.0f);
	
	return perspective * orthogonal;
}



#endif
