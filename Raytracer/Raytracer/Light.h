#pragma once
class Light
{
public:
	Light(Vec3Df position, Vec3Df intensity);
	~Light();
	Vec3Df position;
	Vec3Df intensity;
};

