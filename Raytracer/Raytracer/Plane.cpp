#include "precomp.h"

Plane::Plane(int id, int mid, Vec3Df normal, float distance) : Shape(id, mid)
{
	type = 0;
	Plane::distance = distance;
	Plane::normal = normal;
	Plane::nx = _mm256_set1_ps(normal.get_x());
	Plane::ny = _mm256_set1_ps(normal.get_y());
	Plane::nz = _mm256_set1_ps(normal.get_z());
	Plane::dist = _mm256_set1_ps(distance);
}

Plane::~Plane()
{
}

void Plane::hit(Ray r, HitInfo* hit)
{
	float t = -(dot_product(r.origin, normal) + distance) / (dot_product(r.direction, normal));
	if (t >= 0 && t < hit->distance)
	{
		*hit = HitInfo{ -normal,r.origin + r.direction * t,t, mat, id};
	}
}

void Plane::hit(__m256 ox, __m256 oy, __m256 oz, __m256 dx, __m256 dy, __m256 dz, __m256 len, __m256& hitNormX, __m256& hitNormY, __m256& hitNormZ, __m256& hitPosX, __m256& hitPosY, __m256& hitPosZ, __m256& hitDist, __m256& hitMatId)
{
	__m256 b = dot_product(dx, dy, dz, nx, ny, nz); 
	__m256 a = _mm256_sub_ps(_mm256_setzero_ps(), _mm256_add_ps(dist, dot_product(ox, oy, oz, nx, ny, nz)));
	__m256 t = _mm256_div_ps(a, b);
	__m256 tPositiveMask = _mm256_cmp_ps(t, _mm256_setzero_ps(), _CMP_GE_OS);
	__m256 closerCollisionMask = _mm256_cmp_ps(t, hitDist, _CMP_LT_OS);
	__m256 collisionMask = _mm256_and_ps(tPositiveMask, closerCollisionMask);

	if (_mm256_movemask_ps(collisionMask) == 0) {
		return;
	}
	hitNormX = _mm256_blendv_ps(hitNormX, _mm256_sub_ps(_mm256_setzero_ps(), nx), collisionMask);
	hitNormY = _mm256_blendv_ps(hitNormY, _mm256_sub_ps(_mm256_setzero_ps(), ny), collisionMask);
	hitNormZ = _mm256_blendv_ps(hitNormZ, _mm256_sub_ps(_mm256_setzero_ps(), nz), collisionMask);
	hitPosX = _mm256_blendv_ps(hitPosX, _mm256_add_ps(ox, _mm256_mul_ps(dx, t)), collisionMask);
	hitPosY = _mm256_blendv_ps(hitPosY, _mm256_add_ps(oy, _mm256_mul_ps(dy, t)), collisionMask);
	hitPosZ = _mm256_blendv_ps(hitPosZ, _mm256_add_ps(oz, _mm256_mul_ps(dz, t)), collisionMask);
	hitDist = _mm256_blendv_ps(hitDist, t, collisionMask);
	hitMatId = _mm256_blendv_ps(hitMatId, mid8, collisionMask);
}

bool Plane::fastHit(Ray r)
{
	float t = -(dot_product(r.origin, normal) + distance) / (dot_product(r.direction, normal));
	return t >= 0;
}
