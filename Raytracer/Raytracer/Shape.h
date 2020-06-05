#pragma once

class Shape {
public:
	Shape(int id, Vec3Df position, Material m);
	~Shape();
	Vec3Df position;
	Material material;
	int id;
	virtual void hit(Ray r, HitInfo* current);
	virtual bool fastHit(Ray r);
};
