#pragma once

class Shape {
public:
	Shape(Vec3Df position, Material m);
	~Shape();
	Vec3Df position;
	Material material;
	virtual void hit(Ray r, HitInfo* current);
	virtual bool fastHit(Ray r);
};
