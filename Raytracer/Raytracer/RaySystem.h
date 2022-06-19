#pragma once

class RaySystem
{
public:
	RaySystem();

	const static int RAY_COUNT = SCREEN_WIDTH * SCREEN_WIDTH / 8;

	__m256 ox8[RAY_COUNT];
	__m256 oy8[RAY_COUNT];
	__m256 oz8[RAY_COUNT];
	__m256 dx8[RAY_COUNT];
	__m256 dy8[RAY_COUNT];
	__m256 dz8[RAY_COUNT];
	__m256 len8[RAY_COUNT];

	Vec3Df* trace();
};