#include <vainmoinen/vainmoinen.hpp>



string commonDataDir = "../../../data/";

CCamera camera;

CTrianglesBuffer cubeTrianglesBuffer, planeTrianglesBuffer;
CTexture texture1, texture2;



void initCamera()
{
	camera.verticalAngle = -PI/4.0f;
	camera.updateFixed(vec3(0.0f, 3.0f, 3.0f), vec3());
}



void updateCamera(int deltaTime)
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

	Triangle cubeTriangles[] =
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

	cubeTrianglesBuffer.create(12);
	Triangle* cubeTrianglesBufferData = cubeTrianglesBuffer.lockData();
	{
		memcpy((void*)cubeTrianglesBufferData, (void*)cubeTriangles, 12*sizeof(Triangle));
	}
	cubeTrianglesBuffer.unlockData();



	Vertex planeVertices[4];

	planeVertices[0].position = vec4(-0.5f, 0.0f, -0.5f);
	planeVertices[1].position = vec4(-0.5f, 0.0f, 0.5f);
	planeVertices[2].position = vec4(0.5f, 0.0f, 0.5f);
	planeVertices[3].position = vec4(0.5f, 0.0f, -0.5f);
/*
	planeVertices[0].position = vec4(-1.0f, 1.0f, 0.0f);
	planeVertices[1].position = vec4(-1.0f, -1.0f, 0.0f);
	planeVertices[2].position = vec4(1.0f, -1.0f, 0.0f);
	planeVertices[3].position = vec4(1.0f, 1.0f, 0.0f);
*/
	planeVertices[0].color = vec3(1.0f, 0.0f, 0.0f);
	planeVertices[1].color = vec3(0.0f, 1.0f, 0.0f);
	planeVertices[2].color = vec3(0.0f, 0.0f, 1.0f);
	planeVertices[3].color = vec3(1.0f, 1.0f, 1.0f);

	planeVertices[0].texCoord = vec2(0.0f, 0.0f);
	planeVertices[1].texCoord = vec2(0.0f, 4.0f);
	planeVertices[2].texCoord = vec2(4.0f, 4.0f);
	planeVertices[3].texCoord = vec2(4.0f, 0.0f);

	Triangle planeTriangles[] =
	{
		planeVertices[0], planeVertices[1], planeVertices[2],
		planeVertices[0], planeVertices[2], planeVertices[3],
	};

	planeTrianglesBuffer.create(2);
	Triangle* planeTrianglesBufferData = planeTrianglesBuffer.lockData();
	{
		memcpy((void*)planeTrianglesBufferData, (void*)planeTriangles, 2*sizeof(Triangle));
	}
	planeTrianglesBuffer.unlockData();



	texture1.createFromFile(commonDataDir + "pwr.png");
	texture2.createFromFile(commonDataDir + "rocks.png");
}



void free()
{

}



void run()
{
	updateCamera(Application.getLastFrameTime());



	const float& aspect = Application.getScreenAspectRatio();

	float frustumLeft = -0.25f * aspect;
	float frustumRight = 0.25f * aspect;
	float frustumBottom = -0.25f;
	float frustumTop = 0.25f;
	float frustumNear = 0.5f;
	float frustumFar = 500.0f;

	mtx viewProjTransform =
		constructViewTransform(camera.getEye(), camera.getRightVector(), camera.getUpVector(), -camera.getForwardVector()) *
		constructProjTransform(frustumLeft, frustumRight, frustumBottom, frustumTop, frustumNear, frustumFar);



	VRenderer.begin();

	VRenderer.setTrianglesBuffer(cubeTrianglesBuffer);
	VRenderer.setTexture(texture1);
	for (int i = 0; i < 15; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			mtx worldTransform = mtx::translate(3.0f*(i - 7), 0.5f, 3.0f*(j - 7));
			VRenderer.setTransform(worldTransform * viewProjTransform);
			VRenderer.draw();
		}
	}

	VRenderer.setTrianglesBuffer(planeTrianglesBuffer);
	VRenderer.setTransform(mtx::scale(15.0f, 1.0f, 15.0f) * viewProjTransform);
	VRenderer.setTexture(texture2);
	VRenderer.draw();

	VRenderer.end();



	Application.setWindowText("Time (ms): " + toString(Application.getLastFrameTime()));



	if (Application.isKeyPressed(SDLK_ESCAPE))
	{
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

	bool createOpenGLContext = false;
	#ifdef VRENDERER_CUDA
		createOpenGLContext = true;
	#endif

	Logger.open("log.txt");
	CImage_createSystem();
	Application.create(0, 0, 0, false, false, createOpenGLContext);
	Application.setMouseMotionFunction(mouseMotionFunction);

	#ifdef VRENDERER_DIRECT_COMPUTE
		VRenderer.create("../../../../direct_compute/");
	#else
		VRenderer.create();
	#endif

	init();
	Application.run(run);
	free();

	VRenderer.destroy();

	Application.destroy();
	CImage_destroySystem();
	Logger.close();

	return 0;
}
