#include "precomp.h"
#include "xmmintrin.h"
#define SIZE SCREEN_WIDTH * SCREEN_HEIGHT

RaySystem::RaySystem(Screen* screen)
{

	this->width = screen->getWidth();
	this->height = screen->getHeight();
}

void RaySystem::init(Scene* scene, Camera* camera) {
	this->scene = scene;
	this->camera = camera;
}

void RaySystem::draw(Pixel* pixelBuffer) {
	Vec3Df startDir = camera->getRelTopLeft();
	Vec3Df camPos = camera->position;
	const static float xOffset = (float)SCREEN_DIMENSION * 2 / (width - 1);
	const static float yOffset = (float)SCREEN_DIMENSION * 2 / (height - 1);

	const __m256 startX = _mm256_set1_ps(startDir.get_x());
	const __m256 startY = _mm256_set1_ps(startDir.get_y());
	const __m256 startZ = _mm256_set1_ps(startDir.get_z());

	const __m256 originX = _mm256_set1_ps(camPos.get_x());
	const __m256 originY = _mm256_set1_ps(camPos.get_y());
	const __m256 originZ = _mm256_set1_ps(camPos.get_z());

	const __m256 rayLen = _mm256_set1_ps(100);

	// #pragma unroll
	for (unsigned int i = 0; i < (height * width) / AVX_SIZE; i++)
	{
		int x = i * AVX_SIZE % width;
		int y = i * AVX_SIZE / width;

		__m256 dirX = _mm256_add_ps(startX,
			_mm256_set_ps(x * xOffset, x + 1 * xOffset, x + 2 * xOffset, x + 3 * xOffset, x + 4 * xOffset, x + 5 * xOffset, x + 6 * xOffset, x + 7 * xOffset));
		__m256 dirY = _mm256_add_ps(startY,
			_mm256_set_ps(y * yOffset, y + 1 * yOffset, y + 2 * yOffset, y + 3 * yOffset, y + 4 * yOffset, y + 5 * yOffset, y + 6 * yOffset, y + 7 * yOffset));
		__m256 dirZ = _mm256_setzero_ps();

		__m256* argb = trace(dirX, dirY, dirZ, originX, originY, originZ, 0);
		// Convert color, but since I dunno what the trace function will return im leaving this empty forn ow
	}

	/*
	Vec3Df rayStartDir = camera->getRelTopLeft();
	float xOffset = (float)SCREEN_DIMENSION * 2 / (width - 1);
	float yOffset = (float)SCREEN_DIMENSION * 2 / (height - 1);
	#pragma unroll
	for (unsigned int y = 0; y < height; y++)
	{
		for (unsigned int x = 0; x < width; x++)
		{
			// Make ray
			Vec3Df direction = rayStartDir + Vec3Df(x * xOffset, y * yOffset, 0);
			direction = normalize_vector(direction);
			Ray r = { camera->position, direction, 100 };
			// Check for hits
			Vec3Df argb = trace(r,0) * Vec3Df(255);

			// Convert color
			unsigned int xy = x * 4 + (y * width) * 4;
			*(pixelBuffer + xy) = std::min((int)argb.extract(0), 255);
			*(pixelBuffer + xy+1) = std::min((int)argb.extract(1), 255);
			*(pixelBuffer + xy+2) = std::min((int)argb.extract(2), 255);
			*(pixelBuffer + xy + 3) = 0;
		}
	};*/
}


// No idea what this should return, but probably a colorX, colorY and colorZ thats why its returning a __m256 array 
__m256* RaySystem::trace(__m256 rayDirX, __m256 rayDirY, __m256 rayDirZ, __m256 originX, __m256 originY, __m256 originZ, int depth)
{
	return NULL;
}

HitInfo* RaySystem::hit()
{
	return NULL;
}
