#include "precomp.h"
#include "xmmintrin.h"
#define SIZE SCREEN_WIDTH * SCREEN_HEIGHT

RaySystem::RaySystem(Screen* screen)
{
	this->width = screen->getWidth();
	this->height = screen->getHeight();
}

void RaySystem::init(Scene* scene, Camera* camera) {
	this->scene = scene;
	this->camera = camera;

	shapes = scene->objects;
	shapeSize = shapes.size();
	lights = scene->lights;
	lightSize = lights.size();

	Vec3Df startDir = camera->getRelTopLeft();
	Vec3Df camPos = camera->position;
	xOffset = (float)SCREEN_DIMENSION * 2 / (width - 1);
	yOffset = (float)SCREEN_DIMENSION * 2 / (height - 1);

	startX = _mm256_set1_ps(startDir.get_x());
	startY = _mm256_set1_ps(startDir.get_y());

	ox = _mm256_set1_ps(camPos.get_x());
	oy = _mm256_set1_ps(camPos.get_y());
	oz = _mm256_set1_ps(camPos.get_z());

	rayLen = _mm256_set1_ps(100);
}

void RaySystem::draw(Pixel* pixelBuffer) {

	// #pragma unroll
	// Initialize ray values
	for (unsigned int i = 0; i < (height * width) / AVX_SIZE; i++)
	{
		int x = i * AVX_SIZE % width;
		int y = i * AVX_SIZE / width;

		__m256 dx = _mm256_add_ps(startX,
			_mm256_set_ps(x * xOffset, x + 1 * xOffset, x + 2 * xOffset, x + 3 * xOffset, x + 4 * xOffset, x + 5 * xOffset, x + 6 * xOffset, x + 7 * xOffset));
		__m256 dy = _mm256_add_ps(startY,
			_mm256_set_ps(y * yOffset, y + 1 * yOffset, y + 2 * yOffset, y + 3 * yOffset, y + 4 * yOffset, y + 5 * yOffset, y + 6 * yOffset, y + 7 * yOffset));
		__m256 dz = _mm256_setzero_ps(); 
		AvxVector3 norm = normalize(dx, dy, dz); 

		dirX[i] = norm.x;
		dirY[i] = norm.y;
		dirZ[i] = norm.z;
		originX[i] = ox;
		originY[i] = oy;
		originZ[i] = oz;
		length[i] = rayLen; 
	}

	for (unsigned int i = 0; i < (height * width) / AVX_SIZE; i++)
	{
		trace(i, 0);
		r[i] = _mm256_min_ps(r[i], _mm256_setzero_ps());
		g[i] = _mm256_min_ps(r[i], _mm256_setzero_ps());
		b[i] = _mm256_min_ps(r[i], _mm256_setzero_ps());

		// Please check later if this is correct lol
		unsigned int startIndex = i * AVX_SIZE * 4;
		for (unsigned int c = 0; c < 8; c++) {
			*(pixelBuffer + startIndex) = r[i].m256_f32[c];
			*(pixelBuffer + startIndex + 1) = g[i].m256_f32[c];
			*(pixelBuffer + startIndex + 2) = b[i].m256_f32[c];
			*(pixelBuffer + startIndex + 3) = 0;
			startIndex += 4;
		} 
	}

	/*
	Vec3Df rayStartDir = camera->getRelTopLeft();
	float xOffset = (float)SCREEN_DIMENSION * 2 / (width - 1);
	float yOffset = (float)SCREEN_DIMENSION * 2 / (height - 1);
	#pragma unroll
	for (unsigned int y = 0; y < height; y++)
	{
		for (unsigned int x = 0; x < width; x++)
		{
			// Make ray
			Vec3Df direction = rayStartDir + Vec3Df(x * xOffset, y * yOffset, 0);
			direction = normalize_vector(direction);
			Ray r = { camera->position, direction, 100 };
			// Check for hits
			Vec3Df argb = trace(r,0) * Vec3Df(255);

			// Convert color
			unsigned int xy = x * 4 + (y * width) * 4;
			*(pixelBuffer + xy) = std::min((int)argb.extract(0), 255);
			*(pixelBuffer + xy+1) = std::min((int)argb.extract(1), 255);
			*(pixelBuffer + xy+2) = std::min((int)argb.extract(2), 255);
			*(pixelBuffer + xy + 3) = 0;
		}
	};*/
}


// No idea what this should return, but probably a colorX, colorY and colorZ thats why its returning a __m256 array 
AvxVector3 RaySystem::trace(int ind, int depth)
{
	__m256 zero8 = _mm256_setzero_ps();
	AvxVector3 zeroVec = { zero8, zero8, zero8 };
	if (depth > RAYTRACER_RECURSION_DEPTH) return zeroVec;

	hit(ind);
	__m256 distMask = _mm256_cmp_ps(hitInfo.dist, _mm256_set1_ps(RAYTRACER_MAX_RENDERDISTANCE), _CMP_GT_OS);
	__m256 one8 = _mm256_set1_ps(1);
	__m256 matRefracIndex = hitInfo.mat.refracId;
	__m256 refracMask = _mm256_cmp_ps(matRefracIndex, one8, _CMP_GT_OS);

	__m256 dx = dirX[ind];
	__m256 dy = dirY[ind];
	__m256 dz = dirZ[ind];
	__m256 ox = originX[ind];
	__m256 oy = originY[ind];
	__m256 oz = originZ[ind];
	__m256 len = length[ind];

	AvxVector3 rayDir = { dx, dy, dz };
	AvxVector3 hitNormal = { hitInfo.nx, hitInfo.ny, hitInfo.nz };
	__m256 dotDirNor = dot_product(rayDir, hitNormal);
	__m256 condMask = _mm256_cmp_ps(dotDirNor, zero8, _CMP_LT_OS);

	__m256 refracIndex = _mm256_blendv_ps(one8, _mm256_mul_ps(matRefracIndex, matRefracIndex), condMask);
	__m256 cosTheta = _mm256_div_ps(_mm256_sub_ps(one8, _mm256_mul_ps(_mm256_set1_ps(1.000586f), _mm256_sub_ps(_mm256_set1_ps(1), _mm256_mul_ps(dotDirNor, dotDirNor)))), _mm256_mul_ps(refracIndex, refracIndex));
	__m256 refractedMask = _mm256_cmp_ps(cosTheta, zero8, _CMP_GT_OS);
	AvxVector3 sinPhi = mul(sub(rayDir, mul(hitNormal, dotDirNor)), refracIndex);
	AvxVector3 refDir = normalize(sub(sinPhi, mul(hitNormal, _mm256_sqrt_ps(cosTheta))));
	AvxVector3 refractDir = blend(refDir, zeroVec, refractedMask);
	AvxVector3 negMatAbs = { -hitInfo.mat.absx, -hitInfo.mat.absy, -hitInfo.mat.absz };
	AvxVector3 power = mul(negMatAbs, refractDir);
	__m256 e8 = _mm256_set1_ps(E);
	AvxVector3 kp8 = { _mm256_pow_ps(e8, power.x), _mm256_pow_ps(e8, power.y), _mm256_pow_ps(e8, power.z) };
	AvxVector3 k = blend(kp8, zeroVec, _mm256_andnot_ps(refracMask, condMask));

	__m256 R0 = _mm256_div_ps(_mm256_mul_ps(_mm256_sub_ps(matRefracIndex, one8), _mm256_sub_ps(matRefracIndex, one8)), _mm256_mul_ps(_mm256_add_ps(matRefracIndex, one8), _mm256_add_ps(matRefracIndex, one8)));
	__m256 c_comp = _mm256_sub_ps(one8, dotDirNor);
	__m256 R = _mm256_add_ps(R0, _mm256_mul_ps(_mm256_sub_ps(one8, R0), _mm256_pow_ps(c_comp, _mm256_set1_ps(5))));

	// mirror ray
	AvxVector3 mirrDir = normalize(sub(rayDir, mul(hitNormal, dotDirNor * 2)));
	AvxVector3 hitPos = { hitInfo.px, hitInfo.py, hitInfo.pz };
	__m256 migraine8 = _mm256_set1_ps(RAY_MIGRAINE);
	AvxVector3 newOriginM = add(hitPos, mul(mirrDir, migraine8));
	originX[ind] = newOriginM.x;
	originY[ind] = newOriginM.y;
	originZ[ind] = newOriginM.z;
	dirX[ind] = mirrDir.x;
	dirY[ind] = mirrDir.y;
	dirZ[ind] = mirrDir.z;
	length[ind] = _mm256_set1_ps(1000);
	AvxVector3 mCol = trace(ind, depth + 1);

	// refracted ray
	AvxVector3 newOriginR = add(hitPos, mul(refractDir, migraine8));
	originX[ind] = newOriginR.x;
	originY[ind] = newOriginR.y;
	originZ[ind] = newOriginR.z;
	dirX[ind] = refractDir.x;
	dirY[ind] = refractDir.y;
	dirZ[ind] = refractDir.z;
	length[ind] = _mm256_set1_ps(1000);
	AvxVector3 rCol = trace(ind, depth + 1);

	// two rays bounced on one hit
	AvxVector3 color1 = add(mul(mul(k, R), mCol), mul(rCol, _mm256_sub_ps(one8, R)));

	__m256 colx = hitInfo.mat.ambx;
	__m256 coly = hitInfo.mat.amby;
	__m256 colz = hitInfo.mat.ambz;

	AvxVector3 diffCol = { hitInfo.mat.diffx, hitInfo.mat.diffy, hitInfo.mat.diffz };
	AvxVector3 specCol = { hitInfo.mat.specx, hitInfo.mat.specy, hitInfo.mat.specz };
	for (Light* l : lights)
	{
		AvxVector3 lPos = { _mm256_set1_ps(l->position.get_x()), _mm256_set1_ps(l->position.get_y()), _mm256_set1_ps(l->position.get_z()) };
		AvxVector3 lightDist = sub(lPos, hitPos);
		__m256 lightMask = _mm256_cmp_ps(dot_product(hitNormal, lightDist), zero8, _CMP_LT_OS);

		AvxVector3 lightV = normalize(lightDist);
		__m256 lightDistAbs = vector_length(lightDist);
		__m256 shadowLength = _mm256_sub_ps(lightDistAbs, _mm256_mul_ps(migraine8, _mm256_set1_ps(2)));

		AvxVector3 halfVector = normalize(sub(lightV, rayDir));
		AvxVector3 lightInt = { _mm256_set1_ps(l->intensity.get_x()), _mm256_set1_ps(l->intensity.get_y()), _mm256_set1_ps(l->intensity.get_z()) };
		AvxVector3 diffuse = mul(lightInt, _mm256_rcp_ps(lightDistAbs));

		AvxVector3 blinnphong = mul(mul(diffCol, lightInt), _mm256_max_ps(zero8, dot_product(hitNormal, lightV)));
		__m256 specCoef = _mm256_pow_ps(_mm256_max_ps(zero8, dot_product(hitNormal, halfVector)), _mm256_set1_ps(BLINN_PHONG_POWER));
		AvxVector3 specular = mul(mul(diffCol, specCol), mul(lightInt, specCoef));
		
		AvxVector3 lightColor = mul(diffuse, add(blinnphong, specular));
		colx = _mm256_add_ps(colx, lightColor.x);
		coly = _mm256_add_ps(coly, lightColor.y);
		colx = _mm256_add_ps(colz, lightColor.z);
	}

	AvxVector3 calcLight = { colx, coly, colz };
	AvxVector3 normalCol = mul(calcLight, diffCol);
	__m256 mirror8 = hitInfo.mat.mirror;
	__m256 mirrorMask = _mm256_cmp_ps(mirror8, zero8, _CMP_EQ_OS);
	AvxVector3 mirrCol = mul(diffCol, mCol);
	AvxVector3 finalCol = add(mul(mirrCol, mirror8), mul(normalCol, _mm256_sub_ps(one8, mirror8)));

	AvxVector3 color2 = { _mm256_blendv_ps(normalCol.x, finalCol.x, mirrorMask), _mm256_blendv_ps(normalCol.y, finalCol.y, mirrorMask), _mm256_blendv_ps(normalCol.z, finalCol.z, mirrorMask) };
	r[ind] = _mm256_blendv_ps(color1.x, color2.x, refracMask);
	g[ind] = _mm256_blendv_ps(color1.y, color2.y, refracMask);
	b[ind] = _mm256_blendv_ps(color1.y, color2.y, refracMask);

	return { r[ind], g[ind], b[ind] };

	//if (hitInfo.material.refracIndex > 1)
	//{
	//	// Init beers law
	//	Vec3Df k = Vec3Df(1);
	//	float c = 0;
	//	lastId = hitInfo.id;

	//	bool cond = dot_product(ray.direction, hitInfo.normal) < 0;
	//	Vec3Df hitNormal = hitInfo.normal * (cond ? 1 : -1);
	//	float refracIndex = cond ? hitInfo.material.refracIndex : (1 / hitInfo.material.refracIndex);
	//	// Obtuse angle
	//	float cosTheta = rayCosTheta(ray, hitNormal, refracIndex);
	//	bool refracted = cosTheta >= 0;
	//	Vec3Df refractDir = refracted ? refractRay(ray, hitNormal, refracIndex, cosTheta) : Vec3Df(0);
	//	if (!cond)
	//	{
	//		if (!refracted)
	//		{
	//			return trace(mirrorRay(ray.direction, hitInfo), depth + 1);
	//		}
	//		Vec3Df power = -hitInfo.material.absorbtion * refractDir;
	//		k = Vec3Df(pow(E, power[0]), pow(E, power[1]), pow(E, power[2]));
	//	}
	//	Ray refractedRay = { hitInfo.hitPos + refractDir * Vec3Df(RAY_MIGRAINE), refractDir, 1000 };
	//	Vec3Df rayDir = cond ? -ray.direction : ray.direction;
	//	c = dot_product(rayDir, hitInfo.normal);
	//	float R0 = ((hitInfo.material.refracIndex - 1) * (hitInfo.material.refracIndex - 1)) / ((hitInfo.material.refracIndex + 1) * (hitInfo.material.refracIndex + 1));
	//	float c_comp = 1 - c;
	//	float R = R0 + (1 - R0) * c_comp * c_comp * c_comp * c_comp * c_comp;
	//	return  k * (R * trace(mirrorRay(ray.direction, hitInfo), depth + 1)) + (1 - R) * trace(refractedRay, depth + 1);
	//}
	//else
	//{
	//	Vec3Df normalCol = calculateLight(hitInfo, ray.direction) * hitInfo.material.diffuseColor;
	//	if (hitInfo.material.mirror == 0)
	//	{
	//		return normalCol;
	//	}
	//	else
	//	{
	//		Vec3Df mirrCol = hitInfo.material.diffuseColor * trace(mirrorRay(ray.direction, hitInfo), depth + 1);
	//		Vec3Df finalCol = mirrCol * Vec3Df(hitInfo.material.mirror) + normalCol * (Vec3Df(1 - hitInfo.material.mirror));
	//		return finalCol;
	//	}
	//}
}

void RaySystem::hit(int ind)
{
	__m256 dx = dirX[ind];
	__m256 dy = dirY[ind];
	__m256 dz = dirZ[ind];
	__m256 ox = originX[ind];
	__m256 oy = originY[ind];
	__m256 oz = originZ[ind];
	__m256 len = length[ind];

	HitInfo* h = new HitInfo[AVX_SIZE];
	for (size_t j = 0; j < AVX_SIZE; j++)
	{
		Ray r = { Vec3Df(dx.m256_f32[j], dy.m256_f32[j], dz.m256_f32[j]), Vec3Df(ox.m256_f32[j], oy.m256_f32[j], oz.m256_f32[j]), len.m256_f32[j]};
		for (size_t i = 0; i < shapeSize; i++)
		{
			if (shapes[i]->hit(r, h + j)) break;
		}
	}
}