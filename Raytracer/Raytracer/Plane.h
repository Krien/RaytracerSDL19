#pragma once
class Plane : public Shape
{
public:
	Vec3Df normal;
	float distance;
	Plane(int id, int mid, Vec3Df normal, float distance);
	~Plane();
	bool hit(Ray r, HitInfo* hit) override;
	bool fastHit(Ray r) override;
};