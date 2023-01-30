#ifndef COMMON_APPLICATION_H
#define COMMON_APPLICATION_H

#include "common.hpp"

#include <SDL/SDL.h>



class CApplication
{
	struct Timer
	{
		int id;
		SDL_TimerID id_SDL;
		int interval;
		void (*eventFunction)();
	};

	static Uint32 timerFunc(Uint32 interval, void* param)
	{
		SDL_Event event;

		event.type = SDL_USEREVENT;
		event.user.code = *((int*)(param));
		event.user.data1 = 0;
		event.user.data2 = 0;

		SDL_PushEvent(&event);

		return interval;
	}

public:
	void create(int screenWidth, int screenHeight, int bitsPerPixel, bool fullScreen, bool vSync, bool createOpenGLContext);
	void destroy();

	SDL_Surface* getSDLSurface() const { return SDLSurface; }

	const int& getScreenWidth() const { return screenWidth; }
	const int& getScreenHeight() const { return screenHeight; }
	const int& getScreenBitsPerPixel() const { return screenBitsPerPixel; }
	const float& getScreenAspectRatio() const { return screenAspectRatio; }
	const bool& isFullScreen() const { return fullScreen; }
	const bool& isVSync() const { return vSync; }

	int getTickCount() { return SDL_GetTicks(); }
	const int& getLastFrameTime() const { return lastFrameTime; }

	void setWindowText(const string& text) { SDL_WM_SetCaption(text.c_str(), text.c_str()); }
	void setCursorVisibility(bool visible) { SDL_ShowCursor(visible); }
	void setGrabInputState(bool state) { SDL_WM_GrabInput((SDL_GrabMode)state); }

	const bool& isKeyPressed(int key) const { return keys[key]; }

	const int& getMouseX() const { return mouseX; }
	const int& getMouseY() const { return mouseY; }
	const int& getMouseRelX() const { return mouseRelX; }
	const int& getMouseRelY() const { return mouseRelY; }

	const bool& isMouseLeftButtonPressed() const { return mouseLeftButtonPressed; }
	const bool& isMouseMiddleButtonPressed() const { return mouseMiddleButtonPressed; }
	const bool& isMouseRightButtonPressed() const { return mouseRightButtonPressed; }

	void addTimer(const int& id, int interval, void(*eventFunction)());
	void deleteTimer(int id);

	void breakRunFunction() { breakRunFuctionState = true; }

	void run(void (*runFunction)());
	void setKeyDownFunction(void (*function)(int key)) { keyDownFunction = function; }
	void setKeyUpFunction(void (*function)(int key)) { keyUpFunction = function; }
	void setMousePosition(int x, int y) { SDL_WarpMouse(x, y); }
	void setMouseMotionFunction(void (*function)()) { mouseMotionFunction = function; }
	void setMouseLeftButtonDownFunction(void (*function)()) { mouseLeftButtonDownFunction = function; }
	void setMouseMiddleButtonDownFunction(void (*function)()) { mouseMiddleButtonDownFunction = function; }
	void setMouseRightButtonDownFunction(void (*function)()) { mouseRightButtonDownFunction = function; }
	void setMouseLeftButtonUpFunction(void (*function)()) { mouseLeftButtonUpFunction = function; }
	void setMouseMiddleButtonUpFunction(void (*function)()) { mouseMiddleButtonUpFunction = function; }
	void setMouseRightButtonUpFunction(void (*function)()) { mouseRightButtonUpFunction = function; }

private:
	SDL_Surface* SDLSurface;

	int screenWidth, screenHeight, screenBitsPerPixel;
	float screenAspectRatio;
	bool fullScreen, vSync;
	bool openGLContext;

	int lastFrameTime, timeBeforeFrame, timeAfterFrame;

	bool keys[512];
	int mouseX, mouseY, mouseRelX, mouseRelY;
	bool mouseLeftButtonPressed, mouseMiddleButtonPressed, mouseRightButtonPressed;

	vector<Timer> timers;

	bool breakRunFuctionState;

	void (*runFunction)();
    void (*keyDownFunction)(int key);
    void (*keyUpFunction)(int key);
	void (*mouseMotionFunction)();
	void (*mouseLeftButtonDownFunction)();
	void (*mouseMiddleButtonDownFunction)();
	void (*mouseRightButtonDownFunction)();
	void (*mouseLeftButtonUpFunction)();
	void (*mouseMiddleButtonUpFunction)();
	void (*mouseRightButtonUpFunction)();
};
extern CApplication Application;



#endif
