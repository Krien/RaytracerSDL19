#include "precomp.h"

Plane::Plane(int id, int mid, Vec3Df normal, float distance) : Shape(id, mid)
{
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

void Plane::hit(Ray8 ray, HitInfo8* hit)
{
	__m256 a = _mm256_sub_ps(_mm256_setzero_ps(), _mm256_add_ps(dist, dot_product(ray.ox, ray.oy, ray.oz, nx, ny, nz)));
	__m256 t = _mm256_div_ps(a, dot_product(ray.dx, ray.dy, ray.dz, nx, ny, nz));
	__m256 tPositiveMask = _mm256_cmp_ps(t, _mm256_setzero_ps(), _CMP_GE_OS);
	__m256 closerCollisionMask = _mm256_cmp_ps(t, hit->dist, _CMP_LT_OS);
	__m256 collisionMask = _mm256_and_ps(tPositiveMask, closerCollisionMask);
	hit->nx = _mm256_blendv_ps(hit->nx, _mm256_sub_ps(_mm256_setzero_ps(), nx), collisionMask);
	hit->ny = _mm256_blendv_ps(hit->ny, _mm256_sub_ps(_mm256_setzero_ps(), ny), collisionMask);
	hit->nz = _mm256_blendv_ps(hit->nz, _mm256_sub_ps(_mm256_setzero_ps(), nz), collisionMask);
	
	hit->px = _mm256_blendv_ps(hit->px, _mm256_add_ps(ray.ox, _mm256_mul_ps(ray.dx, t)), collisionMask);
	hit->py = _mm256_blendv_ps(hit->py, _mm256_add_ps(ray.oy, _mm256_mul_ps(ray.dy, t)), collisionMask);
	hit->pz = _mm256_blendv_ps(hit->pz, _mm256_add_ps(ray.oz, _mm256_mul_ps(ray.dz, t)), collisionMask);
	hit->dist = _mm256_blendv_ps(hit->dist, t, collisionMask);
	hit->matId = _mm256_blendv_ps(hit->matId, mid8, collisionMask); 
}

bool Plane::fastHit(Ray r)
{
	float t = -(dot_product(r.origin, normal) + distance) / (dot_product(r.direction, normal));
	return t >= 0;
}
