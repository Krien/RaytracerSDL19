#pragma once
#include "xmmintrin.h"

class RaySystem
{
public:
	RaySystem(Screen* screen);
	
	const static int AVX_SIZE = 8;
	const static int RAY_COUNT = (SCREEN_WIDTH * SCREEN_WIDTH) / 8;
	__m256 originX[RAY_COUNT];
	__m256 originY[RAY_COUNT];
	__m256 originZ[RAY_COUNT];
	__m256 dirX[RAY_COUNT];
	__m256 dirY[RAY_COUNT];
	__m256 dirZ[RAY_COUNT];
	__m256 length[RAY_COUNT];
	__m256 r[RAY_COUNT];
	__m256 g[RAY_COUNT];
	__m256 b[RAY_COUNT];
	//__m256i depth[RAY_COUNT];
	void draw(Pixel* pixelBuffer);
	void init(Scene* scene, Camera* camera);
	void trace(int ind, int depth);
	HitInfo* hit();

private:
	Scene* scene = NULL;
	Camera* camera = NULL;
	int width;
	int height;
};