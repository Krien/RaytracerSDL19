#include "precomp.h"

Screen::Screen(Window* window, int posX, int posY, int width, int height)
{
	Screen::window = window;
	Screen::posX = posX;
	Screen::posY = posY;
	Screen::width = width;
	Screen::height = height;
	initScreen();
}

Screen::~Screen()
{
}

void Screen::initScreen()
{
	// First validate request
	if (posX < 0 || posX + width > window->getScreenWidth()
		|| posY < 0 || posY + height > window->getScreenHeight())
	{
		printf("ERROR: screen to big for window!");
	}
	// First create the renderer.
	renderer = SDL_CreateRenderer(window->getWindow(), -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL) {
		// In the case that the window could not be made...
		printf("Could not create renderer: %s\n", SDL_GetError());
		return;
	}
	// Then build the texture to draw to.
	texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STREAMING,
		width, height);
	if (texture == NULL) {
		// In the case that the window could not be made...
		printf("Could not create texture: %s\n", SDL_GetError());
		return;
	}

	SDL_RendererInfo info;
	SDL_GetRendererInfo(renderer, &info);
	printf("Created texture %i %i %i %i: \n", posX, posY, width, height);
	printf("	-Renderer name: %s \n", info.name);
	printf("	-Texture Formats: \n");
	for (Uint32 i = 0; i < info.num_texture_formats; i++)
	{
		printf("		#%s \n", SDL_GetPixelFormatName(info.texture_formats[i]));
	}
}

void Screen::prepareScreen()
{

}

void Screen::drawTextureFromBuffer(Pixel* p)
{
	SDL_UpdateTexture(texture, NULL, &((*p)), width * 4); //copy entire array only once
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

int Screen::getWidth()
{
	return width;
}

int Screen::getHeight()
{
	return height;
}
