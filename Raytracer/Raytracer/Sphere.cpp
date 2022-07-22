#include "precomp.h"


Sphere::Sphere(int id, int mid, Vec3Df position, float radius) : Shape(id, mid)
{
	type = 1;
	Sphere::radius = radius;
	Sphere::radiusSq = radius * radius;
	Sphere::position = position;
	Sphere::radius8 = _mm256_set1_ps(radius);
	Sphere::radiusSq8 = _mm256_set1_ps(radiusSq);
	Sphere::posX = _mm256_set1_ps(position.get_x());
	Sphere::posY = _mm256_set1_ps(position.get_y());
	Sphere::posZ = _mm256_set1_ps(position.get_z());
}

Sphere::~Sphere()
{
}

void Sphere::hit(Ray r, HitInfo* hit)
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
			}
		}
	}
}

void Sphere::hit(__m256 ox, __m256 oy, __m256 oz, __m256 dx, __m256 dy, __m256 dz, __m256 len, __m256& hitNormX, __m256& hitNormY, __m256& hitNormZ, __m256& hitPosX, __m256& hitPosY, __m256& hitPosZ, __m256& hitDist, __m256& hitMatId) {
	// Vec3Df c = position - r.origin;
	// float cLenSq = dot_product(c, c);  

	// Vec3Df c = position - r.origin;
	// float cLenSq = dot_product(c, c); 
	__m256 cx = _mm256_sub_ps(posX, ox);
	__m256 cy = _mm256_sub_ps(posY, oy);
	__m256 cz = _mm256_sub_ps(posZ, oz);

	/*float t = dot_product(c, r.direction);
	Vec3Df q = c - Vec3Df(t) * r.direction;
	float pSquared = dot_product(q, q);*/
	__m256 t = dot_product(cx, cy, cz, dx, dy, dz);
	__m256 qx = _mm256_sub_ps(cx, _mm256_mul_ps(dx, t));
	__m256 qy = _mm256_sub_ps(cy, _mm256_mul_ps(dy, t));
	__m256 qz = _mm256_sub_ps(cz, _mm256_mul_ps(dz, t));
	__m256 pSquared = dot_product(qx, qy, qz);
	t = _mm256_sub_ps(t, _mm256_sqrt_ps(_mm256_sub_ps(radiusSq8, pSquared)));

	// if (cLenSq < radiusSq)
	__m256 cLenMask = _mm256_cmp_ps(dot_product(cx, cy, cz), radiusSq8, _CMP_LT_OQ);
	// if (radius < r.length) (inside sphere)
	__m256 shortMask = _mm256_cmp_ps(radius8, len, _CMP_LT_OQ);
	__m256 inSphereMask = _mm256_and_ps(cLenMask, shortMask);

	// if (pSquared <= radiusSq)
	__m256 checkHitMask = _mm256_cmp_ps(pSquared, radiusSq8, _CMP_LE_OQ);
	// 	if ((t > 0) && (t < hit->distance)) 
	__m256 withinEyeAndDistMask = _mm256_and_ps(_mm256_cmp_ps(t, _mm256_set1_ps(0.0f), _CMP_GT_OQ), _mm256_cmp_ps(t, hitDist, _CMP_LT_OQ));
	__m256 outsideHitMask = _mm256_and_ps(checkHitMask, withinEyeAndDistMask);

	// we precalc all masks in hope of an early out before doing expensive stuff
	__m256 completeHitMask = _mm256_or_ps(inSphereMask, outsideHitMask);

	if (_mm256_movemask_ps(completeHitMask) == 0) {
		return;
	}

	__m256 hitVecX = _mm256_add_ps(ox, _mm256_mul_ps(dx, radius8));
	__m256 hitVecY = _mm256_add_ps(oy, _mm256_mul_ps(dy, radius8));
	__m256 hitVecZ = _mm256_add_ps(oz, _mm256_mul_ps(dz, radius8));
	AvxVector3 normal = normalize(_mm256_sub_ps(hitVecX, posX), _mm256_sub_ps(hitVecY, posY), _mm256_sub_ps(hitVecZ, posZ));
	__m256 sphereHitNormX = _mm256_sub_ps(_mm256_setzero_ps(), normal.x);
	__m256 sphereHitNormY = _mm256_sub_ps(_mm256_setzero_ps(), normal.y);
	__m256 sphereHitNormZ = _mm256_sub_ps(_mm256_setzero_ps(), normal.z);
	__m256 sphereHitPosX = hitVecX;
	__m256 sphereHitPosY = hitVecY;
	__m256 sphereHitPosZ = hitVecZ;
	__m256 sphereHitDist = radius8;
	// mat id is the same so thats why you dont see me reset this later
	__m256 sphereHitMatId = mid8;

	// else (outside sphere)
	//Vec3Df hitPos = r.origin + r.direction * Vec3Df(t);
	//Vec3Df normal = normalize_vector(hitPos - position);
	//*hit = HitInfo{ normal,hitPos,t,mat, id };
	hitVecX = _mm256_add_ps(ox, _mm256_mul_ps(dx, t));
	hitVecY = _mm256_add_ps(oy, _mm256_mul_ps(dy, t));
	hitVecZ = _mm256_add_ps(oz, _mm256_mul_ps(dz, t));
	AvxVector3 normal2 = normalize(_mm256_sub_ps(hitVecX, posX), _mm256_sub_ps(hitVecY, posY), _mm256_sub_ps(hitVecZ, posZ));
	sphereHitNormX = _mm256_blendv_ps(sphereHitNormX, normal2.x, outsideHitMask);
	sphereHitNormY = _mm256_blendv_ps(sphereHitNormY, normal2.y, outsideHitMask);
	sphereHitNormZ = _mm256_blendv_ps(sphereHitNormZ, normal2.z, outsideHitMask);
	sphereHitPosX = _mm256_blendv_ps(sphereHitPosX, hitVecX, outsideHitMask);
	sphereHitPosY = _mm256_blendv_ps(sphereHitPosY, hitVecY, outsideHitMask);
	sphereHitPosZ = _mm256_blendv_ps(sphereHitPosZ, hitVecZ, outsideHitMask);
	sphereHitDist = _mm256_blendv_ps(sphereHitDist, t, outsideHitMask);

	hitPosX = _mm256_blendv_ps(hitPosX, sphereHitPosX, completeHitMask);
	hitPosY = _mm256_blendv_ps(hitPosY, sphereHitPosY, completeHitMask);
	hitPosZ = _mm256_blendv_ps(hitPosZ, sphereHitPosZ, completeHitMask);
	hitNormX = _mm256_blendv_ps(hitNormX, sphereHitNormX, completeHitMask);
	hitNormY = _mm256_blendv_ps(hitNormY, sphereHitNormY, completeHitMask);
	hitNormZ = _mm256_blendv_ps(hitNormZ, sphereHitNormZ, completeHitMask);
	hitDist = _mm256_blendv_ps(hitDist, sphereHitDist, completeHitMask);
	hitMatId = _mm256_blendv_ps(hitMatId, sphereHitMatId, completeHitMask);
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
