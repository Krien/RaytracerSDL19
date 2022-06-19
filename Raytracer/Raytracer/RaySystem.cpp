#include "precomp.h"

#define SIZE SCREEN_WIDTH * SCREEN_HEIGHT

RaySystem::RaySystem()
{

}

Vec3Df* RaySystem::trace()
{
	Vec3Df* traceResult = new Vec3Df[SIZE];
	return traceResult;
}
