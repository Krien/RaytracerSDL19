#pragma once

#define BLUE_REFRACT 0
#define SOLID_RED 1
#define SOLID_ORANGE 2
#define PINK_WALL 3
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