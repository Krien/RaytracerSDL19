#pragma once
class Sphere : public Shape 
{
	public:
		float radius;
		float radiusSq;
		Vec3Df position;
		Sphere(int id, int mid, Vec3Df position, float radius);
		~Sphere();
		void hit(Ray r, HitInfo* hit) override;
		bool fastHit(Ray r) override;
};