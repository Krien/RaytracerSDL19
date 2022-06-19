#include "precomp.h"

Plane::Plane(int id, int mid, Vec3Df normal, float distance) : Shape(id, mid)
{
	Plane::distance = distance;
	Plane::normal = normal;
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

bool Plane::fastHit(Ray r)
{
	float t = -(dot_product(r.origin, normal) + distance) / (dot_product(r.direction, normal));
	return t >= 0;
}
