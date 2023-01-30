#include <common/application.hpp>
#include <common/logger.hpp>



CApplication Application;



void CApplication::create(int screenWidth, int screenHeight, int bitsPerPixel, bool fullScreen, bool vSync, bool createOpenGLContext)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
	{
		Logger << "\tERROR: SDL initialization failed: " << SDL_GetError() << endl;
		exit(1);
	}

	// keep window in center of the screen
	SDL_putenv((char*)"SDL_VIDEO_CENTERED=1");

	const SDL_VideoInfo *videoInfo = SDL_GetVideoInfo();

	if (screenWidth == 0 && screenHeight == 0)
	{
		this->screenWidth = videoInfo->current_w;
		this->screenHeight = videoInfo->current_h;
	}
	else
	{
		this->screenWidth = screenWidth;
		this->screenHeight = screenHeight;
	}

	if (bitsPerPixel == 0)
	{
		screenBitsPerPixel = videoInfo->vfmt->BitsPerPixel;
	}
	else
	{
		screenBitsPerPixel = bitsPerPixel;
	}

	screenAspectRatio = (float)this->screenWidth / (float)this->screenHeight;

	this->fullScreen = fullScreen;
	this->vSync = vSync;

	int flags = 0;
	if (fullScreen)
		flags |= SDL_FULLSCREEN;

	if (createOpenGLContext)
	{
		this->openGLContext = createOpenGLContext;

		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, true);
		SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, vSync ? 1 : 0);

		flags |= SDL_OPENGL;
	}

	SDLSurface = SDL_SetVideoMode(this->screenWidth, this->screenHeight, screenBitsPerPixel, flags);

	if (!SDLSurface)
	{
		Logger << "ERROR: Video mode set failed: " << SDL_GetError() << endl;
		exit(1);
	}

	breakRunFuctionState = false;

	runFunction = NULL;
    keyDownFunction = NULL;
    keyUpFunction = NULL;
	mouseMotionFunction = NULL;
	mouseLeftButtonDownFunction = NULL;
	mouseMiddleButtonDownFunction = NULL;
	mouseRightButtonDownFunction = NULL;
	mouseLeftButtonUpFunction = NULL;
	mouseMiddleButtonUpFunction = NULL;
	mouseRightButtonUpFunction = NULL;

	setWindowText("MaxestFrameworkApp");
	setCursorVisibility(false);
	setGrabInputState(true);

	Logger << "OK: Application start" << endl;
}



void CApplication::destroy()
{
	SDL_Quit();

    keyDownFunction = NULL;
    keyUpFunction = NULL;
	mouseMotionFunction = NULL;
	mouseLeftButtonDownFunction = NULL;
	mouseMiddleButtonDownFunction = NULL;
	mouseRightButtonDownFunction = NULL;
	mouseLeftButtonUpFunction = NULL;
	mouseMiddleButtonUpFunction = NULL;
	mouseRightButtonUpFunction = NULL;

	for (uint i = 0; i < timers.size(); i++)
		SDL_RemoveTimer(timers[i].id_SDL);
	timers.clear();

	Logger << "OK: Application quit" << endl;
}



void CApplication::addTimer(const int& id, int interval, void(*eventFunction)())
{
	Timer timer;

	timer.id = id;
	timer.id_SDL = SDL_AddTimer(interval, timerFunc, (void*)&id);
	timer.interval = interval;
	timer.eventFunction = eventFunction;

	timers.push_back(timer);
}



void CApplication::deleteTimer(int id)
{
	for (uint i = 0; i < timers.size(); i++)
	{
		if (timers[i].id == id)
		{
			SDL_RemoveTimer(timers[i].id_SDL);
			timers.erase(timers.begin() + i);
			break;
		}
	}
}



void CApplication::run(void (*runFunction)())
{
	for (;;)
	{
		lastFrameTime = timeAfterFrame - timeBeforeFrame;
		timeBeforeFrame = SDL_GetTicks();

		SDL_Event event;

		while (SDL_PollEvent(&event))
		{
			switch(event.type)
			{
			case SDL_KEYDOWN:
				if (keyDownFunction != NULL)
					keyDownFunction((int)event.key.keysym.sym);
				keys[event.key.keysym.sym] = true;
				break;

			case SDL_KEYUP:
				if (keyUpFunction != NULL)
					keyUpFunction((int)event.key.keysym.sym);
				keys[event.key.keysym.sym] = false;
				break;

			case SDL_MOUSEMOTION:
				if (mouseMotionFunction != NULL)
					mouseMotionFunction();
				mouseX = event.motion.x;
				mouseY = event.motion.y;
				mouseRelX = event.motion.xrel;
				mouseRelY = event.motion.yrel;
				break;

			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					if (mouseLeftButtonDownFunction != NULL)
						mouseLeftButtonDownFunction();
					mouseLeftButtonPressed = true;
				}
				else if (event.button.button == SDL_BUTTON_MIDDLE)
				{
					if (mouseMiddleButtonDownFunction != NULL)
						mouseMiddleButtonDownFunction();
					mouseMiddleButtonPressed = true;
				}
				else if (event.button.button == SDL_BUTTON_RIGHT)
				{
					if (mouseRightButtonDownFunction != NULL)
						mouseRightButtonDownFunction();
					mouseRightButtonPressed = true;
				}
				break;

			case SDL_MOUSEBUTTONUP:
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					if (mouseLeftButtonUpFunction != NULL)
						mouseLeftButtonUpFunction();
					mouseLeftButtonPressed = false;
				}
				else if (event.button.button == SDL_BUTTON_MIDDLE)
				{
					if (mouseMiddleButtonUpFunction != NULL)
						mouseMiddleButtonUpFunction();
					mouseMiddleButtonPressed = false;
				}
				else if (event.button.button == SDL_BUTTON_RIGHT)
				{
					if (mouseRightButtonUpFunction != NULL)
						mouseRightButtonUpFunction();
					mouseRightButtonPressed = false;
				}
				break;

			case SDL_USEREVENT:
				for (uint i = 0; i < timers.size(); i++)
				{
					if (event.user.code == timers[i].id)
						timers[i].eventFunction();
				}
				break;
			}
		}

		runFunction();
		if (openGLContext)
			SDL_GL_SwapBuffers();

		timeAfterFrame = SDL_GetTicks();

		if (breakRunFuctionState)
			break;
	}
}
