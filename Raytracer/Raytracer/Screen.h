#pragma once
class Renderer;


class Screen {
	public:
		Screen(Window* window,int posX,int posY,int width,	int height);
		~Screen();
		void prepareScreen();
		void drawTextureFromBuffer(Pixel* p);
		int getWidth();
		int getHeight();
	private:
		void initScreen();
		Window* window;
		SDL_Renderer* renderer = NULL;
		SDL_Texture* texture = NULL;
		int posX; 
		int posY;
		int width;
		int height;
};