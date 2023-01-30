#include <common/main.hpp>
#include <math/main.hpp>
#include <renderer/main.hpp>

#ifdef WIN32
	#if defined(DEBUG) | defined(_DEBUG)
		#include <crtdbg.h>
	#endif
#endif



string localDataDir = "../../data/";
string commonDataDir = "../../../data/";

CCamera camera;

CVertexDeclaration vd;
CVertexBuffer cubeVB, planeVB;
CVertexShader sceneVS;
CPixelShader scenePS;
CTexture texture1, texture2;



struct Vertex
{
	vec4 position;
	vec3 color;
	vec2 texCoord;
};



inline mtx constructViewTransform(const vec3& position, const vec3& rightVector, const vec3& upVector, const vec3& forwardVector)
{
	mtx translate = mtx::translate(-position);

	mtx rotate(rightVector.x, upVector.x, forwardVector.x, 0.0f,
			   rightVector.y, upVector.y, forwardVector.y, 0.0f,
			   rightVector.z, upVector.z, forwardVector.z, 0.0f,
			   0.0f,          0.0f,       0.0f,            1.0f);

	return translate * rotate;
}

inline mtx constructProjTransform(float left, float right, float bottom, float top, float zNear, float zFar)
{
	mtx perspective(zNear, 0.0f,  0.0f,          0.0f,
					0.0f,  zNear, 0.0f,          0.0f,
					0.0f,  0.0f,  zNear + zFar, -1.0f,
					0.0f,  0.0f,  zNear * zFar,  0.0f);

	mtx orthogonal( 2.0f / (right - left),            0.0f,                             0.0f,                            0.0f,
					0.0f,                             2.0f / (top - bottom),            0.0f,                            0.0f,
					0.0f,                             0.0f,                            -2.0f / (zFar - zNear),           0.0f,
				   -(right + left) / (right - left), -(top + bottom) / (top - bottom), -(zNear + zFar) / (zFar - zNear), 1.0f);
	
	return perspective * orthogonal;
}



void initCamera()
{
	camera.verticalAngle = -PI/4.0f;
	camera.updateFixed(vec3(0.0f, 3.0f, 3.0f), vec3());
}



void updateCamera(float deltaTime)
{
	vec3 eye;

	float speed = 0.02f;
	if (Application.isKeyPressed(SDLK_LSHIFT))
		speed = 0.08f;

	eye = camera.getEye();
	if (Application.isKeyPressed(SDLK_w))
		eye += speed * deltaTime * camera.getForwardVector();
	if (Application.isKeyPressed(SDLK_s))
		eye -= speed * deltaTime * camera.getForwardVector();
	if (Application.isKeyPressed(SDLK_a))
		eye -= speed * deltaTime * camera.getRightVector();
	if (Application.isKeyPressed(SDLK_d))
		eye += speed * deltaTime * camera.getRightVector();

	camera.updateFree(eye);
}



void mouseMotionFunction()
{
	camera.horizontalAngle -= Application.getMouseRelX() / 1000.0f;
	camera.verticalAngle -= Application.getMouseRelY() / 1000.0f;
}



void init()
{
	initCamera();



	vd.create();
	vd.addVertexElement(0, VertexElementType::Float4, VertexElementSemantic::Position, 0);
	vd.addVertexElement(16, VertexElementType::Float3, VertexElementSemantic::Color, 0);
	vd.addVertexElement(28, VertexElementType::Float2, VertexElementSemantic::TexCoord, 0);
	vd.build();

	cubeVB.create(36*sizeof(Vertex));
	planeVB.create(6*sizeof(Vertex));

	sceneVS.createFromFile(localDataDir + "scene.vp");

	scenePS.createFromFile(localDataDir + "scene.fp");

	texture1.createFromFile(commonDataDir + "pwr.png", 0);
	texture2.createFromFile(commonDataDir + "rocks.png", 0);
	texture1.setFiltering_ogl(SamplerFiltering::Linear, SamplerFiltering::Linear, SamplerFiltering::Linear);
	texture2.setFiltering_ogl(SamplerFiltering::Linear, SamplerFiltering::Linear, SamplerFiltering::Linear);



	Vertex cubeVertices[24];

	for (int i = 0; i < 24; i++)
		cubeVertices[i].color = vec3(1.0f, 1.0f, 1.0f);

	cubeVertices[0].position = vec4(-0.5f, 0.5f, 0.5f);
	cubeVertices[0].texCoord = vec2(0.0f, 0.0f);
	cubeVertices[1].position = vec4(-0.5f, -0.5f, 0.5f);
	cubeVertices[1].texCoord = vec2(0.0f, 1.0f);
	cubeVertices[2].position = vec4(0.5f, -0.5f, 0.5f);
	cubeVertices[2].texCoord = vec2(1.0f, 1.0f);
	cubeVertices[3].position = vec4(0.5f, 0.5f, 0.5f);
	cubeVertices[3].texCoord = vec2(1.0f, 0.0f);

	cubeVertices[4].position = vec4(0.5f, 0.5f, 0.5f);
	cubeVertices[4].texCoord = vec2(0.0f, 0.0f);
	cubeVertices[5].position = vec4(0.5f, -0.5f, 0.5f);
	cubeVertices[5].texCoord = vec2(0.0f, 1.0f);
	cubeVertices[6].position = vec4(0.5f, -0.5f, -0.5f);
	cubeVertices[6].texCoord = vec2(1.0f, 1.0f);
	cubeVertices[7].position = vec4(0.5f, 0.5f, -0.5f);
	cubeVertices[7].texCoord = vec2(1.0f, 0.0f);

	cubeVertices[8].position = vec4(0.5f, 0.5f, -0.5f);
	cubeVertices[8].texCoord = vec2(0.0f, 0.0f);
	cubeVertices[9].position = vec4(0.5f, -0.5f, -0.5f);
	cubeVertices[9].texCoord = vec2(0.0f, 1.0f);
	cubeVertices[10].position = vec4(-0.5f, -0.5f, -0.5f);
	cubeVertices[10].texCoord = vec2(1.0f, 1.0f);
	cubeVertices[11].position = vec4(-0.5f, 0.5f, -0.5f);
	cubeVertices[11].texCoord = vec2(1.0f, 0.0f);

	cubeVertices[12].position = vec4(-0.5f, 0.5f, -0.5f);
	cubeVertices[12].texCoord = vec2(0.0f, 0.0f);
	cubeVertices[13].position = vec4(-0.5f, -0.5f, -0.5f);
	cubeVertices[13].texCoord = vec2(0.0f, 1.0f);
	cubeVertices[14].position = vec4(-0.5f, -0.5f, 0.5f);
	cubeVertices[14].texCoord = vec2(1.0f, 1.0f);
	cubeVertices[15].position = vec4(-0.5f, 0.5f, 0.5f);
	cubeVertices[15].texCoord = vec2(1.0f, 0.0f);

	cubeVertices[16].position = vec4(-0.5f, 0.5f, -0.5f);
	cubeVertices[16].texCoord = vec2(0.0f, 0.0f);
	cubeVertices[17].position = vec4(-0.5f, 0.5f, 0.5f);
	cubeVertices[17].texCoord = vec2(0.0f, 1.0f);
	cubeVertices[18].position = vec4(0.5f, 0.5f, 0.5f);
	cubeVertices[18].texCoord = vec2(1.0f, 1.0f);
	cubeVertices[19].position = vec4(0.5f, 0.5f, -0.5f);
	cubeVertices[19].texCoord = vec2(1.0f, 0.0f);

	cubeVertices[20].position = vec4(-0.5f, -0.5f, 0.5f);
	cubeVertices[20].texCoord = vec2(0.0f, 0.0f);
	cubeVertices[21].position = vec4(-0.5f, -0.5f, -0.5f);
	cubeVertices[21].texCoord = vec2(0.0f, 1.0f);
	cubeVertices[22].position = vec4(0.5f, -0.5f, -0.5f);
	cubeVertices[22].texCoord = vec2(1.0f, 1.0f);
	cubeVertices[23].position = vec4(0.5f, -0.5f, 0.5f);
	cubeVertices[23].texCoord = vec2(1.0f, 0.0f);

	Vertex cubeTriangles[] =
	{
		cubeVertices[0], cubeVertices[1], cubeVertices[2],
		cubeVertices[0], cubeVertices[2], cubeVertices[3],
		cubeVertices[4], cubeVertices[5], cubeVertices[6],
		cubeVertices[4], cubeVertices[6], cubeVertices[7],
		cubeVertices[8], cubeVertices[9], cubeVertices[10],
		cubeVertices[8], cubeVertices[10], cubeVertices[11],
		cubeVertices[12], cubeVertices[13], cubeVertices[14],
		cubeVertices[12], cubeVertices[14], cubeVertices[15],
		cubeVertices[16], cubeVertices[17], cubeVertices[18],
		cubeVertices[16], cubeVertices[18], cubeVertices[19],
		cubeVertices[20], cubeVertices[21], cubeVertices[22],
		cubeVertices[20], cubeVertices[22], cubeVertices[23]
	};

	Vertex* cubeVerticesData;
	cubeVB.map((void*&)cubeVerticesData);
	{
		memcpy((void*)cubeVerticesData, (void*)cubeTriangles, 36*sizeof(Vertex));
	}
	cubeVB.unmap();



	Vertex planeVertices[4];

	planeVertices[0].position = vec4(-0.5f, 0.0f, -0.5f);
	planeVertices[1].position = vec4(-0.5f, 0.0f, 0.5f);
	planeVertices[2].position = vec4(0.5f, 0.0f, 0.5f);
	planeVertices[3].position = vec4(0.5f, 0.0f, -0.5f);

	planeVertices[0].color = vec3(1.0f, 0.0f, 0.0f);
	planeVertices[1].color = vec3(0.0f, 1.0f, 0.0f);
	planeVertices[2].color = vec3(0.0f, 0.0f, 1.0f);
	planeVertices[3].color = vec3(1.0f, 1.0f, 1.0f);

	planeVertices[0].texCoord = vec2(0.0f, 0.0f);
	planeVertices[1].texCoord = vec2(0.0f, 4.0f);
	planeVertices[2].texCoord = vec2(4.0f, 4.0f);
	planeVertices[3].texCoord = vec2(4.0f, 0.0f);

	Vertex planeTriangles[] =
	{
		planeVertices[0], planeVertices[1], planeVertices[2],
		planeVertices[0], planeVertices[2], planeVertices[3],
	};

	Vertex* planeVerticesData;
	planeVB.map((void*&)planeVerticesData);
	{
		memcpy((void*)planeVerticesData, (void*)planeTriangles, 6*sizeof(Vertex));
	}
	planeVB.unmap();
}



void run()
{
	float deltaTime = (float)Application.getLastFrameTime();



	updateCamera(deltaTime);



	const float& aspect = Application.getScreenAspectRatio();

	float left = -0.25f * aspect;
	float right = 0.25f * aspect;
	float bottom = -0.25f;
	float top = 0.25f;
	float near = 0.5f;
	float far = 500.0f;

	mtx viewProjTransform =
		constructViewTransform(camera.getEye(), camera.getRightVector(), camera.getUpVector(), -camera.getForwardVector()) *
		constructProjTransform(left, right, bottom, top, near, far);



	Renderer.beginScene();
	Renderer.setVertexDeclaration(vd);
	Renderer.clear(true, true, false, vec3(0.0f, 0.0f, 0.0f));

	Renderer.setVertexShader(sceneVS);
	{
		Renderer.setVertexShaderConstant("viewProjTransform", viewProjTransform);
	}
	Renderer.setPixelShader(scenePS);

	Renderer.setVertexBuffer(cubeVB);
	Renderer.setTexture(0, "diffuseMap", texture1);
	for (int i = 0; i < 15; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			mtx worldTransform = mtx::translate(3.0f*(i - 7), 0.5f, 3.0f*(j - 7));
			Renderer.setVertexShaderConstant("worldTransform", worldTransform);
			Renderer.drawPrimitives(PrimitiveType::TriangleList, 12, 0);
		}
	}

	Renderer.setVertexBuffer(planeVB);
	Renderer.setVertexShaderConstant("worldTransform", mtx::scale(15.0f, 1.0f, 15.0f));
	Renderer.setTexture(0, "diffuseMap", texture2);
	Renderer.drawPrimitives(PrimitiveType::TriangleList, 2, 0);

	Renderer.endScene();



	Application.setWindowText("Time (ms): " + toString(Application.getLastFrameTime()));



	if (Application.isKeyPressed(SDLK_ESCAPE))
	{
		sceneVS.destroy();
		scenePS.destroy();
		texture1.destroy();
		texture2.destroy();

		Application.breakRunFunction();
	}
}



#ifdef WIN32
	#undef main
#endif

int main()
{
	#ifdef WIN32
		#if defined(DEBUG) | defined(_DEBUG)
			_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		#endif
	#endif

	Logger.open("log_BlossomEngineApplication.txt");
	Application.create(0, 0, 0, false, false, true);
	Application.setMouseMotionFunction(mouseMotionFunction);
	Renderer.create(Application.getScreenWidth(), Application.getScreenHeight(), Application.isFullScreen(), Application.isVSync());

	init();
	Application.run(run);

	Renderer.destroy();
	Application.destroy();
	Logger.close();

	return 0;
}
