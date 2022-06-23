#include "precomp.h"

Vec3Df whiteAmbient = Vec3Df(0.1f);
Vec3Df whiteDiffuse = Vec3Df(1);
Vec3Df whiteSpecular = Vec3Df(1);
Vec3Df blueDiffuse = Vec3Df(1, 0, 0);
Vec3Df greenDiffuse = Vec3Df(0, 1, 0);
Vec3Df redDiffuse = Vec3Df(0, 0, 1);
Vec3Df orangeDiffuse = Vec3Df(0, 0.5f, 1);
Vec3Df pinkDiffuse = Vec3Df(1, 0, 1);
Vec3Df refractNorm = Vec3Df(1);

const Material matLUT[5] = {
	{ whiteAmbient, whiteDiffuse, whiteSpecular, refractNorm, 0, 3.000293f }, // blue refract
	{ whiteAmbient, redDiffuse, whiteSpecular, refractNorm, 0, 0 }, // solid red
	{ whiteAmbient, orangeDiffuse, whiteSpecular, refractNorm, 0, 0 }, // solid green
	{ whiteAmbient, blueDiffuse, whiteSpecular, refractNorm, 0, 0 }, // solid blue
	{ whiteAmbient, pinkDiffuse, whiteSpecular * 0.5f, refractNorm, 0, 0 }, // pinkwall
};

Shape::Shape(int id, int mat_id)
{
	Shape::id = id;
	Shape::mat = matLUT[mat_id];
}

Shape::~Shape()
{
}

bool Shape::hit(Ray r, HitInfo* hit)
{
	return false;
}

bool Shape::fastHit(Ray r)
{
	return false;
}

Material Shape::getMat(int mid)
{
	return matLUT[mid];
}
