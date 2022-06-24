#pragma once
class Plane : public Shape
{
public:
	Vec3Df normal;
	float distance;
	__m256 nx;
	__m256 ny;
	__m256 nz;
	__m256 dist;
	Plane(int id, int mid, Vec3Df normal, float distance);
	~Plane();
	void hit(Ray r, HitInfo* hit) override;
	void hit(__m256 ox, __m256 oy, __m256 oz, __m256 dx, __m256 dy, __m256 dz, __m256 len, HitInfo8* hit) override;
	bool fastHit(Ray r) override;
};