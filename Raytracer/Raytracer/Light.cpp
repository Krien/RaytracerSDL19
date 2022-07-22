#include "precomp.h"

Light::Light(Vec3Df position, Vec3Df intensity)
{
	Light::position = position;
	Light::intensity = intensity;
	Light::posX = _mm256_set1_ps(position.get_x());
	Light::posY = _mm256_set1_ps(position.get_y());
	Light::posZ = _mm256_set1_ps(position.get_z());
	Light::intensity8 = _mm256_set1_ps(intensity.get_x());
}
