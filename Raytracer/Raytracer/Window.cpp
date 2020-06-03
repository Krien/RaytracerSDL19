#include "precomp.h"

Window::Window(const char* name, int posX, int posY, int screenWidth, int screenHeight)
{
	Window::name = name;
	Window::posX = posX;
	Window::posY = posY;
	Window::screenWidth = screenWidth;
	Window::screenHeight = screenHeight;
	initWindow();
}

void Window::initWindow()
{
	// Create an application window with the following settings:
	window = SDL_CreateWindow(
		name,                  // window title
		posX,                  // initial x position
		posY,                  // initial y position
		screenWidth,           // width, in pixels
		screenHeight,          // height, in pixels
		SDL_WINDOW_OPENGL      // flags - see below
	);

	// Check that the window was successfully created
	if (window == NULL) {
		// In the case that the window could not be made...
		printf("Could not create window: %s\n", SDL_GetError());
	}
}


Window::~Window()
{
	SDL_DestroyWindow(window);
	window = NULL;
}

SDL_Window* Window::getWindow()
{
	return window;
}

int Window::getScreenWidth()
{
	return screenWidth;
}

int Window::getScreenHeight()
{
	return screenHeight;
}


