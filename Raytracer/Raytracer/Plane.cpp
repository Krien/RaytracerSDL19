#include "precomp.h"

Plane::Plane(int id, Material material, Vec3Df normal, float distance) : Shape(id, Vec3Df(0), material)
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
		*hit = HitInfo{ -normal,r.origin + r.direction * t,t,material, id };
	}
}

bool Plane::fastHit(Ray r)
{
	float t = -(dot_product(r.origin, normal) + distance) / (dot_product(r.direction, normal));
	return t >= 0;
}
