#include "precomp.h"

Shape::Shape(int id, Vec3Df position, Material material)
{
	Shape::id = id;
	Shape::position = position;
	Shape::material = material;
}

Shape::~Shape()
{
}

void Shape::hit(Ray r, HitInfo* hit)
{
}

bool Shape::fastHit(Ray r)
{
	return false;
}
