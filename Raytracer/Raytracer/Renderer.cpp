#include "precomp.h"

Renderer::Renderer(Screen* screen)
{
	Renderer::screen = screen;
	initRenderer();
}


void Renderer::initRenderer()
{
	width = screen->getWidth();
	height = screen->getHeight();
	pixelsSize = (width*height) * 4;
	pixelBuffer = (Pixel*)MALLOC64(width * height * sizeof(Pixel) * 4);
}

void Renderer::draw(int iteration)
{
	assert(currentScene != NULL && camera != NULL);
	for (int i = 0; i < currentScene->objects.size(); i += 1)
	{
		*(pixelBuffer+i) = 6000;
	}
}

void Renderer::flush()
{
	screen->drawTextureFromBuffer(pixelBuffer);
}

void Renderer::setScene(Scene* scene)
{
	Renderer::currentScene = scene;
}

void Renderer::setCamera(Camera* camera)
{
	Renderer::camera = camera;
}

Renderer::~Renderer()
{
}
