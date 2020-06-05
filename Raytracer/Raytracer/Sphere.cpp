#include "precomp.h"


Sphere::Sphere(int id, Vec3Df position, Material material, float radius) : Shape(id, position, material)
{
	Sphere::radius = radius;
	Sphere::radiusSq = radius * radius;
}

void Sphere::hit(Ray r, HitInfo* hit)
{
	Vec3Df c = position - r.origin;
	// In sphere
	if (vector_length(c) < radius)
	{
		if (radius < r.length)
		{
			Vec3Df hitPos = r.origin + r.direction * Vec3Df(radius);
			Vec3Df normal = normalize_vector(hitPos - position);
			*hit = HitInfo{ -normal,hitPos,radius,material, id };
		}
	}
	else
	{
		float t = dot_product(c, r.direction);
		Vec3Df q = c - Vec3Df(t) * r.direction;
		float pSquared = dot_product(q, q);
		if (pSquared <= radiusSq)
		{
			t -= sqrtf(radiusSq - pSquared);
			if ((t > 0) && (t < hit->distance)) 
			{
				Vec3Df hitPos = r.origin + r.direction * Vec3Df(t);
				Vec3Df normal = normalize_vector(hitPos - position);
				*hit = HitInfo{ normal,hitPos,t,material, id };
				int b = 0;
			}
		}
	}
}

bool Sphere::fastHit(Ray r)
{
	Vec3Df c = position - r.origin;
	// In sphere
	if (vector_length(c) < radius)
	{
		if (radius < r.length)
		{
			return true;
		}
	}
	else
	{
		float t = dot_product(c, r.direction);
		Vec3Df q = c - Vec3Df(t) * r.direction;
		float pSquared = dot_product(q, q);
		if (pSquared <= radiusSq)
		{
			t -= sqrtf(radiusSq - pSquared);
			if (t > 0)
			{
				return true;
			}
		}
	}
	return false;
}
