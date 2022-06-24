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
		void hit(__m256 ox, __m256 oy, __m256 oz, __m256 dx, __m256 dy, __m256 dz, __m256 len, HitInfo8* hit) override;
		bool fastHit(Ray r) override;
};