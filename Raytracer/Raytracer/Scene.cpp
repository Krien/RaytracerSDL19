#include "precomp.h"

Scene::Scene()
{
	Vec3Df whiteAmbient = Vec3Df(0.1f);
	Vec3Df whiteDiffuse = Vec3Df(0.5f);
	Vec3Df whiteSpecular = Vec3Df(1);
	Vec3Df blueDiffuse = Vec3Df(1,1,1);
	Vec3Df redDiffuse = Vec3Df(0,0,1);
	Material blueMirror = { whiteAmbient,blueDiffuse,whiteSpecular ,0.5f,0.9f };
	Material solidRed = { whiteAmbient,redDiffuse,whiteSpecular ,0,0 };
	Sphere* test1 = new Sphere(Vec3Df(-1.0f, -0.9f, 3), solidRed, 0.5f);
	objects.push_back(test1);
	Sphere* test2 = new Sphere(Vec3Df(0, -0.9f, 3), solidRed, 0.5f);
	objects.push_back(test2);
	Sphere* test3 = new Sphere(Vec3Df(1, -0.9f, 3), blueMirror, 0.5f);
	objects.push_back(test3);
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
