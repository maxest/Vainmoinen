#ifndef RENDERER_OGL_SHADER_H
#define RENDERER_OGL_SHADER_H

#include <common/common.hpp>
#include <math/vector.hpp>
#include <math/matrix.hpp>

#include <GL/glew.h>



class CRenderer;



namespace ShaderType
{
	enum TYPE
	{
		Vertex = GL_VERTEX_SHADER,
		Pixel = GL_FRAGMENT_SHADER
	};
}



class CShader
{
	friend class CRenderer;

	struct ConstantTable
	{
		map<string, int> ints;
		map<string, float> floats;
		map<string, vec4> vectors;
		map<string, mtx> matrices;
		map<string, MatrixArray> matricesArrays;

		void setInt(const string& name, int i)
		{
			ints[name] = i;
		}

		void setFloat(const string& name, float f)
		{
			floats[name] = f;
		}

		void setVector(const string& name, const vec4& v)
		{
			vectors[name] = v;
		}

		void setMatrix(const string& name, const mtx& m)
		{
			matrices[name] = m;
		}

		void setMatrixArray(const string& name, const MatrixArray& ma)
		{
			matricesArrays[name] = ma;
		}
	};

public:
	CShader() { id = 0; }
	~CShader() { destroy(); }

	void createFromFile(const string& fileName, ShaderType::TYPE type, const string& macros = "");
	void destroy();

private:
	uint id;
	mutable ConstantTable constantTable;

	string name;
};



class CVertexShader: public CShader
{
public:
	void createFromFile(const string& fileName, const string& macros = "")
	{
		CShader::createFromFile(fileName, ShaderType::Vertex, macros);
	}
};

class CPixelShader: public CShader
{
public:
	void createFromFile(const string& fileName, const string& macros = "")
	{
		CShader::createFromFile(fileName, ShaderType::Pixel, macros);
	}
};



#endif
