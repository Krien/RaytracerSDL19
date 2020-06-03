#include "precomp.h"

Light::Light(Vec3Df position, Vec3Df col, Vec3Df intensity)
{
	Light::position = position;
	Light::col = col;
	Light::intensity = intensity;
}
