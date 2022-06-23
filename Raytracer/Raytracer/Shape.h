#pragma once

#define BLUE_REFRACT 0
#define SOLID_RED 1
#define SOLID_GREEN 2
#define SOLID_BLUE 3
#define PINK_WALL 4
class Shape {
public:
	Shape(int id, int mid);
	~Shape();
	int id;
	virtual bool hit(Ray r, HitInfo* current);
	virtual bool fastHit(Ray r);
	Material mat;

	static Material getMat(int mid);
};