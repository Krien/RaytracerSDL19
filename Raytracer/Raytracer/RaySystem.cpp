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
	startZ = _mm256_set1_ps(startDir.get_z());

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
			_mm256_setr_ps(x * xOffset, (x + 1) * xOffset, (x + 2) * xOffset, (x + 3) * xOffset,
				(x + 4) * xOffset, (x + 5) * xOffset, (x + 6) * xOffset, (x + 7) * xOffset));
		__m256 dy = _mm256_add_ps(startY,
			_mm256_set1_ps(y * yOffset));
		
		AvxVector3 norm = normalize(dx, dy, startZ); 

		dirX[i] = norm.x;
		dirY[i] = norm.y;
		dirZ[i] = norm.z;
		originX[i] = ox;
		originY[i] = oy;
		originZ[i] = oz;
		length[i] = rayLen; 
	}
	for (unsigned int j = 0; j < (height * width) / AVX_SIZE; j++)
		trace(j, 0);

	for (unsigned int j = 0; j < (height * width) / AVX_SIZE; j++)
	{ 
		__m256 maxColor8 = _mm256_set1_ps(255.0f);
		r[j] = _mm256_min_ps(_mm256_mul_ps(r[j], maxColor8), maxColor8);
		g[j] = _mm256_min_ps(_mm256_mul_ps(g[j], maxColor8), maxColor8);
		b[j] = _mm256_min_ps(_mm256_mul_ps(b[j], maxColor8), maxColor8);
		
		// Please check later if this is correct lol
		int x = j * AVX_SIZE % width;
		float y = j * AVX_SIZE / width;
		
		for (unsigned int c = 0; c < 8; c++) {
			unsigned int startIndex = x * 4 + (y * width) * 4;
			pixelBuffer[startIndex] = r[j].m256_f32[c];
			pixelBuffer[startIndex + 1] = g[j].m256_f32[c];
			pixelBuffer[startIndex + 2] = b[j].m256_f32[c];
			pixelBuffer[startIndex + 3] = 0;
			x += 1;
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


	__m256 dx = dirX[ind];
	__m256 dy = dirY[ind];
	__m256 dz = dirZ[ind];
	__m256 ox = originX[ind];
	__m256 oy = originY[ind];
	__m256 oz = originZ[ind];
	__m256 len = length[ind];

	// -- hit function  currently not fully AVX --

	HitInfo h[AVX_SIZE];
	for (unsigned int j = 0; j < AVX_SIZE; j++)
	{
		h[j] = { Vec3Df(0),Vec3Df(0),1000 };
		Ray r = { Vec3Df(ox.m256_f32[j], oy.m256_f32[j], oz.m256_f32[j]), Vec3Df(dx.m256_f32[j], dy.m256_f32[j], dz.m256_f32[j]), len.m256_f32[j] };
		for (unsigned int i = 0; i < shapeSize; i++)
		{
			bool earlyOut = shapes[i]->hit(r, &h[j]); 
			if (earlyOut) {
				break;
			}
		}
	}

	HitInfo_Avx hitInfo = HitInfo_Avx();
	hitInfo.nx = _mm256_setr_ps(h[0].normal.get_x(), h[1].normal.get_x(), h[2].normal.get_x(), h[3].normal.get_x(), h[4].normal.get_x(), h[5].normal.get_x(), h[6].normal.get_x(), h[7].normal.get_x());
	hitInfo.ny = _mm256_setr_ps(h[0].normal.get_y(), h[1].normal.get_y(), h[2].normal.get_y(), h[3].normal.get_y(), h[4].normal.get_y(), h[5].normal.get_y(), h[6].normal.get_y(), h[7].normal.get_y());
	hitInfo.nz = _mm256_setr_ps(h[0].normal.get_z(), h[1].normal.get_z(), h[2].normal.get_z(), h[3].normal.get_z(), h[4].normal.get_z(), h[5].normal.get_z(), h[6].normal.get_z(), h[7].normal.get_z());
	hitInfo.px = _mm256_setr_ps(h[0].hitPos.get_x(), h[1].hitPos.get_x(), h[2].hitPos.get_x(), h[3].hitPos.get_x(), h[4].hitPos.get_x(), h[5].hitPos.get_x(), h[6].hitPos.get_x(), h[7].hitPos.get_x());
	hitInfo.py = _mm256_setr_ps(h[0].hitPos.get_y(), h[1].hitPos.get_y(), h[2].hitPos.get_y(), h[3].hitPos.get_y(), h[4].hitPos.get_y(), h[5].hitPos.get_y(), h[6].hitPos.get_y(), h[7].hitPos.get_y());
	hitInfo.pz = _mm256_setr_ps(h[0].hitPos.get_z(), h[1].hitPos.get_z(), h[2].hitPos.get_z(), h[3].hitPos.get_z(), h[4].hitPos.get_z(), h[5].hitPos.get_z(), h[6].hitPos.get_z(), h[7].hitPos.get_z());
	hitInfo.dist = _mm256_setr_ps(h[0].distance, h[1].distance, h[2].distance, h[3].distance, h[4].distance, h[5].distance, h[6].distance, h[7].distance);
	hitInfo.id = _mm256_setr_epi32(h[0].id, h[1].id, h[2].id, h[3].id, h[4].id, h[5].id, h[6].id, h[7].id);
	hitInfo.mat.absx = _mm256_setr_ps(h[0].material.absorbtion.get_x(), h[1].material.absorbtion.get_x(), h[2].material.absorbtion.get_x(), h[3].material.absorbtion.get_x(), h[4].material.absorbtion.get_x(), h[5].material.absorbtion.get_x(), h[6].material.absorbtion.get_x(), h[7].material.absorbtion.get_x());
	hitInfo.mat.absy = _mm256_setr_ps(h[0].material.absorbtion.get_y(), h[1].material.absorbtion.get_y(), h[2].material.absorbtion.get_y(), h[3].material.absorbtion.get_y(), h[4].material.absorbtion.get_y(), h[5].material.absorbtion.get_y(), h[6].material.absorbtion.get_y(), h[7].material.absorbtion.get_y());
	hitInfo.mat.absz = _mm256_setr_ps(h[0].material.absorbtion.get_z(), h[1].material.absorbtion.get_z(), h[2].material.absorbtion.get_z(), h[3].material.absorbtion.get_z(), h[4].material.absorbtion.get_z(), h[5].material.absorbtion.get_z(), h[6].material.absorbtion.get_z(), h[7].material.absorbtion.get_z());
	hitInfo.mat.diffx = _mm256_setr_ps(h[0].material.diffuseColor.get_x(), h[1].material.diffuseColor.get_x(), h[2].material.diffuseColor.get_x(), h[3].material.diffuseColor.get_x(), h[4].material.diffuseColor.get_x(), h[5].material.diffuseColor.get_x(), h[6].material.diffuseColor.get_x(), h[7].material.diffuseColor.get_x());
	hitInfo.mat.diffy = _mm256_setr_ps(h[0].material.diffuseColor.get_y(), h[1].material.diffuseColor.get_y(), h[2].material.diffuseColor.get_y(), h[3].material.diffuseColor.get_y(), h[4].material.diffuseColor.get_y(), h[5].material.diffuseColor.get_y(), h[6].material.diffuseColor.get_y(), h[7].material.diffuseColor.get_y());
	hitInfo.mat.diffz = _mm256_setr_ps(h[0].material.diffuseColor.get_z(), h[1].material.diffuseColor.get_z(), h[2].material.diffuseColor.get_z(), h[3].material.diffuseColor.get_z(), h[4].material.diffuseColor.get_z(), h[5].material.diffuseColor.get_z(), h[6].material.diffuseColor.get_z(), h[7].material.diffuseColor.get_z());
	hitInfo.mat.specx = _mm256_setr_ps(h[0].material.specularColor.get_x(), h[1].material.specularColor.get_x(), h[2].material.specularColor.get_x(), h[3].material.specularColor.get_x(), h[4].material.specularColor.get_x(), h[5].material.specularColor.get_x(), h[6].material.specularColor.get_x(), h[7].material.specularColor.get_x());
	hitInfo.mat.specy = _mm256_setr_ps(h[0].material.specularColor.get_y(), h[1].material.specularColor.get_y(), h[2].material.specularColor.get_y(), h[3].material.specularColor.get_y(), h[4].material.specularColor.get_y(), h[5].material.specularColor.get_y(), h[6].material.specularColor.get_y(), h[7].material.specularColor.get_y());
	hitInfo.mat.specz = _mm256_setr_ps(h[0].material.specularColor.get_z(), h[1].material.specularColor.get_z(), h[2].material.specularColor.get_z(), h[3].material.specularColor.get_z(), h[4].material.specularColor.get_z(), h[5].material.specularColor.get_z(), h[6].material.specularColor.get_z(), h[7].material.specularColor.get_z());
	hitInfo.mat.ambx = _mm256_setr_ps(h[0].material.ambientColor.get_x(), h[1].material.ambientColor.get_x(), h[2].material.ambientColor.get_x(), h[3].material.ambientColor.get_x(), h[4].material.ambientColor.get_x(), h[5].material.ambientColor.get_x(), h[6].material.ambientColor.get_x(), h[7].material.ambientColor.get_x());
	hitInfo.mat.amby = _mm256_setr_ps(h[0].material.ambientColor.get_y(), h[1].material.ambientColor.get_y(), h[2].material.ambientColor.get_y(), h[3].material.ambientColor.get_y(), h[4].material.ambientColor.get_y(), h[5].material.ambientColor.get_y(), h[6].material.ambientColor.get_y(), h[7].material.ambientColor.get_y());
	hitInfo.mat.ambz = _mm256_setr_ps(h[0].material.ambientColor.get_z(), h[1].material.ambientColor.get_z(), h[2].material.ambientColor.get_z(), h[3].material.ambientColor.get_z(), h[4].material.ambientColor.get_z(), h[5].material.ambientColor.get_z(), h[6].material.ambientColor.get_z(), h[7].material.ambientColor.get_z());
	hitInfo.mat.mirror = _mm256_setr_ps(h[0].material.mirror, h[1].material.mirror, h[2].material.mirror, h[3].material.mirror, h[4].material.mirror, h[5].material.mirror, h[6].material.mirror, h[7].material.mirror);
	hitInfo.mat.refracIndex = _mm256_setr_ps(h[0].material.refracIndex, h[1].material.refracIndex, h[2].material.refracIndex, h[3].material.refracIndex, h[4].material.refracIndex, h[5].material.refracIndex, h[6].material.refracIndex, h[7].material.refracIndex);
	
	// end of hit function

	// distance mask
	__m256 distMask = _mm256_cmp_ps(hitInfo.dist, _mm256_set1_ps(RAYTRACER_MAX_RENDERDISTANCE), _CMP_GT_OS);

	__m256 one8 = _mm256_set1_ps(1);
	__m256 matRefracIndex = hitInfo.mat.refracIndex;
	// refraction mask
	__m256 refracMask = _mm256_cmp_ps(matRefracIndex, one8, _CMP_GT_OS);


	AvxVector3 rayDir = { dx, dy, dz };
	AvxVector3 hitNormal = { hitInfo.nx, hitInfo.ny, hitInfo.nz };
	__m256 migraine8 = _mm256_set1_ps(RAY_MIGRAINE); 
	
	// CALC LIGHT FUNCTION 
	__m256 calcLightR = hitInfo.mat.ambx;
	__m256 calcLightG = hitInfo.mat.amby;
	__m256 calcLightB = hitInfo.mat.ambz;
	for (Light* l : lights)
	{ 
		// assuming all light intensities are the same here, since that makes it a bit easier 
		const __m256 lightInt = _mm256_set1_ps(l->intensity.get_x()); 
		
		// Vec3Df lightDist = l->position - hitI.hitPos;
		const __m256 lightDistx = _mm256_sub_ps(_mm256_set1_ps(l->position.get_x()), hitInfo.px);
		const __m256 lightDisty = _mm256_sub_ps(_mm256_set1_ps(l->position.get_y()), hitInfo.py);
		const __m256 lightDistz = _mm256_sub_ps(_mm256_set1_ps(l->position.get_z()), hitInfo.pz);
		
		//if (dot_product(hitI.normal, lightDist) < 0)
		//	continue;
		__m256 lightMask = _mm256_cmp_ps(dot_product(hitInfo.nx, hitInfo.ny, hitInfo.nz, lightDistx, lightDisty ,lightDistz), zero8, _CMP_LT_OS);

		// 	Vec3Df lightV = normalize_vector(lightDist);
		const AvxVector3 lightV = normalize(lightDistx, lightDisty, lightDistz);
		const __m256 lenLight = vector_length(lightDistx, lightDisty, lightDistz);
		//	float shadowLength = (float)(vector_length(lightDist) - 2 * RAY_MIGRAINE);
		__m256 shadowLength = _mm256_sub_ps(lenLight, _mm256_mul_ps(migraine8, _mm256_set1_ps(2)));
		// Vec3Df halfVector = normalize_vector(lightV - direction);
		const AvxVector3 halfVector = normalize(_mm256_sub_ps(lightV.x, dx), _mm256_sub_ps(lightV.y, dy), _mm256_sub_ps(lightV.z, dz));
		
		__m256 diffuse = _mm256_div_ps(lightInt, lenLight);
		
		//	Vec3Df blinnphong = hitI.material.diffuseColor * l->intensity * Vec3Df(std::max(0.0f, dot_product(hitI.normal, lightV)));
		__m256 blinnPhongRightSide = _mm256_max_ps(_mm256_setzero_ps(), dot_product(hitInfo.nx, hitInfo.ny, hitInfo.nz, lightV.x, lightV.y, lightV.z));
		__m256 blinnX = _mm256_mul_ps(_mm256_mul_ps(hitInfo.mat.diffx, lightInt), blinnPhongRightSide);
		__m256 blinnY = _mm256_mul_ps(_mm256_mul_ps(hitInfo.mat.diffy, lightInt), blinnPhongRightSide);
		__m256 blinnZ = _mm256_mul_ps(_mm256_mul_ps(hitInfo.mat.diffz, lightInt), blinnPhongRightSide);
		
		//	Vec3Df specular = hitI.material.diffuseColor * hitI.material.specularColor * l->intensity;
	    //	specular *= Vec3Df((float)pow(std::max(0.0f, dot_product(hitI.normal, halfVector)), BLINN_PHONG_POWER));
		__m256 specCoeff = _mm256_pow_ps(_mm256_max_ps(zero8, dot_product(hitNormal, halfVector)), _mm256_set1_ps(BLINN_PHONG_POWER));
		__m256 specularX = _mm256_mul_ps(_mm256_mul_ps(_mm256_mul_ps(hitInfo.mat.specx, hitInfo.mat.diffx), lightInt), specCoeff);
		__m256 specularY = _mm256_mul_ps(_mm256_mul_ps(_mm256_mul_ps(hitInfo.mat.specy, hitInfo.mat.diffy), lightInt), specCoeff);
		__m256 specularZ = _mm256_mul_ps(_mm256_mul_ps(_mm256_mul_ps(hitInfo.mat.specz, hitInfo.mat.diffz), lightInt), specCoeff);
		//	blinnphong += specular;
		blinnX = _mm256_add_ps(blinnX, specularX);
		blinnY = _mm256_add_ps(blinnY, specularY);
		blinnZ = _mm256_add_ps(blinnZ, specularZ);

		//	Vec3Df lightColor = diffuse * blinnphong;
	    //	argb += lightColor;
		calcLightR = _mm256_add_ps(calcLightR, _mm256_blendv_ps(_mm256_mul_ps(blinnX, diffuse), zero8, lightMask));
		calcLightG = _mm256_add_ps(calcLightG, _mm256_blendv_ps(_mm256_mul_ps(blinnY, diffuse), zero8, lightMask));
		calcLightB = _mm256_add_ps(calcLightB, _mm256_blendv_ps(_mm256_mul_ps(blinnZ, diffuse), zero8, lightMask));
		
		
	} 
	r[ind] = _mm256_blendv_ps(_mm256_mul_ps(calcLightR, hitInfo.mat.diffx), zero8, distMask); 
	g[ind] = _mm256_blendv_ps(_mm256_mul_ps(calcLightG, hitInfo.mat.diffy), zero8, distMask);
	b[ind] = _mm256_blendv_ps(_mm256_mul_ps(calcLightB, hitInfo.mat.diffz), zero8, distMask);
	 
	return { r[ind], g[ind], b[ind] }; 
} 