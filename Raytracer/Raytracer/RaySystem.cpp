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
}

void RaySystem::draw(Pixel* pixelBuffer) {
	Vec3Df startDir = camera->getRelTopLeft();
	Vec3Df camPos = camera->position;
	const static float xOffset = (float)SCREEN_DIMENSION * 2 / (width - 1);
	const static float yOffset = (float)SCREEN_DIMENSION * 2 / (height - 1);

	const __m256 startX = _mm256_set1_ps(startDir.get_x());
	const __m256 startY = _mm256_set1_ps(startDir.get_y());
	const __m256 startZ = _mm256_set1_ps(startDir.get_z());

	const __m256 ox = _mm256_set1_ps(camPos.get_x());
	const __m256 oy = _mm256_set1_ps(camPos.get_y());
	const __m256 oz = _mm256_set1_ps(camPos.get_z());

	const __m256 rayLen = _mm256_set1_ps(100);

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
void RaySystem::trace(int ind, int depth)
{
	__m256 dx = dirX[ind];
	__m256 dy = dirY[ind];
	__m256 dz = dirZ[ind];
	__m256 ox = originX[ind];
	__m256 oy = originY[ind];
	__m256 oz = originZ[ind];
	__m256 len = length[ind];
	//if (depth > RAYTRACER_RECURSION_DEPTH)
	//	return Vec3Df(0);
	//HitInfo hitInfo = { Vec3Df(0),Vec3Df(0),1000 };
	//for (size_t i = 0; i < shapeSize; i++)
	//{
	//	shapes[i]->hit(ray, &hitInfo);
	//}
	//// Not a hit, so return
	//if (hitInfo.distance > RAYTRACER_MAX_RENDERDISTANCE)
	//{
	//	return Vec3Df(0);
	//}

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

HitInfo* RaySystem::hit()
{
	return NULL;
}
