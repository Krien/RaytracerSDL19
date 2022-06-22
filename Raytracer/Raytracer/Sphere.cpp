#include "precomp.h"


Sphere::Sphere(int id, int mid, Vec3Df position, float radius) : Shape(id, mid)
{
	Sphere::radius = radius;
	Sphere::radiusSq = radius * radius;
	Sphere::position = position;
}

bool Sphere::hit(Ray r, HitInfo* hit)
{
	Vec3Df c = position - r.origin;
	float cLenSq = dot_product(c, c);
	// In sphere
	if (cLenSq < radiusSq)
	{
		if (radius < r.length)
		{
			Vec3Df hitPos = r.origin + r.direction * Vec3Df(radius);
			Vec3Df normal = normalize_vector(hitPos - position);
			*hit = HitInfo{ -normal,hitPos,radius,mat, id };
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
			if ((t > 0) && (t < hit->distance)) 
			{
				Vec3Df hitPos = r.origin + r.direction * Vec3Df(t);
				Vec3Df normal = normalize_vector(hitPos - position);
				*hit = HitInfo{ normal,hitPos,t,mat, id };
				return true;
			}
		}
	}
	return false;
}

bool Sphere::fastHit(Ray r)
{
	Vec3Df c = position - r.origin;
	float cLenSq = dot_product(c, c);
	// In sphere
	if (cLenSq < radiusSq)
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
