#ifdef RENDERER_OGL

#include <renderer_ogl/renderer_ogl.hpp>
#include <renderer_ogl/vertex_declaration_ogl.hpp>
#include <renderer_ogl/vertex_buffer_ogl.hpp>
#include <renderer_ogl/index_buffer_ogl.hpp>
#include <common/logger.hpp>
#include <math/vector.hpp>
#include <math/matrix.hpp>



CRenderer Renderer;



void CRenderer::create(int screenWidth, int screenHeight, bool fullScreen, bool vSync)
{
	glewInit();
	glEnable(GL_DEPTH_TEST);

	setCullMode(CullMode::CW);
	setDepthTestingFunction(TestingFunction::LessEqual);

	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	previousShaderProgramID = -1;

	currentVertexDeclaration = NULL;
	currentVertexBuffer = NULL;
	currentIndexBuffer = NULL;
	currentVertexShader = NULL;
	currentPixelShader = NULL;
	for (int i = 0; i < MAX_SAMPLERS_NUM; i++)
	{
		currentTextures[i] = NULL;
	}

	activeVertexAttribArraysNum = 0;

	Logger << "OK: Renderer start, OpenGL " << glGetString(GL_VERSION) << ", " << glGetString(GL_RENDERER) << endl;
}



void CRenderer::destroy()
{
	// release all shader programs (not the shaders itself; they are deleted in CShader class)
	map<string, ShaderProgram>::iterator it;
	for (it = shaderPrograms.begin(); it != shaderPrograms.end(); it++)
	{
		int attachedShadersNum;
		glGetProgramiv(it->second.id, GL_ATTACHED_SHADERS, &attachedShadersNum);

		uint *attachedShaders = new uint[attachedShadersNum];
		glGetAttachedShaders(it->second.id, attachedShadersNum, NULL, attachedShaders);

		for (int i = 0; i < attachedShadersNum; i++)
			glDetachShader(it->second.id, attachedShaders[i]);

		delete[] attachedShaders;		
		glDeleteProgram(it->second.id);
	}

	Logger << "OK: Renderer quit" << endl;
}



void CRenderer::beginScene()
{

}



void CRenderer::endScene()
{

}



void CRenderer::clear(bool target, bool depth, bool stencil, const vec3& targetColor)
{
	uint flags = 0;

	if (target)
	{
		glClearColor(targetColor.x, targetColor.y, targetColor.z, 1.0f);
		flags |= GL_COLOR_BUFFER_BIT;
	}
	if (depth)
	{
		glClearDepth(1.0f);
		flags |= GL_DEPTH_BUFFER_BIT;
	}
	if (stencil)
	{
		glClearStencil(0);
		flags |= GL_STENCIL_BUFFER_BIT;
	}

	glClear(flags);
}



void CRenderer::beginOcclusionQuery()
{

}



int CRenderer::endOcclusionQuery(bool immediate)
{
	return 0;
}



void CRenderer::setViewport(int x, int y, int width, int height)
{
	glViewport(x, screenHeight - y - height, width, height);
}



void CRenderer::setDepthRange(float minZ, float maxZ)
{
	glDepthRange(minZ, maxZ);
}



void CRenderer::setCullMode(CullMode::TYPE cullMode)
{
	switch (cullMode)
	{
	case CullMode::None:
		glDisable(GL_CULL_FACE);
		break;

	case CullMode::CW:
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
		break;

	case CullMode::CCW:
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CW);
		break;
	}
}



void CRenderer::setScissorTestState(bool state)
{
	if (state)
		glEnable(GL_SCISSOR_TEST);
	else
		glDisable(GL_SCISSOR_TEST);
}



void CRenderer::setScissorTestRect(int x, int y, int width, int height)
{
	glScissor(x, screenHeight - y - height, width, height);
}



void CRenderer::setTargetWriteState(bool state)
{
	glColorMask(state, state, state, state);
}



void CRenderer::setDepthWriteState(bool state)
{
	glDepthMask(state);
}



void CRenderer::setDepthTestingFunction(TestingFunction::TYPE testingFunction)
{
	glDepthFunc(testingFunction);
}



void CRenderer::setStencilState(bool state)
{
	if (state)
		glEnable(GL_STENCIL_TEST);
	else
		glDisable(GL_STENCIL_TEST);
}



void CRenderer::setStencilTwoSidedState(bool state)
{
	if (state)
		glEnable(GL_STENCIL_TEST_TWO_SIDE_EXT);
	else
		glDisable(GL_STENCIL_TEST_TWO_SIDE_EXT);
}



void CRenderer::setStencilFunction(TestingFunction::TYPE testingFunction, byte reference, byte mask)
{
	glActiveStencilFaceEXT(GL_FRONT);
	glStencilFunc(testingFunction, reference, mask);

	glActiveStencilFaceEXT(GL_BACK);
	glStencilFunc(testingFunction, reference, mask);
}



void CRenderer::setStencilOperation(StencilOperation::TYPE fail, StencilOperation::TYPE zfail, StencilOperation::TYPE pass)
{
	glActiveStencilFaceEXT(GL_FRONT);
	glStencilOp(fail, zfail, pass);
}



void CRenderer::setStencilOperationBackFace(StencilOperation::TYPE fail, StencilOperation::TYPE zfail, StencilOperation::TYPE pass)
{
	glActiveStencilFaceEXT(GL_BACK);
	glStencilOp(fail, zfail, pass);
}



void CRenderer::setStencilMask(uint mask)
{
	glStencilMask(mask);
}



void CRenderer::setBlendingState(bool state)
{
	if (state)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);
}



void CRenderer::setBlendingFunction(BlendingFunction::TYPE srcBlendingFunction, BlendingFunction::TYPE destBlendingFunction)
{
	glBlendFunc(srcBlendingFunction, destBlendingFunction);
}

/*

void CRenderer::setRenderTargetToBackBuffer(byte renderTargetIndex)
{
	D3DDevice->SetRenderTarget(renderTargetIndex, backBufferRenderTarget);
}



void CRenderer::setRenderTarget(byte renderTargetIndex, const CTexture& texture, CubeMapFace::TYPE cubeMapFace)
{
	if (texture.usage&  TextureUsage::RenderTarget)
	{
		D3DDevice->SetRenderTarget(renderTargetIndex, texture.surfaces[(byte)cubeMapFace]);
	}
	else
	{
		Logger << "\tERROR: Couldn't bind texture as render target: " << texture.name << endl;
		exit(1);
	}
}



void CRenderer::setRenderTargetOff(byte renderTargetIndex)
{
	D3DDevice->SetRenderTarget(renderTargetIndex, NULL);
}



void CRenderer::setDepthStencilSurfaceToBackBuffer()
{
	D3DDevice->SetDepthStencilSurface(backBufferDepthStencilSurface);
}



void CRenderer::setDepthStencilSurface(const CTexture& texture)
{
	// standard textures only (no cube)
	if ((texture.type == 0) && (texture.usage&  TextureUsage::DepthStencil))
	{
		D3DDevice->SetDepthStencilSurface(texture.surfaces[0]);
	}
	else
	{
		Logger << "\tERROR: Couldn't bind texture as depth-stencil surface: " << texture.name << endl;
		exit(1);
	}
}

*/

void CRenderer::setVertexDeclaration(const CVertexDeclaration& vertexDeclaration)
{
	currentVertexDeclaration = &vertexDeclaration;
}



void CRenderer::setVertexBuffer(const CVertexBuffer& vertexBuffer, byte stride, uint offset, byte dummy)
{
	if (currentVertexBuffer != &vertexBuffer)
	{
		currentVertexBuffer = &vertexBuffer;

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.id);

		byte oldSize = activeVertexAttribArraysNum;
		byte newSize = currentVertexDeclaration->vertexElements.size();

		if (newSize > oldSize)
		{
			for (byte i = oldSize; i < newSize; i++)
				glEnableVertexAttribArray(i);
		}
		else if (newSize < oldSize)
		{
			for (byte i = newSize; i < oldSize; i++)
				glDisableVertexAttribArray(i);
		}

		activeVertexAttribArraysNum = newSize;

		for (uint i = 0; i < newSize; i++)
		{
			glVertexAttribPointer(
				i,
				VertexElementType::getComponentsNum(currentVertexDeclaration->vertexElements[i].type),
				VertexElementType::getGLType(currentVertexDeclaration->vertexElements[i].type),
				GL_TRUE,
				stride,
				(char*)NULL + currentVertexDeclaration->vertexElements[i].offset);
		}
	}
}



void CRenderer::setVertexBuffer(const CVertexBuffer& vertexBuffer, uint offset, byte dummy)
{
	setVertexBuffer(vertexBuffer, currentVertexDeclaration->size, offset, dummy);
}



void CRenderer::setIndexBuffer(const CIndexBuffer& indexBuffer)
{
	if (currentIndexBuffer == &indexBuffer)
		return;

	currentIndexBuffer = &indexBuffer;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.id);
}



void CRenderer::setVertexShader(const CVertexShader& vertexShader)
{
	currentVertexShader = &vertexShader;
}



void CRenderer::setPixelShader(const CPixelShader& pixelShader)
{
	currentPixelShader = &pixelShader;
}



void CRenderer::setTexture(int samplerIndex, const string& samplerName, const CTexture& texture)
{
	if (currentTextures[samplerIndex] == &texture)
		return;

	currentTextures[samplerIndex] = &texture;
	currentTextures[samplerIndex]->samplerName = samplerName;

	glActiveTexture(GL_TEXTURE0 + samplerIndex);
	glBindTexture(texture.type, texture.id);
}



void CRenderer::setTextureOff(int samplerIndex)
{
	if (currentTextures[samplerIndex] == NULL)
		return;

	currentTextures[samplerIndex]->samplerName = "";

	glActiveTexture(GL_TEXTURE0 + samplerIndex);
	glBindTexture(currentTextures[samplerIndex]->type, 0);

	currentTextures[samplerIndex] = NULL;
}



void CRenderer::drawPrimitives(PrimitiveType::TYPE primitiveType, int primitivesNum, int startVertex)
{
	preDraw();
	glDrawArrays(
		primitiveType,
		startVertex,
		convertPrimitivesNumToVerticesNum(primitiveType, primitivesNum));
	postDraw();
}



void CRenderer::drawIndexedPrimitives(PrimitiveType::TYPE primitiveType, int primitivesNum, int startIndex, int startVertex, int verticesNum)
{
	preDraw();
	glDrawElements(
		primitiveType,
		convertPrimitivesNumToVerticesNum(primitiveType, primitivesNum),
		currentIndexBuffer->format == IndexBufferFormat::BIT_16 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT,
		currentIndexBuffer->format == IndexBufferFormat::BIT_16 ? (char*)NULL + startIndex*sizeof(ushort) : (char*)NULL + startIndex*sizeof(uint));
	postDraw();
}



void CRenderer::preDraw()
{
	string shaderProgramName = currentVertexShader->name + "|" + currentPixelShader->name + "|" + currentVertexDeclaration->hash;
	map<string, ShaderProgram>::iterator it = shaderPrograms.find(shaderProgramName);
	ShaderProgram* shaderProgram;

	// create a new shader program out of the current two shaders (vertex and fragment) and vertex declaration
	if (it == shaderPrograms.end())
	{
		uint newShaderProgramID = glCreateProgram();
		shaderPrograms[shaderProgramName].id = newShaderProgramID;
		shaderProgram = &shaderPrograms[shaderProgramName];

		glAttachShader(newShaderProgramID, currentVertexShader->id);
		glAttachShader(newShaderProgramID, currentPixelShader->id);

		for (uint i = 0; i < currentVertexDeclaration->vertexElements.size(); i++)
		{
			string attribName = VertexElementSemantic::toString(currentVertexDeclaration->vertexElements[i].semantic, currentVertexDeclaration->vertexElements[i].semanticIndex);
			glBindAttribLocation(newShaderProgramID, i, attribName.c_str());
		}

		glLinkProgram(newShaderProgramID);
		glUseProgram(newShaderProgramID);

		int linkingStatus;
		glGetProgramiv(newShaderProgramID, GL_LINK_STATUS, &linkingStatus);
		if (linkingStatus == GL_FALSE)
		{
			int infoLogLength = 0;
			char *infoLogData;

			glGetProgramiv(newShaderProgramID, GL_INFO_LOG_LENGTH, &infoLogLength);
			infoLogData = new char[infoLogLength];

			glGetProgramInfoLog(newShaderProgramID, infoLogLength, NULL, infoLogData);

			Logger << "\tERROR: Couldn't link program from shaders: " << currentVertexShader->name << " and " << currentPixelShader->name << "; vertex declaration: " << currentVertexDeclaration->name << endl;
			Logger << infoLogData;
			exit(1);
		}
	}
	// use the found shader program
	else
	{
		shaderProgram = &it->second;

		// select the found shader program only if it has not been selected in the previous draw call
		if (shaderProgram->id != previousShaderProgramID)
			glUseProgram(shaderProgram->id);
	}

	previousShaderProgramID = shaderProgram->id;

	map<string, int>::iterator iit;
	map<string, float>::iterator fit;
	map<string, vec4>::iterator vit;
	map<string, mtx>::iterator mit;
	map<string, MatrixArray>::iterator mait;

	// vertex shader's uniforms
	for (iit = currentVertexShader->constantTable.ints.begin(); iit != currentVertexShader->constantTable.ints.end(); iit++)
	{
		if (!shaderProgram->setInt(iit->first.c_str(), iit->second))
		{
			Logger << "\tWARNING: There is no uniform with given name or it's not used in the shader" << endl;
			Logger << "\t\tVertex shader: " << currentVertexShader->name << endl;
			Logger << "\t\tUniform name: " << iit->first.c_str() << endl;
		}
	}
	for (fit = currentVertexShader->constantTable.floats.begin(); fit != currentVertexShader->constantTable.floats.end(); fit++)
	{
		if (!shaderProgram->setFloat(fit->first.c_str(), fit->second))
		{
			Logger << "\tWARNING: There is no uniform with given name or it's not used in the shader" << endl;
			Logger << "\t\tVertex shader: " << currentVertexShader->name << endl;
			Logger << "\t\tUniform name: " << fit->first.c_str() << endl;
		}
	}
	for (vit = currentVertexShader->constantTable.vectors.begin(); vit != currentVertexShader->constantTable.vectors.end(); vit++)
	{
		if (!shaderProgram->setVector(vit->first.c_str(), vit->second))
		{
			Logger << "\tWARNING: There is no uniform with given name or it's not used in the shader" << endl;
			Logger << "\t\tVertex shader: " << currentVertexShader->name << endl;
			Logger << "\t\tUniform name: " << vit->first.c_str() << endl;
		}
	}
	for (mit = currentVertexShader->constantTable.matrices.begin(); mit != currentVertexShader->constantTable.matrices.end(); mit++)
	{
		if (!shaderProgram->setMatrix(mit->first.c_str(), mit->second))
		{
			Logger << "\tWARNING: There is no uniform with given name or it's not used in the shader" << endl;
			Logger << "\t\tVertex shader: " << currentVertexShader->name << endl;
			Logger << "\t\tUniform name: " << mit->first.c_str() << endl;
		}
	}
	for (mait = currentVertexShader->constantTable.matricesArrays.begin(); mait != currentVertexShader->constantTable.matricesArrays.end(); mait++)
	{
		if (!shaderProgram->setMatrixArray(mait->first.c_str(), mait->second))
		{
			Logger << "\tWARNING: There is no uniform with given name or it's not used in the shader" << endl;
			Logger << "\t\tVertex shader: " << currentVertexShader->name << endl;
			Logger << "\t\tUniform name: " << mait->first.c_str() << endl;
		}
	}

	// pixel shader's uniforms
	for (iit = currentPixelShader->constantTable.ints.begin(); iit != currentPixelShader->constantTable.ints.end(); iit++)
	{
		if (!shaderProgram->setInt(iit->first.c_str(), iit->second))
		{
			Logger << "\tWARNING: There is no uniform with given name or it's not used in the shader" << endl;
			Logger << "\t\tPixel shader: " << currentPixelShader->name << endl;
			Logger << "\t\tUniform name: " << iit->first.c_str() << endl;
		}
	}
	for (fit = currentPixelShader->constantTable.floats.begin(); fit != currentPixelShader->constantTable.floats.end(); fit++)
	{
		if (!shaderProgram->setFloat(fit->first.c_str(), fit->second))
		{
			Logger << "\tWARNING: There is no uniform with given name or it's not used in the shader" << endl;
			Logger << "\t\tPixel shader: " << currentPixelShader->name << endl;
			Logger << "\t\tUniform name: " << fit->first.c_str() << endl;
		}
	}
	for (vit = currentPixelShader->constantTable.vectors.begin(); vit != currentPixelShader->constantTable.vectors.end(); vit++)
	{
		if (!shaderProgram->setVector(vit->first.c_str(), vit->second))
		{
			Logger << "\tWARNING: There is no uniform with given name or it's not used in the shader" << endl;
			Logger << "\t\tPixel shader: " << currentPixelShader->name << endl;
			Logger << "\t\tUniform name: " << vit->first.c_str() << endl;
		}
	}
	for (mit = currentPixelShader->constantTable.matrices.begin(); mit != currentPixelShader->constantTable.matrices.end(); mit++)
	{
		if (!shaderProgram->setMatrix(mit->first.c_str(), mit->second))
		{
			Logger << "\tWARNING: There is no uniform with given name or it's not used in the shader" << endl;
			Logger << "\t\tPixel shader: " << currentPixelShader->name << endl;
			Logger << "\t\tUniform name: " << mit->first.c_str() << endl;
		}
	}
	for (mait = currentPixelShader->constantTable.matricesArrays.begin(); mait != currentPixelShader->constantTable.matricesArrays.end(); mait++)
	{
		if (!shaderProgram->setMatrixArray(mait->first.c_str(), mait->second))
		{
			Logger << "\tWARNING: There is no uniform with given name or it's not used in the shader" << endl;
			Logger << "\t\tPixel shader: " << currentPixelShader->name << endl;
			Logger << "\t\tUniform name: " << mait->first.c_str() << endl;
		}
	}

	// textures' uniforms
	for (int i = 0; i < MAX_SAMPLERS_NUM; i++)
	{
		if (currentTextures[i] != NULL)
		{
			if (!shaderProgram->setInt(currentTextures[i]->samplerName.c_str(), i))
			{
				Logger << "\tWARNING: There is no sampler with given name or it's not used in the shader" << endl;
				Logger << "\t\tPixel shader: " << currentPixelShader->name << endl;
				Logger << "\t\tSampler name: " << currentTextures[i]->samplerName << endl;
			}
		}
	}
}



void CRenderer::postDraw()
{
	currentVertexShader->constantTable.ints.clear();
	currentVertexShader->constantTable.floats.clear();
	currentVertexShader->constantTable.vectors.clear();
	currentVertexShader->constantTable.matrices.clear();
	currentVertexShader->constantTable.matricesArrays.clear();

	currentPixelShader->constantTable.ints.clear();
	currentPixelShader->constantTable.floats.clear();
	currentPixelShader->constantTable.vectors.clear();
	currentPixelShader->constantTable.matrices.clear();
	currentPixelShader->constantTable.matricesArrays.clear();
}



#endif
