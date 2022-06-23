#pragma once
class Sphere : public Shape 
{
	public:
		float radius;
		float radiusSq;
		Vec3Df position;
		__m256 radius8;
		__m256 radiusSq8;
		__m256 posX;
		__m256 posY;
		__m256 posZ;
		Sphere(int id, int mid, Vec3Df position, float radius);
		~Sphere();
		void hit(Ray r, HitInfo* hit) override;
		void hit(Ray8 ray, HitInfo8* hit) override;
		bool fastHit(Ray r) override;
};