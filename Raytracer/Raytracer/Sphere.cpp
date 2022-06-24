#include "precomp.h"


Sphere::Sphere(int id, int mid, Vec3Df position, float radius) : Shape(id, mid)
{
	Sphere::radius = radius;
	Sphere::radiusSq = radius * radius;
	Sphere::position = position;
	Sphere::radius8 = _mm256_set1_ps(radius);
	Sphere::radiusSq8 = _mm256_set1_ps(radiusSq);
	Sphere::posX = _mm256_set1_ps(position.get_x());
	Sphere::posY = _mm256_set1_ps(position.get_y());
	Sphere::posZ = _mm256_set1_ps(position.get_z());
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

void Sphere::hit(__m256 ox, __m256 oy, __m256 oz, __m256 dx, __m256 dy, __m256 dz, __m256 len, HitInfo8* hit) {
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
	__m256 withinEyeAndDistMask = _mm256_and_ps(_mm256_cmp_ps(t, _mm256_set1_ps(0.0f), _CMP_GT_OQ), _mm256_cmp_ps(t, hit->dist, _CMP_LT_OQ));
	__m256 outsideHitMask = _mm256_and_ps(checkHitMask, withinEyeAndDistMask);

	// we precalc all masks in hope of an early out before doing expensive stuff
	__m256 completeHitMask = _mm256_or_ps(inSphereMask, outsideHitMask);
	
	if (_mm256_movemask_ps(completeHitMask) == 0) {
		return;
	}
	HitInfo8 sphereHitInfo = HitInfo8();
	

	__m256 hitPosX = _mm256_add_ps(ox, _mm256_mul_ps(dx, radius8));
	__m256 hitPosY = _mm256_add_ps(oy, _mm256_mul_ps(dy, radius8));
	__m256 hitPosZ = _mm256_add_ps(oz, _mm256_mul_ps(dz, radius8));
	AvxVector3 normal = normalize(_mm256_sub_ps(hitPosX, posX), _mm256_sub_ps(hitPosY, posY), _mm256_sub_ps(hitPosZ, posZ));
	sphereHitInfo.nx = _mm256_sub_ps(_mm256_setzero_ps(), normal.x);
	sphereHitInfo.ny = _mm256_sub_ps(_mm256_setzero_ps(), normal.y);
	sphereHitInfo.nz = _mm256_sub_ps(_mm256_setzero_ps(), normal.z);
	sphereHitInfo.px = hitPosX;
	sphereHitInfo.py = hitPosY;
	sphereHitInfo.pz = hitPosZ;
	sphereHitInfo.dist = radius8;
	// mat id is the same so thats why you dont see me reset this later
	sphereHitInfo.matId = mid8;
	
	// else (outside sphere)
	
	
	
	

	//Vec3Df hitPos = r.origin + r.direction * Vec3Df(t);
	//Vec3Df normal = normalize_vector(hitPos - position);
	//*hit = HitInfo{ normal,hitPos,t,mat, id };
	hitPosX = _mm256_add_ps(ox, _mm256_mul_ps(dx, t));
	hitPosY = _mm256_add_ps(oy, _mm256_mul_ps(dy, t));
	hitPosZ = _mm256_add_ps(oz, _mm256_mul_ps(dz, t));
	AvxVector3 normal2 = normalize(_mm256_sub_ps(hitPosX, posX), _mm256_sub_ps(hitPosY, posY), _mm256_sub_ps(hitPosZ, posZ));
	sphereHitInfo.nx = _mm256_blendv_ps(sphereHitInfo.nx, normal2.x, outsideHitMask);
	sphereHitInfo.ny = _mm256_blendv_ps(sphereHitInfo.ny, normal2.y, outsideHitMask);
	sphereHitInfo.nz = _mm256_blendv_ps(sphereHitInfo.nz, normal2.z, outsideHitMask);
	sphereHitInfo.px = _mm256_blendv_ps(sphereHitInfo.px, hitPosX, outsideHitMask);
	sphereHitInfo.py = _mm256_blendv_ps(sphereHitInfo.py, hitPosY, outsideHitMask);
	sphereHitInfo.pz = _mm256_blendv_ps(sphereHitInfo.pz, hitPosZ, outsideHitMask);
	sphereHitInfo.dist = _mm256_blendv_ps(sphereHitInfo.dist, t, outsideHitMask); 
	
	int test = _mm256_movemask_ps(completeHitMask);
	hit->px = _mm256_blendv_ps(hit->px, sphereHitInfo.px, completeHitMask);
	hit->py = _mm256_blendv_ps(hit->py, sphereHitInfo.py, completeHitMask);
	hit->pz = _mm256_blendv_ps(hit->pz, sphereHitInfo.pz, completeHitMask);
	hit->nx = _mm256_blendv_ps(hit->nx, sphereHitInfo.nx, completeHitMask);
	hit->ny = _mm256_blendv_ps(hit->ny, sphereHitInfo.ny, completeHitMask);
	hit->nz = _mm256_blendv_ps(hit->nz, sphereHitInfo.nz, completeHitMask);
	hit->dist = _mm256_blendv_ps(hit->dist, sphereHitInfo.dist, completeHitMask);
	hit->matId = _mm256_blendv_ps(hit->matId, sphereHitInfo.matId, completeHitMask);
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
