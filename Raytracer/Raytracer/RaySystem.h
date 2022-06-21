#pragma once
#include "xmmintrin.h"

class RaySystem
{
public:
	RaySystem(Screen* screen);
	
	const static int AVX_SIZE = 8;
	/*__m256 ox8[RAY_COUNT];
	__m256 oy8[RAY_COUNT];
	__m256 oz8[RAY_COUNT];
	__m256 dx8[RAY_COUNT];
	__m256 dy8[RAY_COUNT];
	__m256 dz8[RAY_COUNT];
	__m256 len8[RAY_COUNT];
	__m256i depth8[RAY_COUNT];*/
	void draw(Pixel* pixelBuffer);
	void init(Scene* scene, Camera* camera);
	__m256* trace(__m256 rayDirX, __m256 rayDirY, __m256 rayDirZ, __m256 originX, __m256 originY, __m256 originZ, int depth);
	HitInfo* hit();

private:
	Scene* scene = NULL;
	Camera* camera = NULL;
	int width;
	int height;
};