#pragma once
class Window {
public:
	Window(const char* name, int posX, int posY, int screenWidth, int screenHeight);
	~Window();
	SDL_Window* getWindow();
	int getScreenWidth();
	int getScreenHeight();
private:
	void initWindow();
	SDL_Window* window = NULL;
	int posX;
	int posY;
	int screenWidth;
	int screenHeight;
	const char* name;
};