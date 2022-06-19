#include "precomp.h"

Scene::Scene()
{
	Sphere* test1 = new Sphere(0, SOLID_RED, Vec3Df(-1.0f, -0.9f, 3), 0.5f);
	objects.push_back(test1);
	Sphere* test2 = new Sphere(1, SOLID_RED, Vec3Df(0, -0.9f, 3), 0.5f);
	objects.push_back(test2);
	Sphere* test3 = new Sphere(2, BLUE_MIRROR, Vec3Df(1, -0.9f, 3), 0.5f);
	objects.push_back(test3);

	Plane* pDown = new Plane(3, SOLID_RED, Vec3Df(0, 1, 0), -0.1f);
	Plane* pUp = new Plane(4, SOLID_RED, Vec3Df(0, 1, 0), -5);
	Plane* pRight = new Plane(5, SOLID_RED, Vec3Df(1, 0, 0), -2);
	Plane* pLeft = new Plane(6, SOLID_RED, Vec3Df(-1, 0, 0), -5);
	Plane* pFront = new Plane(7, SOLID_RED, Vec3Df(0, 0, 1), -5);
	Plane* pBack = new Plane(8, SOLID_RED, Vec3Df(0, 0, -1), -5);
	objects.push_back(pDown);
	objects.push_back(pUp);
	objects.push_back(pRight);
	objects.push_back(pLeft);
	objects.push_back(pFront);
	objects.push_back(pBack);
	Light*l1 = new Light(Vec3Df(0.5f, -4, 0.3f), Vec3Df(1), Vec3Df(1));
	lights.push_back(l1);
}

Scene::Scene(std::vector<Shape*> objects)
{
	Scene::objects = objects;
}

Scene::~Scene()
{
}
