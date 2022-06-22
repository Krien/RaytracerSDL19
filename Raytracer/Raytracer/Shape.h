#pragma once

#define BLUE_MIRROR 0
#define SOLID_RED 1

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
