#ifndef RENDERER_RENDERER_OGL_H
#define RENDERER_RENDERER_OGL_H

#include "shader_ogl.hpp"
#include "texture_ogl.hpp"
#include <common/common.hpp>
#include <math/vector.hpp>

#include <gl/glew.h>

#define MAX_ATTRIBUTES_NUM	16
#define MAX_SAMPLERS_NUM	8



class CVertexDeclaration;
class CVertexBuffer;
class CIndexBuffer;



namespace CullMode
{
	enum TYPE
	{
		None,
		CW,
		CCW
	};
}

namespace TestingFunction
{
	enum TYPE
	{
		Never = GL_NEVER,
		Less = GL_LESS,
		Equal = GL_EQUAL,
		LessEqual = GL_LEQUAL,
		Greater = GL_GREATER,
		NotEqual = GL_NOTEQUAL,
		GreaterEqual = GL_GEQUAL,
		Always = GL_ALWAYS
	};
}

namespace StencilOperation
{
	enum TYPE
	{
		Keep = GL_KEEP,
		Zero = GL_ZERO,
		Replace = GL_REPLACE,
		Increase = GL_INCR_WRAP,
		Decrease = GL_DECR_WRAP
	};
}

namespace BlendingFunction
{
	enum TYPE
	{
		Zero = GL_ZERO,
		One = GL_ONE,
		SrcColor = GL_SRC_COLOR,
		InvSrcColor = GL_ONE_MINUS_SRC_COLOR,
		SrcAlpha = GL_SRC_ALPHA,
		InvSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,
		DestColor = GL_DST_COLOR,
		InvDestColor = GL_ONE_MINUS_DST_COLOR,
		DestAlpha = GL_DST_ALPHA,
		InvDestAlpha = GL_ONE_MINUS_DST_ALPHA
	};
}

namespace PrimitiveType
{
	enum TYPE
	{
		PointList = GL_POINTS,
		LineList = GL_LINES,
		LineStrip = GL_LINE_STRIP,
		TriangleList = GL_TRIANGLES,
		TriangleStrip = GL_TRIANGLE_STRIP,
		TriangleFan = GL_TRIANGLE_FAN
	};
}



class CRenderer
{
	friend class CVertexBuffer;
	friend class CIndexBuffer;
	friend class CShader;

	struct ShaderProgram
	{
		struct LocationIntPair
		{
			int location;
			int value;
		};
		struct LocationFloatPair
		{
			int location;
			float value;
		};
		struct LocationVectorPair
		{
			int location;
			vec4 value;
		};
		struct LocationMatrixPair
		{
			int location;
			mtx value;
		};
		struct LocationMatrixArrayPair
		{
			int location;
			MatrixArray value;
		};

		uint id;

		map<string, LocationIntPair> ints;
		map<string, LocationFloatPair> floats;
		map<string, LocationVectorPair> vectors;
		map<string, LocationMatrixPair> matrices;
		map<string, LocationMatrixArrayPair> matricesArrays;

		bool setInt(const string& name, int i)
		{
			map<string, LocationIntPair>::iterator it = ints.find(name);

			if (it != ints.end())
			{
				if (it->second.value != i)
				{
					it->second.value = i;
					glUniform1i(it->second.location, i);
				}
			}
			else
			{
				int location = glGetUniformLocation(id, name.c_str());
				if (location == -1)
					return false;
				glUniform1i(location, i);

				LocationIntPair pair;
				pair.location = location;
				pair.value = i;
				ints[name] = pair;
			}

			return true;
		}

		bool setFloat(const string& name, float f)
		{
			map<string, LocationFloatPair>::iterator it = floats.find(name);

			if (it != floats.end())
			{
				if (it->second.value != f)
				{
					it->second.value = f;
					glUniform1f(it->second.location, f);
				}
			}
			else
			{
				int location = glGetUniformLocation(id, name.c_str());
				if (location == -1)
					return false;
				glUniform1f(location, f);

				LocationFloatPair pair;
				pair.location = location;
				pair.value = f;
				floats[name] = pair;
			}

			return true;
		}

		bool setVector(const string& name, const vec4& v)
		{
			map<string, LocationVectorPair>::iterator it = vectors.find(name);

			if (it != vectors.end())
			{
				if (it->second.value != v)
				{
					it->second.value = v;
					glUniform4f(it->second.location, v.x, v.y, v.z, v.w);
				}
			}
			else
			{
				int location = glGetUniformLocation(id, name.c_str());
				if (location == -1)
					return false;
				glUniform4f(location, v.x, v.y, v.z, v.w);

				LocationVectorPair pair;
				pair.location = location;
				pair.value = v;
				vectors[name] = pair;
			}

			return true;
		}

		bool setMatrix(const string& name, const mtx& m)
		{
			map<string, LocationMatrixPair>::iterator it = matrices.find(name);

			if (it != matrices.end())
			{
				if (it->second.value != m)
				{
					it->second.value = m;
					glUniformMatrix4fv(it->second.location, 1, true, &m._[0][0]);
				}
			}
			else
			{
				int location = glGetUniformLocation(id, name.c_str());
				if (location == -1)
					return false;
				glUniformMatrix4fv(location, 1, true, &m._[0][0]);

				LocationMatrixPair pair;
				pair.location = location;
				pair.value = m;
				matrices[name] = pair;
			}

			return true;
		}

		bool setMatrixArray(const string& name, const MatrixArray& ma)
		{
			map<string, LocationMatrixArrayPair>::iterator it = matricesArrays.find(name);

			if (it != matricesArrays.end())
			{
				if (it->second.value != ma)
				{
					it->second.value = ma;
					glUniformMatrix4fv(it->second.location, it->second.value.count, true, &it->second.value.matrices[0]._[0][0]);
				}
			}
			else
			{
				int location = glGetUniformLocation(id, name.c_str());
				if (location == -1)
					return false;
				glUniformMatrix4fv(location, ma.count, true, &ma.matrices[0]._[0][0]);

				LocationMatrixArrayPair pair;
				pair.location = location;
				pair.value = ma;
				matricesArrays[name] = pair;
			}

			return true;
		}
	};

public:
	void create(int screenWidth, int screenHeight, bool fullScreen, bool vSync);
	void destroy();

	void beginScene();
	void endScene();

	void clear(bool target, bool depth, bool stencil, const vec3& targetColor);

	void beginOcclusionQuery();
	int endOcclusionQuery(bool immediate = false);

	void setViewport(int x, int y, int width, int height); // (x, y) is an upper-left corner
	void setDepthRange(float minZ, float maxZ);
	void setCullMode(CullMode::TYPE cullMode);
	void setScissorTestState(bool state);
	void setScissorTestRect(int x, int y, int width, int height); // (x, y) is an upper-left corner

	void setTargetWriteState(bool state);

	void setDepthWriteState(bool state);
	void setDepthTestingFunction(TestingFunction::TYPE testingFunction);

	void setStencilState(bool state);
	void setStencilTwoSidedState(bool state);
	void setStencilFunction(TestingFunction::TYPE testingFunction, byte reference, byte mask);
	void setStencilOperation(StencilOperation::TYPE fail, StencilOperation::TYPE zfail, StencilOperation::TYPE pass);
	void setStencilOperationBackFace(StencilOperation::TYPE fail, StencilOperation::TYPE zfail, StencilOperation::TYPE pass);
	void setStencilMask(uint mask);

	void setBlendingState(bool state);
	void setBlendingFunction(BlendingFunction::TYPE srcBlendingFunction, BlendingFunction::TYPE destBlendingFunction);
/*
	void setRenderTargetToBackBuffer(byte renderTargetIndex);
	void setRenderTarget(byte renderTargetIndex, const CTexture& texture, CubeMapFace::TYPE cubeMapFace = CubeMapFace::PositiveX);
	void setRenderTargetOff(byte renderTargetIndex);
	void setDepthStencilSurfaceToBackBuffer();
	void setDepthStencilSurface(const CTexture& texture);
*/
	void setVertexDeclaration(const CVertexDeclaration& vertexDeclaration);
	void setVertexBuffer(const CVertexBuffer& vertexBuffer, byte stride, uint offset, byte dummy);
	void setVertexBuffer(const CVertexBuffer& vertexBuffer, uint offset = 0, byte dummy = 0); // takes stride based on the currently bound vertex declaration
	void setIndexBuffer(const CIndexBuffer& indexBuffer);
	void setVertexShader(const CVertexShader& vertexShader);
	void setPixelShader(const CPixelShader& pixelShader);
	void setTexture(int samplerIndex, const string& samplerName, const CTexture& texture);
	void setTextureOff(int samplerIndex);
	void setSamplerFiltering_d3d9(int index, SamplerFiltering::TYPE mag, SamplerFiltering::TYPE min, SamplerFiltering::TYPE mip) {}
	void setSamplerAddressing_d3d9(int index, SamplerAddressing::TYPE addressing) {}
	void setSamplerBorderColor_d3d9(int index, const vec4& borderColor) {}

	void setVertexShaderConstant(const string& name, int i) { currentVertexShader->constantTable.setInt(name, i); }
	void setVertexShaderConstant(const string& name, float f) { currentVertexShader->constantTable.setFloat(name, f); }
	void setVertexShaderConstant(const string& name, float x, float y, float z, float w) { currentVertexShader->constantTable.setVector(name, vec4(x, y, z, w)); }
	void setVertexShaderConstant(const string& name, const vec2& v, float z, float w) { currentVertexShader->constantTable.setVector(name, vec4(v.x, v.y, z, w)); }
	void setVertexShaderConstant(const string& name, const vec3& v, float w) { currentVertexShader->constantTable.setVector(name, vec4(v.x, v.y, v.z, w)); }
	void setVertexShaderConstant(const string& name, const vec4& v) { currentVertexShader->constantTable.setVector(name, v); }
	void setVertexShaderConstant(const string& name, const mtx& m) { currentVertexShader->constantTable.setMatrix(name, m); }
	void setVertexShaderConstant(const string& name, const MatrixArray& ma) { currentVertexShader->constantTable.setMatrixArray(name, ma); }

	void setPixelShaderConstant(const string& name, int i) { currentPixelShader->constantTable.setInt(name, i); }
	void setPixelShaderConstant(const string& name, float f) { currentPixelShader->constantTable.setFloat(name, f); }
	void setPixelShaderConstant(const string& name, float x, float y, float z, float w) { currentPixelShader->constantTable.setVector(name, vec4(x, y, z, w)); }
	void setPixelShaderConstant(const string& name, const vec2& v, float z, float w) { currentPixelShader->constantTable.setVector(name, vec4(v.x, v.y, z, w)); }
	void setPixelShaderConstant(const string& name, const vec3& v, float w) { currentPixelShader->constantTable.setVector(name, vec4(v.x, v.y, v.z, w)); }
	void setPixelShaderConstant(const string& name, const vec4& v) { currentPixelShader->constantTable.setVector(name, v); }
	void setPixelShaderConstant(const string& name, const mtx& m) { currentPixelShader->constantTable.setMatrix(name, m); }
	void setPixelShaderConstant(const string& name, const MatrixArray& ma) { currentPixelShader->constantTable.setMatrixArray(name, ma); }

	void drawPrimitives(PrimitiveType::TYPE primitiveType, int primitivesNum, int startVertex);
	void drawIndexedPrimitives(PrimitiveType::TYPE primitiveType, int primitivesNum, int startIndex, int startVertex, int verticesNum);

private:
	int convertPrimitivesNumToVerticesNum(PrimitiveType::TYPE primitiveType, int primitivesNum)
	{
		if (primitiveType == PrimitiveType::PointList)
			return primitivesNum;
		else if (primitiveType == PrimitiveType::LineList)
			return 2*primitivesNum;
		else if (primitiveType == PrimitiveType::LineStrip)
			return primitivesNum + 1;
		else if (primitiveType == PrimitiveType::TriangleList)
			return 3*primitivesNum;
		else if (primitiveType == PrimitiveType::TriangleStrip)
			return primitivesNum + 2;
		else if (primitiveType == PrimitiveType::TriangleFan)
			return primitivesNum + 2;
		return 0;
	}
	void preDraw();
	void postDraw();

private:
	int screenWidth, screenHeight; // needed for viewport and scissor area

	map<string, ShaderProgram> shaderPrograms; // it's a list of all used shaders and vertex declarations combinations (I hate the way GLSL handles shaders)
	uint previousShaderProgramID;

	const CVertexDeclaration* currentVertexDeclaration;
	const CVertexBuffer* currentVertexBuffer;
	const CIndexBuffer* currentIndexBuffer;
	const CVertexShader* currentVertexShader;
	const CPixelShader* currentPixelShader;
	const CTexture* currentTextures[MAX_SAMPLERS_NUM];

	byte activeVertexAttribArraysNum;
};
extern CRenderer Renderer;



#endif
