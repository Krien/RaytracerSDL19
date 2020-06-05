#pragma once
class Plane : public Shape
{
public:
	Vec3Df normal;
	float distance;
	Plane(int id, Material material, Vec3Df normal, float distance);
	~Plane();
	void hit(Ray r, HitInfo* hit) override;
	bool fastHit(Ray r) override;
};