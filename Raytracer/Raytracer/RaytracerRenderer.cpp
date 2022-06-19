#include "precomp.h"

RaytracerRenderer::RaytracerRenderer(Screen* screen) : Renderer(screen)
{
	width = screen->getWidth();
	height = screen->getHeight();

#if SIMD
	rs = new RaySystem();
#endif
}

RaytracerRenderer::~RaytracerRenderer()
{
}

void RaytracerRenderer::draw(int iteration)
{
	assert(currentScene != NULL && camera != NULL);
	shapes = currentScene->objects;
	shapeSize = shapes.size();
	lights = currentScene->lights;
	lightSize = lights.size();
	Vec3Df rayStartDir = camera->getRelTopLeft();
	float xOffset = (float)SCREEN_DIMENSION * 2 / (width - 1);
	float yOffset = (float)SCREEN_DIMENSION * 2 / (height - 1);
	lastId = -1;
#if SIMD
	assert(rs != NULL);

	float camX = camera->position.get_x();
	float camY = camera->position.get_y();
	float camZ = camera->position.get_z();
	for (unsigned int i = 0; i < height * width / 8; i++)
	{
		rs->ox8[i].m256_f32[0] = camX;
		rs->ox8[i].m256_f32[1] = camX;
		rs->ox8[i].m256_f32[2] = camX;
		rs->ox8[i].m256_f32[3] = camX;
		rs->ox8[i].m256_f32[4] = camX;
		rs->ox8[i].m256_f32[5] = camX;
		rs->ox8[i].m256_f32[6] = camX;
		rs->ox8[i].m256_f32[7] = camX;
		rs->oy8[i].m256_f32[0] = camY;
		rs->oy8[i].m256_f32[1] = camY;
		rs->oy8[i].m256_f32[2] = camY;
		rs->oy8[i].m256_f32[3] = camY;
		rs->oy8[i].m256_f32[4] = camY;
		rs->oy8[i].m256_f32[5] = camY;
		rs->oy8[i].m256_f32[6] = camY;
		rs->oy8[i].m256_f32[7] = camY;
		rs->oz8[i].m256_f32[0] = camZ;
		rs->oz8[i].m256_f32[1] = camZ;
		rs->oz8[i].m256_f32[2] = camZ;
		rs->oz8[i].m256_f32[3] = camZ;
		rs->oz8[i].m256_f32[4] = camZ;
		rs->oz8[i].m256_f32[5] = camZ;
		rs->oz8[i].m256_f32[6] = camZ;
		rs->oz8[i].m256_f32[7] = camZ;

		int x = i * 8 % width;
		int y = i * 8 / width;
		Vec3Df d0 = rayStartDir + Vec3Df(x * xOffset, y * yOffset, 0);
		Vec3Df d1 = d0 + Vec3Df(xOffset, 0, 0);
		Vec3Df d2 = d1 + Vec3Df(xOffset, 0, 0);
		Vec3Df d3 = d2 + Vec3Df(xOffset, 0, 0);
		Vec3Df d4 = d3 + Vec3Df(xOffset, 0, 0);
		Vec3Df d5 = d4 + Vec3Df(xOffset, 0, 0);
		Vec3Df d6 = d5 + Vec3Df(xOffset, 0, 0);
		Vec3Df d7 = d6 + Vec3Df(xOffset, 0, 0);
		d0 = normalize_vector(d0);
		d1 = normalize_vector(d1);
		d2 = normalize_vector(d2);
		d3 = normalize_vector(d3);
		d4 = normalize_vector(d4);
		d5 = normalize_vector(d5);
		d6 = normalize_vector(d6);
		d7 = normalize_vector(d7);
		rs->dx8[i].m256_f32[0] = d0.get_x();
		rs->dx8[i].m256_f32[1] = d1.get_x();
		rs->dx8[i].m256_f32[2] = d2.get_x();
		rs->dx8[i].m256_f32[3] = d3.get_x();
		rs->dx8[i].m256_f32[4] = d4.get_x();
		rs->dx8[i].m256_f32[5] = d5.get_x();
		rs->dx8[i].m256_f32[6] = d6.get_x();
		rs->dx8[i].m256_f32[7] = d7.get_x();
		rs->dy8[i].m256_f32[0] = d0.get_y();
		rs->dy8[i].m256_f32[1] = d1.get_y();
		rs->dy8[i].m256_f32[2] = d2.get_y();
		rs->dy8[i].m256_f32[3] = d3.get_y();
		rs->dy8[i].m256_f32[4] = d4.get_y();
		rs->dy8[i].m256_f32[5] = d5.get_y();
		rs->dy8[i].m256_f32[6] = d6.get_y();
		rs->dy8[i].m256_f32[7] = d7.get_y();
		rs->dz8[i].m256_f32[0] = d0.get_z();
		rs->dz8[i].m256_f32[1] = d1.get_z();
		rs->dz8[i].m256_f32[2] = d2.get_z();
		rs->dz8[i].m256_f32[3] = d3.get_z();
		rs->dz8[i].m256_f32[4] = d4.get_z();
		rs->dz8[i].m256_f32[5] = d5.get_z();
		rs->dz8[i].m256_f32[6] = d6.get_z();
		rs->dz8[i].m256_f32[7] = d7.get_z();

		rs->len8[i].m256_f32[0] = 100;
		rs->len8[i].m256_f32[1] = 100;
		rs->len8[i].m256_f32[2] = 100;
		rs->len8[i].m256_f32[3] = 100;
		rs->len8[i].m256_f32[4] = 100;
		rs->len8[i].m256_f32[5] = 100;
		rs->len8[i].m256_f32[6] = 100;
		rs->len8[i].m256_f32[7] = 100;
	}

	Vec3Df* traceResult = rs->trace();
	for (unsigned int y = 0; y < height; y++)
	{
		for (unsigned int x = 0; x < width; x++)
		{
			Vec3Df argb = traceResult[y * width + x] * Vec3Df(255);

			// Convert color
			unsigned int xy = x * 4 + (y * width) * 4;
			*(pixelBuffer + xy) = std::min((int)argb.extract(0), 255);
			*(pixelBuffer + xy + 1) = std::min((int)argb.extract(1), 255);
			*(pixelBuffer + xy + 2) = std::min((int)argb.extract(2), 255);
			*(pixelBuffer + xy + 3) = 0;
		}
	}
#else
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
	}
#endif
}

Vec3Df RaytracerRenderer::trace(Ray ray, int depth)
{
	if (depth > RAYTRACER_RECURSION_DEPTH)
		return Vec3Df(0);
	HitInfo hitInfo = { Vec3Df(0),Vec3Df(0),1000 };
	for (size_t i = 0; i < shapeSize; i++)
	{
		shapes[i]->hit(ray, &hitInfo);
	}
	// Not a hit, so return
	if (hitInfo.distance > RAYTRACER_MAX_RENDERDISTANCE)
	{
		return Vec3Df(0);
	}
	if (hitInfo.material.mirror == 0 && hitInfo.material.refracIndex <= 1)
	{
		return calculateLight(hitInfo, ray.direction)*hitInfo.material.diffuseColor;
	}
	else if(hitInfo.material.refracIndex <= 1)
	{
		Vec3Df normalCol = calculateLight(hitInfo, ray.direction) * hitInfo.material.diffuseColor;
		Vec3Df mirrCol = hitInfo.material.diffuseColor*trace(mirrorRay(ray.direction, hitInfo), depth + 1);
		Vec3Df finalCol = mirrCol * Vec3Df(hitInfo.material.mirror) + normalCol * (Vec3Df(1 - hitInfo.material.mirror));
		return finalCol;
	}
	else
	{
		// Init beers law
		Vec3Df k = Vec3Df(1);
		float c = 0;
		lastId = hitInfo.id;

		// Obtuse angle
		if (dot_product(ray.direction, hitInfo.normal) < 0)
		{
			float cosTheta = rayCosTheta(ray, hitInfo.normal, hitInfo.material.refracIndex);
			bool refracted = cosTheta >= 0;
			Vec3Df refractDir =  refracted ? refractRay(ray, hitInfo.normal, hitInfo.material.refracIndex, cosTheta) : Vec3Df(0);
			Ray refractedRay = { hitInfo.hitPos + refractDir * Vec3Df(RAY_MIGRAINE), refractDir, 1000 };
			c = dot_product(-ray.direction, hitInfo.normal);
			// Calculate the reflectance at normal incidence.
			float R0 = ((hitInfo.material.refracIndex - 1) * (hitInfo.material.refracIndex - 1)) / ((hitInfo.material.refracIndex + 1) * (hitInfo.material.refracIndex + 1));
			// Calculate schlick's approximation (amount of % the refraction contributes to the total).
			float R = R0 + (1 - R0) * pow((1 - c), 5);
			// The ray has now hit something refractive, if two spheres border each other we want them to refract off each other's indices and not the indice of air, and thus we set the last hit refract indice for the ray.
			if (lastId == hitInfo.id)
			{
				ray.refracIndex = 1.000293f;
				ray.refracIndex2 = 1.000586f;
			}
			else
			{
				ray.refracIndex = hitInfo.material.refracIndex;
				ray.refracIndex2 = hitInfo.material.refracIndex * hitInfo.material.refracIndex;
			}
			// Return percentage of mirror and refraction that results from the above calculations, where R is the amount reflected and 1-R the amount refracted.
			return  k * (R * trace(mirrorRay(ray.direction, hitInfo), depth + 1)) + (1 - R) * trace(refractedRay, depth + 1);
		}
		else
		{
			float cosTheta = rayCosTheta(ray, -hitInfo.normal, 1/hitInfo.material.refracIndex);
			bool refracted = cosTheta >= 0;
			Vec3Df refractDir = refracted ? refractRay(ray, -hitInfo.normal, 1/hitInfo.material.refracIndex, cosTheta) : Vec3Df(0);
			Vec3Df power = -hitInfo.material.absorbtion * refractDir;
			k = Vec3Df(pow(E, power[0]), pow(E, power[1]), pow(E, power[2]));
			if (!refracted)
			{
				return k * trace(mirrorRay(ray.direction, hitInfo), depth + 1);
			}
			Ray refractedRay = { hitInfo.hitPos + refractDir * Vec3Df(RAY_MIGRAINE), refractDir, 1000 };
			c = dot_product(ray.direction, hitInfo.normal);
			// Calculate the reflectance at normal incidence.
			float R0 = ((hitInfo.material.refracIndex - 1) * (hitInfo.material.refracIndex - 1)) / ((hitInfo.material.refracIndex + 1) * (hitInfo.material.refracIndex + 1));
			// Calculate schlick's approximation (amount of % the refraction contributes to the total).
			float R = R0 + (1 - R0) * pow((1 - c), 5);
			// The ray has now hit something refractive, if two spheres border each other we want them to refract off each other's indices and not the indice of air, and thus we set the last hit refract indice for the ray.
			if (lastId == hitInfo.id)
			{
				ray.refracIndex = 1.000293f;
				ray.refracIndex2 = 1.000586f;
			}
			else
			{
				ray.refracIndex = hitInfo.material.refracIndex;
				ray.refracIndex2 = hitInfo.material.refracIndex* hitInfo.material.refracIndex;
			}
			// Return percentage of mirror and refraction that results from the above calculations, where R is the amount reflected and 1-R the amount refracted.
			return  k * (R * trace(mirrorRay(ray.direction, hitInfo), depth + 1)) + (1 - R) * trace(refractedRay, depth + 1);
		}
	}
}

Vec3Df RaytracerRenderer::calculateLight(HitInfo hitI, Vec3Df direction)
{
	Vec3Df argb = hitI.material.ambientColor;
	for (Light* l : lights)
	{
		Vec3Df lightDist = l->position - hitI.hitPos;
		Vec3Df lightV = normalize_vector(lightDist);
		//If we cannot reach this light from the intersection point we go on to the next light.
		if (dot_product(hitI.normal, lightDist) < 0)
			continue;
		// blocked
		Vec3Df shadowOrigin = hitI.hitPos + lightV * Vec3Df(RAY_MIGRAINE);
		float shadowLength = (float)(vector_length(lightDist) - 2 * RAY_MIGRAINE);
		Ray shadowR = { shadowOrigin, lightV, shadowLength };
		for (size_t i = 0; i < shapeSize; i++)
		{
			if (shapes[i]->fastHit(shadowR))
				continue;
		}
		// Diffuse
		Vec3Df halfVector = normalize_vector(lightV - direction);
		Vec3Df diffuse = l->intensity / vector_length(lightDist);
		// Blinn phong
		Vec3Df blinnphong = hitI.material.diffuseColor * l->intensity * Vec3Df(std::max(0.0f, dot_product(hitI.normal, lightV)));
		Vec3Df specular = hitI.material.diffuseColor * hitI.material.specularColor * l->intensity;
		specular *= Vec3Df( (float)pow(std::max(0.0f, dot_product(hitI.normal, halfVector)), BLINN_PHONG_POWER) );
		blinnphong += specular;
		Vec3Df lightColor = diffuse * blinnphong;
		argb += lightColor;
	}
	return argb;
}

Ray RaytracerRenderer::mirrorRay(Vec3Df originalDir, HitInfo hitSurface)
{
	Vec3Df mirrDir = originalDir - (hitSurface.normal * Vec3Df((2.0f * dot_product(originalDir, hitSurface.normal))));
	mirrDir = normalize_vector(mirrDir);
	Ray mirrorRay = Ray{ hitSurface.hitPos + Vec3Df(RAY_MIGRAINE) * mirrDir, mirrDir, 1000 };
	return mirrorRay;
}

float RaytracerRenderer::rayCosTheta(Ray r, Vec3Df normal, float rIndex)
{
	float dotDirNor = dot_product(r.direction, normal);
	float cosVal = (1.0f - (r.refracIndex2 * (1.0f - (dotDirNor* dotDirNor))) / (rIndex * rIndex));
	return cosVal;

}

Vec3Df RaytracerRenderer::refractRay(Ray r, Vec3Df normal, float rIndex, float rayCosTheta)
{
	// Calculate the sine of the phi angle of the transmission vector t.
	Vec3Df sinPhi = (r.refracIndex * (r.direction - normal * dot_product(r.direction, normal))) / rIndex;
	// Calculate the finale refraction direction
	Vec3Df refractDir = normalize_vector(sinPhi - normal * Vec3Df(sqrtf(rayCosTheta)));
	return refractDir;
}
