#pragma once

#define BLUE_REFRACT 0
#define SOLID_RED 1
#define SOLID_GREEN 2
#define SOLID_BLUE 3
#define PINK_WALL 4
class Shape {
public:
	Shape(int id, int mid);
	~Shape();
	int id;
	int type;
	__m256 mid8;
	__m256i id8;
	virtual void hit(Ray r, HitInfo* current);
	virtual void hit(__m256 ox, __m256 oy, __m256 oz, __m256 dx, __m256 dy, __m256 dz, __m256 len, HitInfo8* hit);
	virtual bool fastHit(Ray r);
	Material mat;
	static Mat8* initMatLut8(const Material matLUT[5]);
	static Material getMat(int mid);
	static Mat8 getMat8(int mid); 
	static Mat8 blendMats(__m256 matIds);  
};