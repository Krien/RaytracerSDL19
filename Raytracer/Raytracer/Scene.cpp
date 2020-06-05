#include "precomp.h"

Scene::Scene()
{
	Vec3Df whiteAmbient = Vec3Df(0.1f);
	Vec3Df whiteDiffuse = Vec3Df(1);
	Vec3Df whiteSpecular = Vec3Df(1);
	Vec3Df blueDiffuse = Vec3Df(1,0,0);
	Vec3Df redDiffuse = Vec3Df(0,0,1);
	Vec3Df refractNorm = Vec3Df(1);
	Material blueMirror = { whiteAmbient,whiteDiffuse,whiteSpecular, refractNorm,0,3.000293f };
	Material solidRed = { whiteAmbient,redDiffuse,whiteSpecular,refractNorm ,0,0 };
	Sphere* test1 = new Sphere(0, Vec3Df(-1.0f, -0.9f, 3), solidRed, 0.5f);
	objects.push_back(test1);
	Sphere* test2 = new Sphere(1, Vec3Df(0, -0.9f, 3), solidRed, 0.5f);
	objects.push_back(test2);
	Sphere* test3 = new Sphere(2, Vec3Df(1, -0.9f, 3), blueMirror, 0.5f);
	objects.push_back(test3);

	Plane* pDown = new Plane(3, solidRed, Vec3Df(0, 1, 0), -0.1f);
	Plane* pUp = new Plane(4, solidRed, Vec3Df(0, 1, 0), -5);
	Plane* pRight = new Plane(5, solidRed, Vec3Df(1, 0, 0), -2);
	Plane* pLeft = new Plane(6, solidRed, Vec3Df(-1, 0, 0), -5);
	Plane* pFront = new Plane(7, solidRed, Vec3Df(0, 0, 1), -5);
	Plane* pBack = new Plane(8, solidRed, Vec3Df(0, 0, -1), -5);
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
