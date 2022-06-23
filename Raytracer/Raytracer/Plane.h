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
	void hit(Ray8 ray, HitInfo8* hit) override;
	bool fastHit(Ray r) override;
};