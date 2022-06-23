#pragma once
class Light
{
public:
	Light(Vec3Df position, Vec3Df intensity);
	~Light();
	Vec3Df position;
	Vec3Df intensity;
	__m256 posX;
	__m256 posY;
	__m256 posZ;
	__m256 intensity8;
};

