#pragma once
#include "xmmintrin.h"

struct Mat_Avx {
	__m256 ambx, amby, ambz;
	__m256 diffx, diffy, diffz;
	__m256 specx, specy, specz;
	__m256 absx, absy, absz;
	__m256 mirror;
	__m256 refracId;
};

struct HitInfo_Avx {
	__m256 nx, ny, nz;
	__m256 px, py, pz;
	__m256 dist;
	Mat_Avx mat;
	__m256i id;
};

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
	AvxVector3 trace(int ind, int depth);
	void hit(int ind);

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
	__m256 startX, startY;
	__m256 ox, oy, oz;
	__m256 rayLen;

	HitInfo_Avx hitInfo;
};