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

void Sphere::hit(Ray8 r, HitInfo8* hit) {
	// Vec3Df c = position - r.origin;
	// float cLenSq = dot_product(c, c); 
	__m256 cx = _mm256_sub_ps(posX, r.ox);
	__m256 cy = _mm256_sub_ps(posY, r.oy);
	__m256 cz = _mm256_sub_ps(posZ, r.oz); 
	
	__m256 cLenMask = _mm256_cmp_ps(dot_product(cx, cy, cz), radiusSq8, _CMP_LT_OQ);
	
	// if (cLenSq < radiusSq)
	// if (radius < r.length) (inside sphere)
	HitInfo8 sphereHitInfo = HitInfo8();
	__m256 shortMask = _mm256_cmp_ps(radius8, r.len, _CMP_LT_OQ);
	__m256 inSphereMask = _mm256_and_ps(cLenMask, shortMask);
	__m256 hitPosX = _mm256_add_ps(r.ox, _mm256_mul_ps(r.dx, radius8));
	__m256 hitPosY = _mm256_add_ps(r.oy, _mm256_mul_ps(r.dy, radius8));
	__m256 hitPosZ = _mm256_add_ps(r.oz, _mm256_mul_ps(r.dz, radius8));
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
	
	/*float t = dot_product(c, r.direction);
	Vec3Df q = c - Vec3Df(t) * r.direction;
	float pSquared = dot_product(q, q);*/
	__m256 t = dot_product(cx, cy, cz, r.dx, r.dy, r.dz);
	__m256 qx = _mm256_sub_ps(cx, _mm256_mul_ps(r.dx, t));
	__m256 qy = _mm256_sub_ps(cy, _mm256_mul_ps(r.dy, t));
	__m256 qz = _mm256_sub_ps(cz, _mm256_mul_ps(r.dz, t));
	__m256 pSquared = dot_product(qx, qy, qz);
	__m256 checkHitMask = _mm256_cmp_ps(pSquared, radiusSq8, _CMP_LE_OQ);
	t = _mm256_sub_ps(t, _mm256_sqrt_ps(_mm256_sub_ps(radiusSq8, pSquared)));
	// 	if ((t > 0) && (t < hit->distance)) 
	__m256 withinEyeAndDistMask = _mm256_and_ps(_mm256_cmp_ps(t, _mm256_set1_ps(0.0f), _CMP_GT_OQ), _mm256_cmp_ps(t, hit->dist, _CMP_LT_OQ));
	__m256 outsideHitMask = _mm256_and_ps(checkHitMask, withinEyeAndDistMask);
	//Vec3Df hitPos = r.origin + r.direction * Vec3Df(t);
	//Vec3Df normal = normalize_vector(hitPos - position);
	//*hit = HitInfo{ normal,hitPos,t,mat, id };
	hitPosX = _mm256_add_ps(r.ox, _mm256_mul_ps(r.dx, t));
	hitPosY = _mm256_add_ps(r.oy, _mm256_mul_ps(r.dy, t));
	hitPosZ = _mm256_add_ps(r.oz, _mm256_mul_ps(r.dz, t));
	AvxVector3 normal2 = normalize(_mm256_sub_ps(hitPosX, posX), _mm256_sub_ps(hitPosY, posY), _mm256_sub_ps(hitPosZ, posZ));
	sphereHitInfo.nx = _mm256_blendv_ps(sphereHitInfo.nx, normal2.x, outsideHitMask);
	sphereHitInfo.ny = _mm256_blendv_ps(sphereHitInfo.ny, normal2.y, outsideHitMask);
	sphereHitInfo.nz = _mm256_blendv_ps(sphereHitInfo.nz, normal2.z, outsideHitMask);
	sphereHitInfo.px = _mm256_blendv_ps(sphereHitInfo.px, hitPosX, outsideHitMask);
	sphereHitInfo.py = _mm256_blendv_ps(sphereHitInfo.py, hitPosY, outsideHitMask);
	sphereHitInfo.pz = _mm256_blendv_ps(sphereHitInfo.pz, hitPosZ, outsideHitMask);
	sphereHitInfo.dist = _mm256_blendv_ps(sphereHitInfo.dist, t, outsideHitMask); 
	
	// Ok we now have the correct spherehitinfo (hopefully)
	// so now we must check if we actually hit the sphere at all for the final result
	__m256 completeHitMask = _mm256_or_ps(inSphereMask, outsideHitMask);

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
