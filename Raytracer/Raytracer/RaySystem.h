#pragma once
#include "xmmintrin.h"

class RaySystem
{
public:
	RaySystem(Screen* screen);
	
	const static int AVX_SIZE = 8;
	const static int RAY_COUNT = (SCREEN_WIDTH * SCREEN_WIDTH) / 8;
	// ray arrays
	__m256 originX[RAY_COUNT], originY[RAY_COUNT], originZ[RAY_COUNT];
	__m256 dirX[RAY_COUNT], dirY[RAY_COUNT], dirZ[RAY_COUNT];
	__m256 length[RAY_COUNT];
	__m256 r[RAY_COUNT], g[RAY_COUNT], b[RAY_COUNT];
	// hit arrays
	__m256 hitPosX[RAY_COUNT], hitPosY[RAY_COUNT], hitPosZ[RAY_COUNT];
	__m256 hitNormX[RAY_COUNT], hitNormY[RAY_COUNT], hitNormZ[RAY_COUNT];
	__m256 hitDist[RAY_COUNT];
	__m256 hitMatId[RAY_COUNT];
	__m256 iterHitPosX[RAY_COUNT], iterHitPosY[RAY_COUNT], iterHitPosZ[RAY_COUNT];
	//__m256i depth[RAY_COUNT];
	void draw(Pixel* pixelBuffer);
	void init(Scene* scene, Camera* camera);
	void drawParallel(int startIndex, int endIndex, Pixel* pixelBuffer);
	AvxVector3 trace(int ind, int depth); 

private:
	Scene* scene = NULL;
	Camera* camera = NULL;
	int width;
	int height;

	size_t shapeSize;
	std::vector<Shape*> shapes; 
	
	size_t lightSize;
	std::vector<Light*> lights;

	float xOffset, yOffset;
	// start variables
	__m256 startX, startY, startZ;
	__m256 ox, oy, oz;
	__m256 rayLen;
};