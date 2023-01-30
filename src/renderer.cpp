#include <vainmoinen/renderer.hpp>



CVRenderer VRenderer;



// ----------------------------------------------------------------------------



__forceinline float implicitLine(float x, float y, const vec4& v1, const vec4& v2)
{
	return (v1.y - v2.y)*x + (v2.x - v1.x)*y + v1.x*v2.y - v2.x*v1.y;
}



__forceinline int getMin(float a, float b, float c)
{
	return (int)(MIN(a, MIN(b, c)));
}



__forceinline int getMax(float a, float b, float c)
{
	return (int)(MAX(a, MAX(b, c)));
}



__forceinline bool areVerticesCCWInScreenSpace(const Vertex& v0, const Vertex& v1, const Vertex& v2)
{
	float dx01 = v1.position.x - v0.position.x;
	float dy01 = v1.position.y - v0.position.y;
	float dx02 = v2.position.x - v0.position.x;
	float dy02 = v2.position.y - v0.position.y;

	return (dx01*dy02 - dy01*dx02 > 0.0f);
}



__forceinline int getPixelIndex_wrap(short x, short y, ushort width, ushort height)
{
	if (x == width)
		x = 0;
	if (y == height)
		y = 0;

	return y*width + x;
}



__forceinline vec2 texCoord_wrap(const vec2& texCoord)
{
	return vec2(_frac(texCoord.x), _frac(texCoord.y));
}



// ----------------------------------------------------------------------------



#ifdef VRENDERER_DIRECT_COMPUTE
	void CVRenderer::create(const string& shadersDir)
#else
	void CVRenderer::create()
#endif
{
	setViewport(Application.getScreenWidth(), Application.getScreenHeight());

	screenWidth_tiles = (screenWidth + 15) / 16;
	screenHeight_tiles = (screenHeight + 15) / 16;

	#ifdef VRENDERER_SOFTWARE
		screenSurface = Application.getSDLSurface();
		colorBuffer = (byte*)screenSurface->pixels;
		depthBuffer = new float[screenWidth * screenHeight];
	#elif VRENDERER_DIRECT_COMPUTE
		Renderer.create(screenWidth, screenHeight, Application.isFullScreen(), Application.isVSync());
		D3D11DeviceContext = Renderer.getD3D11DeviceContext();
		D3D11Device = Renderer.getD3D11Device();;

		// textures
		{
			D3D11_TEXTURE2D_DESC textureDesc;

			ZeroMemory(&textureDesc, sizeof(textureDesc));
			textureDesc.Width = screenWidth;
			textureDesc.Height = screenHeight;
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = 1;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.MiscFlags = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
			textureDesc.CPUAccessFlags = 0;
			textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			D3D11Device->CreateTexture2D(&textureDesc, NULL, &colorBufferTexture);

			ZeroMemory(&textureDesc, sizeof(textureDesc));
			textureDesc.Width = screenWidth;
			textureDesc.Height = screenHeight;
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = 1;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.MiscFlags = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
			textureDesc.CPUAccessFlags = 0;
			textureDesc.Format = DXGI_FORMAT_R32_FLOAT;
			D3D11Device->CreateTexture2D(&textureDesc, NULL, &depthBufferTexture);
		}

		// textures views
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
			D3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDesc;

			// shader resource
		
			ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
			shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
			shaderResourceViewDesc.Texture2D.MipLevels = 1;
			shaderResourceViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			D3D11Device->CreateShaderResourceView(colorBufferTexture, &shaderResourceViewDesc, &colorBufferSRV);

			// unordered access

			ZeroMemory(&unorderedAccessViewDesc, sizeof(unorderedAccessViewDesc));
			unorderedAccessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
			unorderedAccessViewDesc.Buffer.FirstElement = 0;
			unorderedAccessViewDesc.Buffer.NumElements = 1;
			unorderedAccessViewDesc.Buffer.Flags = 0;
			unorderedAccessViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			D3D11Device->CreateUnorderedAccessView(colorBufferTexture, &unorderedAccessViewDesc, &colorBufferUAV);

			ZeroMemory(&unorderedAccessViewDesc, sizeof(unorderedAccessViewDesc));
			unorderedAccessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
			unorderedAccessViewDesc.Buffer.FirstElement = 0;
			unorderedAccessViewDesc.Buffer.NumElements = 1;
			unorderedAccessViewDesc.Buffer.Flags = 0;
			unorderedAccessViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
			D3D11Device->CreateUnorderedAccessView(depthBufferTexture, &unorderedAccessViewDesc, &depthBufferUAV);
		}

		// buffers
		{
			D3D11_BUFFER_DESC bufferDesc;

			ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
			bufferDesc.ByteWidth = TRIANGLES_PER_PACK * sizeof(TriangleToRasterize);
			bufferDesc.Usage = D3D11_USAGE_STAGING;
			bufferDesc.BindFlags = 0;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = sizeof(TriangleToRasterize);
			D3D11Device->CreateBuffer(&bufferDesc, NULL, &trianglesToRasterizeBuffer_CPU);

			ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
			bufferDesc.ByteWidth = TRIANGLES_PER_PACK * sizeof(TriangleToRasterize);
			bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			bufferDesc.StructureByteStride = sizeof(TriangleToRasterize);
			D3D11Device->CreateBuffer(&bufferDesc, NULL, &trianglesToRasterizeBuffer);

			ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
			bufferDesc.ByteWidth = screenWidth_tiles * screenHeight_tiles * TRIANGLES_PER_PACK * sizeof(unsigned int);
			bufferDesc.Usage = D3D11_USAGE_STAGING;
			bufferDesc.BindFlags = 0;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bufferDesc.MiscFlags = 0;
			D3D11Device->CreateBuffer(&bufferDesc, NULL, &indicesToTrianglesToRasterizeBuffer_CPU);

			ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
			bufferDesc.ByteWidth = screenWidth_tiles * screenHeight_tiles * TRIANGLES_PER_PACK * sizeof(unsigned int);
			bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
			D3D11Device->CreateBuffer(&bufferDesc, NULL, &indicesToTrianglesToRasterizeBuffer);
		}

		// buffers views
		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDesc;

			ZeroMemory(&unorderedAccessViewDesc, sizeof(unorderedAccessViewDesc));
			unorderedAccessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			unorderedAccessViewDesc.Buffer.FirstElement = 0;
			unorderedAccessViewDesc.Buffer.NumElements = TRIANGLES_PER_PACK;
			unorderedAccessViewDesc.Buffer.Flags = 0;
			unorderedAccessViewDesc.Format = DXGI_FORMAT_UNKNOWN;
			D3D11Device->CreateUnorderedAccessView(trianglesToRasterizeBuffer, &unorderedAccessViewDesc, &trianglesToRasterizeBufferUAV);

			ZeroMemory(&unorderedAccessViewDesc, sizeof(unorderedAccessViewDesc));
			unorderedAccessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			unorderedAccessViewDesc.Buffer.FirstElement = 0;
			unorderedAccessViewDesc.Buffer.NumElements = screenWidth_tiles * screenHeight_tiles * TRIANGLES_PER_PACK;
			unorderedAccessViewDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
			unorderedAccessViewDesc.Format = DXGI_FORMAT_R32_TYPELESS;
			D3D11Device->CreateUnorderedAccessView(indicesToTrianglesToRasterizeBuffer, &unorderedAccessViewDesc, &indicesToTrianglesToRasterizeBufferUAV);
		}

		// shaders
		{
			ID3DBlob* shaderBuffer = NULL;

			CompileShaderFromFile(string(shadersDir + "screen_quad.hlsl_vs").c_str(), "main", "vs_5_0", &shaderBuffer);
			D3D11Device->CreateVertexShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &screenQuadVertexShader);
			SAFE_RELEASE(shaderBuffer);

			CompileShaderFromFile(string(shadersDir + "screen.hlsl_ps").c_str(), "main", "ps_5_0", &shaderBuffer);
			D3D11Device->CreatePixelShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &screenPixelShader);
			SAFE_RELEASE(shaderBuffer);

			CompileShaderFromFile(string(shadersDir + "clear_buffers.hlsl_cs").c_str(), "main", "cs_5_0", &shaderBuffer);
			D3D11Device->CreateComputeShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &clearBuffersComputeShader);
			SAFE_RELEASE(shaderBuffer);

			D3D10_SHADER_MACRO macros[2];
			macros[0].Name = "TRIANGLES_PER_PACK";
			string temp = toString(TRIANGLES_PER_PACK);
			macros[0].Definition = temp.c_str();
			macros[1].Name = macros[1].Definition = NULL;
			CompileShaderFromFile(string(shadersDir + "rasterize.hlsl_cs").c_str(), "main", "cs_5_0", &shaderBuffer, macros);
			D3D11Device->CreateComputeShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &rasterizeComputeShader);
			SAFE_RELEASE(shaderBuffer);
		}

		// constant buffers
		{
			D3D11_BUFFER_DESC bufferDesc;

			ZeroMemory(&bufferDesc, sizeof(bufferDesc));
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bufferDesc.MiscFlags = 0;
			bufferDesc.ByteWidth = 4 * sizeof(int);
			D3D11Device->CreateBuffer(&bufferDesc, NULL, &rasterizeComputeShader_constantBuffer);
		}

		// sampler states
		{
			D3D11_SAMPLER_DESC samplerDesc;
			ZeroMemory(&samplerDesc, sizeof(samplerDesc));

			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			samplerDesc.MipLODBias = 0.0f;
			samplerDesc.MaxAnisotropy = 1;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
			samplerDesc.BorderColor[0] = samplerDesc.BorderColor[1] = samplerDesc.BorderColor[2] = samplerDesc.BorderColor[3] = 0.0f;
			samplerDesc.MinLOD = 0;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
			samplerDesc.AddressU = samplerDesc.AddressV = samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

			D3D11Device->CreateSamplerState(&samplerDesc, &pointSampler);
		}
	#elif VRENDERER_CUDA
		create_CUDA();
	#endif
}



void CVRenderer::destroy()
{
	#ifdef VRENDERER_SOFTWARE
		delete[] depthBuffer;
	#elif VRENDERER_DIRECT_COMPUTE
		SAFE_RELEASE(pointSampler);

		SAFE_RELEASE(rasterizeComputeShader_constantBuffer);

		SAFE_RELEASE(screenQuadVertexShader);
		SAFE_RELEASE(screenPixelShader);
		SAFE_RELEASE(clearBuffersComputeShader);
		SAFE_RELEASE(rasterizeComputeShader);

		SAFE_RELEASE(trianglesToRasterizeBuffer_CPU);
		SAFE_RELEASE(trianglesToRasterizeBuffer);
		SAFE_RELEASE(indicesToTrianglesToRasterizeBuffer_CPU);
		SAFE_RELEASE(indicesToTrianglesToRasterizeBuffer);
		SAFE_RELEASE(trianglesToRasterizeBufferUAV);
		SAFE_RELEASE(indicesToTrianglesToRasterizeBufferUAV);

		SAFE_RELEASE(colorBufferTexture);
		SAFE_RELEASE(colorBufferSRV);
		SAFE_RELEASE(colorBufferUAV);
		SAFE_RELEASE(depthBufferTexture);
		SAFE_RELEASE(depthBufferUAV);

		Renderer.destroy();
	#elif VRENDERER_CUDA
		destroy_CUDA();
	#endif
}



void CVRenderer::setViewport(ushort screenWidth, ushort screenHeight)
{
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	windowTransform = mtx((float)screenWidth/2.0f,		 0.0f,						0.0f,	0.0f,
						  0.0f,							 (float)screenHeight/2.0f,	0.0f,	0.0f,
						  0.0f,							 0.0f,						1.0f,	0.0f,
						  (float)screenWidth/2.0f,		 (float)screenHeight/2.0f,	0.0f,	1.0f);
}



void CVRenderer::setTrianglesBuffer(const CTrianglesBuffer& trianglesBuffer)
{
	currentTrianglesBuffer = &trianglesBuffer;
}



void CVRenderer::setTransform(const mtx& transform)
{
	currentTransform = transform;
}



void CVRenderer::setTexture(const CTexture& texture)
{
	currentTexture = &texture;
}



void CVRenderer::draw(ushort trianglesNum)
{
	DrawCall drawCall;

	drawCall.trianglesBuffer = currentTrianglesBuffer;
	drawCall.trianglesNum = (trianglesNum == 0 ? currentTrianglesBuffer->getTrianglesNum() : trianglesNum);
	drawCall.transform = currentTransform;
	drawCall.texture = currentTexture;

	drawCalls.push_back(drawCall);
}



void CVRenderer::begin()
{
	#ifdef VRENDERER_DIRECT_COMPUTE
		Renderer.beginScene();
		Renderer.setViewport(0, 0, screenWidth, screenHeight);

		ID3D11RenderTargetView* RTVs[1] = { Renderer.getD3D11BackBufferRTV() };
		D3D11DeviceContext->OMSetRenderTargets(1, RTVs, NULL);

		Renderer.clear(true, false, false, vec3(0.5f, 0.5f, 0.5f));
	#endif
}



void CVRenderer::end()
{
	runVertexProcessor();
	runPixelProcessor();

	drawCalls.clear();
	trianglesToRasterize.clear();

	#ifdef VRENDERER_SOFTWARE
		SDL_Flip(screenSurface);
	#elif VRENDERER_DIRECT_COMPUTE
		D3D11DeviceContext->VSSetShader(screenQuadVertexShader, NULL, 0);
		D3D11DeviceContext->PSSetShader(screenPixelShader, NULL, 0);
		D3D11DeviceContext->PSSetSamplers(0, 1, &pointSampler);
		D3D11DeviceContext->PSSetShaderResources(0, 1, &colorBufferSRV);
		D3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		D3D11DeviceContext->Draw(4, 0);

		ID3D11ShaderResourceView* nullSRVs[1] = { NULL };
		D3D11DeviceContext->PSSetShaderResources(0, 1, nullSRVs);

		Renderer.endScene();
	#elif VRENDERER_CUDA
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, colorBuffer_GL_pbo);
		glBindTexture(GL_TEXTURE_2D, colorBuffer_GL_texture);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, screenWidth, screenHeight, GL_BGRA, GL_UNSIGNED_BYTE, NULL);

		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(-1.0f, -1.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex2f(-1.0f, 1.0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex2f(1.0f, 1.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex2f(1.0f, -1.0f);
		glEnd();
	#endif
}



// ----------------------------------------------------------------------------



__forceinline void CVRenderer::processProspectiveTriangleToRasterize(const Vertex& _v0, const Vertex& _v1, const Vertex& _v2, const CTexture* _texture)
{
	TriangleToRasterize t;

	t.v0 = _v0;
	t.v1 = _v1;
	t.v2 = _v2;
	t.texture = _texture;

	t.one_over_z0 = 1.0f / t.v0.position.w;
	t.one_over_z1 = 1.0f / t.v1.position.w;
	t.one_over_z2 = 1.0f / t.v2.position.w;

	// project from 4D to 2D window position with depth value in z coordinate
	t.v0.position.divideByW();
	t.v0.position *= windowTransform;
	t.v1.position.divideByW();
	t.v1.position *= windowTransform;
	t.v2.position.divideByW();
	t.v2.position *= windowTransform;

	if (!areVerticesCCWInScreenSpace(t.v0, t.v1, t.v2))
		return;

	// find bounding box for the triangle
	t.minX = MAX(getMin(t.v0.position.x, t.v1.position.x, t.v2.position.x), 0);
	t.maxX = MIN(getMax(t.v0.position.x, t.v1.position.x, t.v2.position.x), screenWidth - 1);
	t.minY = MAX(getMin(t.v0.position.y, t.v1.position.y, t.v2.position.y), 0);
	t.maxY = MIN(getMax(t.v0.position.y, t.v1.position.y, t.v2.position.y), screenHeight - 1);

	if (t.maxX <= t.minX || t.maxY <= t.minY)
		return;

	// these will be used for barycentric weights computation
	t.one_over_v0ToLine12 = 1.0f / implicitLine(t.v0.position.x, t.v0.position.y, t.v1.position, t.v2.position);
	t.one_over_v1ToLine20 = 1.0f / implicitLine(t.v1.position.x, t.v1.position.y, t.v2.position, t.v0.position);
	t.one_over_v2ToLine01 = 1.0f / implicitLine(t.v2.position.x, t.v2.position.y, t.v0.position, t.v1.position);

	// for partial derivatives computation
	t.alphaPlane = plane(vec3(t.v0.position.x, t.v0.position.y, t.v0.texCoord.x*t.one_over_z0),
						 vec3(t.v1.position.x, t.v1.position.y, t.v1.texCoord.x*t.one_over_z1),
						 vec3(t.v2.position.x, t.v2.position.y, t.v2.texCoord.x*t.one_over_z2));
	t.betaPlane = plane(vec3(t.v0.position.x, t.v0.position.y, t.v0.texCoord.y*t.one_over_z0),
						vec3(t.v1.position.x, t.v1.position.y, t.v1.texCoord.y*t.one_over_z1),
						vec3(t.v2.position.x, t.v2.position.y, t.v2.texCoord.y*t.one_over_z2));
	t.gammaPlane = plane(vec3(t.v0.position.x, t.v0.position.y, t.one_over_z0),
						 vec3(t.v1.position.x, t.v1.position.y, t.one_over_z1),
						 vec3(t.v2.position.x, t.v2.position.y, t.one_over_z2));
	t.one_over_alpha_c = 1.0f / t.alphaPlane.c;
	t.one_over_beta_c = 1.0f / t.betaPlane.c;
	t.one_over_gamma_c = 1.0f / t.gammaPlane.c;
	t.alpha_ffx = -t.alphaPlane.a * t.one_over_alpha_c;
	t.beta_ffx = -t.betaPlane.a * t.one_over_beta_c;
	t.gamma_ffx = -t.gammaPlane.a * t.one_over_gamma_c;
	t.alpha_ffy = -t.alphaPlane.b * t.one_over_alpha_c;
	t.beta_ffy = -t.betaPlane.b * t.one_over_beta_c;
	t.gamma_ffy = -t.gammaPlane.b * t.one_over_gamma_c;

	t.tileMinX = t.minX / 16;
	t.tileMinY = t.minY / 16;
	t.tileMaxX = t.maxX / 16;
	t.tileMaxY = t.maxY / 16;

	trianglesToRasterize.push_back(t);
}



__forceinline vector<Vertex> CVRenderer::clipPolygonToPlaneIn4D(const vector<Vertex>& vertices, const vec4& planeNormal)
{
	vector<Vertex> clippedVertices;

	for (uint i = 0; i < vertices.size(); i++)
	{
		int a = i;
		int b = (i + 1) % vertices.size();

		float fa = vertices[a].position % planeNormal;
		float fb = vertices[b].position % planeNormal;

		if ( (fa < 0 && fb > 0) || (fa > 0 && fb < 0) )
		{
			vec4 direction = vertices[b].position - vertices[a].position;
			float t = - (planeNormal % vertices[a].position) / (planeNormal % direction);

			vec4 position = vertices[a].position + t*direction;
			vec3 color = vertices[a].color + t*(vertices[b].color - vertices[a].color);
			vec2 texCoord = vertices[a].texCoord + t*(vertices[b].texCoord - vertices[a].texCoord);

			if (fa < 0)
			{
				clippedVertices.push_back(vertices[a]);
				clippedVertices.push_back(Vertex(position, color, texCoord));
			}
			else if (fb < 0)
			{
				clippedVertices.push_back(Vertex(position, color, texCoord));
			}
		}
		else if (fa < 0 && fb < 0)
		{
			clippedVertices.push_back(vertices[a]);
		}
	}

	return clippedVertices;
}



__forceinline Vertex CVRenderer::runVertexShader(const mtx& transform, const Vertex& input)
{
	Vertex output;

	output.position = input.position * transform;
	output.color = input.color;
	output.texCoord = input.texCoord;

	return output;
}



void CVRenderer::runVertexProcessor()
{
	for (uint i = 0; i < drawCalls.size(); i++)
	{
		for (uint j = 0; j < drawCalls[i].trianglesNum; j++)
		{
			Triangle triangle;

			for (byte k = 0; k < 3; k++)
				triangle.vertices[k] = runVertexShader(drawCalls[i].transform, drawCalls[i].trianglesBuffer->triangles[j].vertices[k]);

			// classify vertices visibility with respect to the planes of the view frustum

			byte visibilityBits[3];

			for (byte k = 0; k < 3; k++)
			{
				visibilityBits[k] = 0;

				if (triangle.vertices[k].position.x < -triangle.vertices[k].position.w)
					visibilityBits[k] |= 1;
				if (triangle.vertices[k].position.x > triangle.vertices[k].position.w)
					visibilityBits[k] |= 2;
				if (triangle.vertices[k].position.y < -triangle.vertices[k].position.w)
					visibilityBits[k] |= 4;
				if (triangle.vertices[k].position.y > triangle.vertices[k].position.w)
					visibilityBits[k] |= 8;
				if (triangle.vertices[k].position.z < -triangle.vertices[k].position.w)
					visibilityBits[k] |= 16;
				if (triangle.vertices[k].position.z > triangle.vertices[k].position.w)
					visibilityBits[k] |= 32;
			}

			// all vertices are not visible - reject the triangle
			if ( (visibilityBits[0] & visibilityBits[1] & visibilityBits[2]) > 0 )
			{
				continue;
			}

			// all vertices are visible - pass the whole triangle to the rasterizer
			if ( (visibilityBits[0] | visibilityBits[1] | visibilityBits[2]) == 0 )
			{
				processProspectiveTriangleToRasterize(
					triangle.vertices[0],
					triangle.vertices[1],
					triangle.vertices[2],
					drawCalls[i].texture);

				continue;
			}

			// at this moment the triangle is known to be intersecting one of the view frustum's planes

			vector<Vertex> vertices;
			vertices.push_back(triangle.vertices[0]);
			vertices.push_back(triangle.vertices[1]);
			vertices.push_back(triangle.vertices[2]);

			vertices = clipPolygonToPlaneIn4D(vertices, vec4(0.0f, 0.0f, -1.0f, -1.0f));
		//	vertices = clipPolygonToPlaneIn4D(vertices, vec4(0.0f, 0.0f, 1.0f, -1.0f));
		//	vertices = clipPolygonToPlaneIn4D(vertices, vec4(-1.0f, 0.0f, 0.0f, -1.0f));
		//	vertices = clipPolygonToPlaneIn4D(vertices, vec4(1.0f, 0.0f, 0.0f, -1.0f));
		//	vertices = clipPolygonToPlaneIn4D(vertices, vec4(0.0f, -1.0f, 0.0f, -1.0f));
		//	vertices = clipPolygonToPlaneIn4D(vertices, vec4(0.0f, 1.0f, 0.0f, -1.0f));

			// triangulation of the polygon formed of vertices array
			if (vertices.size() >= 3)
			{
				for (byte k = 0; k < vertices.size() - 2; k++)
				{
					processProspectiveTriangleToRasterize(
						vertices[0],
						vertices[1 + k],
						vertices[2 + k],
						drawCalls[i].texture);
				}
			}
		}
	}
}



__forceinline vec3 CVRenderer::tex2D_point(const CTexture* texture, const vec2& imageTexCoord, byte mipmapIndex)
{
	int x = (int)imageTexCoord.x;
	int y = (int)imageTexCoord.y;
	vec3 color;

	int pixelIndex = y*texture->mipmaps[mipmapIndex].width + x;
	color.x = texture->mipmaps[mipmapIndex].data[4*pixelIndex + 0];
	color.y = texture->mipmaps[mipmapIndex].data[4*pixelIndex + 1];
	color.z = texture->mipmaps[mipmapIndex].data[4*pixelIndex + 2];

	return color;
}



__forceinline vec3 CVRenderer::tex2D_linear(const CTexture* texture, const vec2& imageTexCoord, byte mipmapIndex)
{
	int x = (int)imageTexCoord.x;
	int y = (int)imageTexCoord.y;
	float alpha = imageTexCoord.x - x;
	float beta = imageTexCoord.y - y;
	float oneMinusAlpha = 1.0f - alpha;
	float oneMinusBeta = 1.0f - beta;
	float a = oneMinusAlpha * oneMinusBeta;
	float b = alpha * oneMinusBeta;
	float c = oneMinusAlpha * beta;
	float d = alpha * beta;
	vec3 color;

	int pixelIndex;

	pixelIndex = getPixelIndex_wrap(x, y, texture->mipmaps[mipmapIndex].width, texture->mipmaps[mipmapIndex].height);
	color.x = a * texture->mipmaps[mipmapIndex].data[4*pixelIndex + 0];
	color.y = a * texture->mipmaps[mipmapIndex].data[4*pixelIndex + 1];
	color.z = a * texture->mipmaps[mipmapIndex].data[4*pixelIndex + 2];

	pixelIndex = getPixelIndex_wrap(x + 1, y, texture->mipmaps[mipmapIndex].width, texture->mipmaps[mipmapIndex].height);
	color.x += b * texture->mipmaps[mipmapIndex].data[4*pixelIndex + 0];
	color.y += b * texture->mipmaps[mipmapIndex].data[4*pixelIndex + 1];
	color.z += b * texture->mipmaps[mipmapIndex].data[4*pixelIndex + 2];

	pixelIndex = getPixelIndex_wrap(x, y + 1, texture->mipmaps[mipmapIndex].width, texture->mipmaps[mipmapIndex].height);
	color.x += c * texture->mipmaps[mipmapIndex].data[4*pixelIndex + 0];
	color.y += c * texture->mipmaps[mipmapIndex].data[4*pixelIndex + 1];
	color.z += c * texture->mipmaps[mipmapIndex].data[4*pixelIndex + 2];

	pixelIndex = getPixelIndex_wrap(x + 1, y + 1, texture->mipmaps[mipmapIndex].width, texture->mipmaps[mipmapIndex].height);
	color.x += d * texture->mipmaps[mipmapIndex].data[4*pixelIndex + 0];
	color.y += d * texture->mipmaps[mipmapIndex].data[4*pixelIndex + 1];
	color.z += d * texture->mipmaps[mipmapIndex].data[4*pixelIndex + 2];

	return color;
}



__forceinline vec3 CVRenderer::tex2D_linear_mipmap_point(const CTexture* texture, const vec2& texCoord)
{
	float px = sqrtf(sqr(dudx) + sqr(dvdx));
	float py = sqrtf(sqr(dudy) + sqr(dvdy));
	float lambda = _log2(MAX(px, py));
	byte mipmapIndex = _clamp(_round(lambda), 0, (int)MAX(texture->widthLog, texture->heightLog));

	vec2 imageTexCoord = texCoord_wrap(texCoord);
	imageTexCoord.x *= texture->mipmaps[mipmapIndex].width;
	imageTexCoord.y *= texture->mipmaps[mipmapIndex].height;

	vec3 color = tex2D_linear(texture, imageTexCoord, mipmapIndex);

	return color;
}



__forceinline vec3 CVRenderer::tex2D_linear_mipmap_linear(const CTexture* texture, const vec2& texCoord)
{
	float px = sqrtf(sqr(dudx) + sqr(dvdx));
	float py = sqrtf(sqr(dudy) + sqr(dvdy));
	float lambda = _log2(MAX(px, py));
	byte mipmapIndex1 = _clamp((int)lambda, 0, MAX(texture->widthLog, texture->heightLog) - 1);
	byte mipmapIndex2 = mipmapIndex1 + 1;

	vec2 imageTexCoord = texCoord_wrap(texCoord);
	vec2 imageTexCoord1 = imageTexCoord;
	imageTexCoord1.x *= texture->mipmaps[mipmapIndex1].width;
	imageTexCoord1.y *= texture->mipmaps[mipmapIndex1].height;
	vec2 imageTexCoord2 = imageTexCoord;
	imageTexCoord2.x *= texture->mipmaps[mipmapIndex2].width;
	imageTexCoord2.y *= texture->mipmaps[mipmapIndex2].height;

	vec3 color, color1, color2;
	color1 = tex2D_linear(texture, imageTexCoord1, mipmapIndex1);
	color2 = tex2D_linear(texture, imageTexCoord2, mipmapIndex2);
	lambda = _frac(MAX(lambda, 0.0f));
	color = (1.0f - lambda)*color1 + lambda*color2;

	return color;
}



__forceinline vec3 CVRenderer::tex2D(const CTexture* texture, const vec2& texCoord)
{
	return (1.0f / 255.0f) * tex2D_linear_mipmap_linear(texture, texCoord);
}



__forceinline vec3 CVRenderer::runPixelShader(const CTexture* texture, const vec3& color, const vec2& texCoord)
{
	return color * tex2D(texture, texCoord);
}



void CVRenderer::runPixelProcessor()
{
	#ifdef VRENDERER_SOFTWARE
		// clear buffers
		memset(colorBuffer, 0, 4*sizeof(byte)*screenWidth*screenHeight);
		memset((void*)depthBuffer, 1000000, sizeof(float)*screenWidth*screenHeight);

		for (uint i = 0; i < trianglesToRasterize.size(); i++)
		{
			TriangleToRasterize& t = trianglesToRasterize[i];

			for (int yi = t.minY; yi <= t.maxY; yi++)
			{
				for (int xi = t.minX; xi <= t.maxX; xi++)
				{
					// we want centers of pixels to be used in computations
					float x = (float)xi + 0.5f;
					float y = (float)yi + 0.5f;

					// affine barycentric weights
					float alpha = implicitLine(x, y, t.v1.position, t.v2.position) * t.one_over_v0ToLine12;
					float beta = implicitLine(x, y, t.v2.position, t.v0.position) * t.one_over_v1ToLine20;
					float gamma = implicitLine(x, y, t.v0.position, t.v1.position) * t.one_over_v2ToLine01;

					// if pixel (x, y) is inside the triangle or on one of its edges
					if (alpha >= 0 && beta >= 0 && gamma >= 0)
					{
						int pixelIndex = (screenHeight - 1 - yi)*screenWidth + xi;

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
							vec3 color_persp = alpha*t.v0.color + beta*t.v1.color + gamma*t.v2.color;
							vec2 texCoord_persp = alpha*t.v0.texCoord + beta*t.v1.texCoord + gamma*t.v2.texCoord;

							// partial derivatives (for mip-mapping)

							float u_over_z = -(t.alphaPlane.a*x + t.alphaPlane.b*y + t.alphaPlane.d) * t.one_over_alpha_c;
							float v_over_z = -(t.betaPlane.a*x + t.betaPlane.b*y + t.betaPlane.d) * t.one_over_beta_c;
							float one_over_z = -(t.gammaPlane.a*x + t.gammaPlane.b*y + t.gammaPlane.d) * t.one_over_gamma_c;
							float one_over_squared_one_over_z = 1.0f / sqr(one_over_z);

							dudx = one_over_squared_one_over_z * (t.alpha_ffx * one_over_z - u_over_z * t.gamma_ffx);
							dudy = one_over_squared_one_over_z * (t.beta_ffx * one_over_z - v_over_z * t.gamma_ffx);
							dvdx = one_over_squared_one_over_z * (t.alpha_ffy * one_over_z - u_over_z * t.gamma_ffy);
							dvdy = one_over_squared_one_over_z * (t.beta_ffy * one_over_z - v_over_z * t.gamma_ffy);

							dudx *= t.texture->mipmaps[0].width;
							dudy *= t.texture->mipmaps[0].width;
							dvdx *= t.texture->mipmaps[0].height;
							dvdy *= t.texture->mipmaps[0].height;

							// run pixel shader
							vec3 pixelColor = runPixelShader(t.texture, color_persp, texCoord_persp);

							// clamp bytes to 255
							byte red = (byte)(255.0f * MIN(pixelColor.x, 1.0f));
							byte green = (byte)(255.0f * MIN(pixelColor.y, 1.0f));
							byte blue = (byte)(255.0f * MIN(pixelColor.z, 1.0f));

							// update buffers
							colorBuffer[4*pixelIndex + 0] = blue;
							colorBuffer[4*pixelIndex + 1] = green;
							colorBuffer[4*pixelIndex + 2] = red;
							depthBuffer[pixelIndex] = z_affine;
						}
					}
				}
			}
		}
	#elif VRENDERER_DIRECT_COMPUTE
		D3D11_MAPPED_SUBRESOURCE mappedSubresource;

		// clear buffers
		ID3D11UnorderedAccessView* UAVs[2] = { colorBufferUAV, depthBufferUAV };
		D3D11DeviceContext->CSSetUnorderedAccessViews(0, 2, UAVs, NULL);
		D3D11DeviceContext->CSSetShader(clearBuffersComputeShader, NULL, 0);
		D3D11DeviceContext->Dispatch(screenWidth_tiles, screenHeight_tiles, 1);

		if (trianglesToRasterize.size() == 0)
		{
			ID3D11UnorderedAccessView* nullUAVs[2] = { NULL, NULL };
			D3D11DeviceContext->CSSetUnorderedAccessViews(0, 2, nullUAVs, NULL);

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

			// copy trianglesToRasterizeBuffer to GPU
			D3D11DeviceContext->Map(trianglesToRasterizeBuffer_CPU, 0, D3D11_MAP_WRITE, 0, &mappedSubresource);
			{
				memcpy(mappedSubresource.pData, &trianglesToRasterize[TRIANGLES_PER_PACK*k], trianglesToRasterizeNumInCurrentPack * sizeof(TriangleToRasterize));
			}
			D3D11DeviceContext->Unmap(trianglesToRasterizeBuffer_CPU, 0);
			D3D11DeviceContext->CopyResource(trianglesToRasterizeBuffer, trianglesToRasterizeBuffer_CPU);

			// copy indicesToTrianglesToRasterizeBuffer to GPU
			D3D11DeviceContext->Map(indicesToTrianglesToRasterizeBuffer_CPU, 0, D3D11_MAP_WRITE, 0, &mappedSubresource);
			{
				unsigned int* data = (unsigned int*)mappedSubresource.pData;
				memset(data, 0, screenWidth_tiles * screenHeight_tiles * TRIANGLES_PER_PACK * sizeof(unsigned int));

				for (int i = 0; i < screenWidth_tiles * screenHeight_tiles; i++)
				{
					int tileX = i % screenWidth_tiles;
					int tileY = i / screenWidth_tiles;
					int offset = 0;

					for (int j = 0; j < trianglesToRasterizeNumInCurrentPack; j++)
					{
						if (trianglesToRasterize[TRIANGLES_PER_PACK*k + j].coversTile(tileX, tileY))
						{
							data[TRIANGLES_PER_PACK*i + offset] = j + 1;
							offset++;
						}
					}
				}
			}
			D3D11DeviceContext->Unmap(indicesToTrianglesToRasterizeBuffer_CPU, 0);
			D3D11DeviceContext->CopyResource(indicesToTrianglesToRasterizeBuffer, indicesToTrianglesToRasterizeBuffer_CPU);

			// update constants
			D3D11DeviceContext->Map(rasterizeComputeShader_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
			{
				int* data = (int*)mappedSubresource.pData;
				data[0] = screenWidth_tiles;
			}
			D3D11DeviceContext->Unmap(rasterizeComputeShader_constantBuffer, 0);
			D3D11DeviceContext->CSSetConstantBuffers(0, 1, &rasterizeComputeShader_constantBuffer);

			// rasterize
			ID3D11UnorderedAccessView* UAVs[4] = { colorBufferUAV, depthBufferUAV, trianglesToRasterizeBufferUAV, indicesToTrianglesToRasterizeBufferUAV };
			D3D11DeviceContext->CSSetUnorderedAccessViews(0, 4, UAVs, NULL);
			D3D11DeviceContext->CSSetShader(rasterizeComputeShader, NULL, 0);
			D3D11DeviceContext->Dispatch(screenWidth_tiles, screenHeight_tiles, 1);
			ID3D11UnorderedAccessView* nullUAVs[4] = { NULL, NULL, NULL, NULL };
			D3D11DeviceContext->CSSetUnorderedAccessViews(0, 4, nullUAVs, NULL);
		}
	#elif VRENDERER_CUDA
		runPixelProcessor_CUDA();
	#endif
}
