#pragma once
class Sphere : public Shape 
{
	public:
		float radius;
		float radiusSq;
		Sphere(int id, Vec3Df position, Material material, float radius);
		~Sphere();
		void hit(Ray r, HitInfo* hit) override;
		bool fastHit(Ray r) override;
};