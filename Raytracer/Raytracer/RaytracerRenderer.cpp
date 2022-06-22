#include "precomp.h"

RaytracerRenderer::RaytracerRenderer(Screen* screen) : Renderer(screen)
{
	width = screen->getWidth();
	height = screen->getHeight(); 
#if SIMD
	rs = new RaySystem(screen); 
#endif
}

RaytracerRenderer::~RaytracerRenderer()
{
}

void RaytracerRenderer::draw(int iteration)
{
	assert(currentScene != NULL && camera != NULL);

	// ok this is kinda scuffed lmao wtf? why are we assigning each iter?
	shapes = currentScene->objects;
	shapeSize = shapes.size();
	lights = currentScene->lights;
	lightSize = lights.size();
	lastId = -1;
#if 1
	rs->init(currentScene, camera);
	rs->draw(pixelBuffer);
#else
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
		bool earlyOut = shapes[i]->hit(ray, &hitInfo);
		if (earlyOut)
			break;
	}
	// Not a hit, so return
	if (hitInfo.distance > RAYTRACER_MAX_RENDERDISTANCE)
	{
		return Vec3Df(0);
	}

	if (hitInfo.material.refracIndex > 1)
	{
		// Init beers law
		Vec3Df k = Vec3Df(1);
		float c = 0;
		lastId = hitInfo.id;

		bool cond = dot_product(ray.direction, hitInfo.normal) < 0;
		Vec3Df hitNormal = hitInfo.normal * (cond ? 1 : -1);
		float refracIndex = cond ? hitInfo.material.refracIndex : (1 / hitInfo.material.refracIndex);
		// Obtuse angle
		float cosTheta = rayCosTheta(ray, hitNormal, refracIndex);
		bool refracted = cosTheta >= 0;
		Vec3Df refractDir = refracted ? refractRay(ray, hitNormal, refracIndex, cosTheta) : Vec3Df(0);
		if (!cond)
		{
			if (!refracted)
			{
				return trace(mirrorRay(ray.direction, hitInfo), depth + 1);
			}
			Vec3Df power = -hitInfo.material.absorbtion * refractDir;
			k = Vec3Df(pow(E, power[0]), pow(E, power[1]), pow(E, power[2]));
		}
		Ray refractedRay = { hitInfo.hitPos + refractDir * Vec3Df(RAY_MIGRAINE), refractDir, 1000 };
		Vec3Df rayDir = cond ? -ray.direction : ray.direction;
		c = dot_product(rayDir, hitInfo.normal);
		float R0 = ((hitInfo.material.refracIndex - 1) * (hitInfo.material.refracIndex - 1)) / ((hitInfo.material.refracIndex + 1) * (hitInfo.material.refracIndex + 1));
		float c_comp = 1 - c;
		float R = R0 + (1 - R0) * c_comp * c_comp * c_comp * c_comp * c_comp;
		return  k * (R * trace(mirrorRay(ray.direction, hitInfo), depth + 1)) + (1 - R) * trace(refractedRay, depth + 1);
	}
	else
	{
		Vec3Df normalCol = calculateLight(hitInfo, ray.direction) * hitInfo.material.diffuseColor;
		if (hitInfo.material.mirror == 0)
		{
			return normalCol;
		}
		else
		{
			Vec3Df mirrCol = hitInfo.material.diffuseColor * trace(mirrorRay(ray.direction, hitInfo), depth + 1);
			Vec3Df finalCol = mirrCol * Vec3Df(hitInfo.material.mirror) + normalCol * (Vec3Df(1 - hitInfo.material.mirror));
			return finalCol;
		}
	}
}

Vec3Df RaytracerRenderer::calculateLight(HitInfo hitI, Vec3Df direction)
{
	Vec3Df argb = hitI.material.ambientColor;
	for (Light* l : lights)
	{
		Vec3Df lightDist = l->position - hitI.hitPos;
		//If we cannot reach this light from the intersection point we go on to the next light.
		if (dot_product(hitI.normal, lightDist) < 0)
			continue;
		Vec3Df lightV = normalize_vector(lightDist);
		// blocked
		float shadowLength = (float)(vector_length(lightDist) - 2 * RAY_MIGRAINE);
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
