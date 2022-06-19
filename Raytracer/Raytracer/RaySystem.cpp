#include "precomp.h"

#define SIZE SCREEN_WIDTH * SCREEN_HEIGHT

RaySystem::RaySystem(Scene* scene)
{
	RaySystem::scene = scene;
}

Vec3Df* RaySystem::trace()
{
	__m256 rx8[RAY_COUNT];
	__m256 ry8[RAY_COUNT];
	__m256 rz8[RAY_COUNT];

	Vec3Df* traceResult = new Vec3Df[SIZE];
	for (int i = 0; i < SIZE / 8; i++)
	{
		traceResult[i * 8    ] = Vec3Df(rx8[i].m256_f32[0], ry8[i].m256_f32[0], rz8[i].m256_f32[0]);
		traceResult[i * 8 + 1] = Vec3Df(rx8[i].m256_f32[1], ry8[i].m256_f32[1], rz8[i].m256_f32[1]);
		traceResult[i * 8 + 2] = Vec3Df(rx8[i].m256_f32[2], ry8[i].m256_f32[2], rz8[i].m256_f32[2]);
		traceResult[i * 8 + 3] = Vec3Df(rx8[i].m256_f32[3], ry8[i].m256_f32[3], rz8[i].m256_f32[3]);
		traceResult[i * 8 + 4] = Vec3Df(rx8[i].m256_f32[4], ry8[i].m256_f32[4], rz8[i].m256_f32[4]);
		traceResult[i * 8 + 5] = Vec3Df(rx8[i].m256_f32[5], ry8[i].m256_f32[5], rz8[i].m256_f32[5]);
		traceResult[i * 8 + 6] = Vec3Df(rx8[i].m256_f32[6], ry8[i].m256_f32[6], rz8[i].m256_f32[6]);
		traceResult[i * 8 + 7] = Vec3Df(rx8[i].m256_f32[7], ry8[i].m256_f32[7], rz8[i].m256_f32[7]);
	}

	return traceResult;
}

HitInfo* RaySystem::hit()
{
	// hit info
	__m256 hit_nx8[RAY_COUNT];
	__m256 hit_ny8[RAY_COUNT];
	__m256 hit_nz8[RAY_COUNT];
	__m256 hit_px8[RAY_COUNT];
	__m256 hit_py8[RAY_COUNT];
	__m256 hit_pz8[RAY_COUNT];
	__m256 hit_dist8[RAY_COUNT];
	__m256i hit_mat8[RAY_COUNT];
	__m256i hit_id8[RAY_COUNT];

	std::vector<Shape*> shapes = scene->objects;
	for (size_t i = 0; i < shapes.size(); i++)
	{
		
	}

	HitInfo* hitResult = new HitInfo[SIZE];
	for (int i = 0; i < SIZE / 8; i++)
	{
		hitResult[i * 8    ] = HitInfo{ Vec3Df(hit_nx8[i].m256_f32[0], hit_ny8[i].m256_f32[0], hit_nz8[i].m256_f32[0]),
									   Vec3Df(hit_px8[i].m256_f32[0], hit_py8[i].m256_f32[0], hit_pz8[i].m256_f32[0]),
									   hit_dist8[i].m256_f32[0], Shape::getMat(hit_mat8[i].m256i_i32[0]), hit_id8[i].m256i_i32[0] };
		hitResult[i * 8 + 1] = HitInfo{ Vec3Df(hit_nx8[i].m256_f32[1], hit_ny8[i].m256_f32[1], hit_nz8[i].m256_f32[1]),
									   Vec3Df(hit_px8[i].m256_f32[1], hit_py8[i].m256_f32[1], hit_pz8[i].m256_f32[1]),
									   hit_dist8[i].m256_f32[1], Shape::getMat(hit_mat8[i].m256i_i32[1]), hit_id8[i].m256i_i32[1] };
		hitResult[i * 8 + 2] = HitInfo{ Vec3Df(hit_nx8[i].m256_f32[2], hit_ny8[i].m256_f32[2], hit_nz8[i].m256_f32[2]),
									   Vec3Df(hit_px8[i].m256_f32[2], hit_py8[i].m256_f32[2], hit_pz8[i].m256_f32[2]),
									   hit_dist8[i].m256_f32[2], Shape::getMat(hit_mat8[i].m256i_i32[2]), hit_id8[i].m256i_i32[2] };
		hitResult[i * 8 + 3] = HitInfo{ Vec3Df(hit_nx8[i].m256_f32[3], hit_ny8[i].m256_f32[3], hit_nz8[i].m256_f32[3]),
									   Vec3Df(hit_px8[i].m256_f32[3], hit_py8[i].m256_f32[3], hit_pz8[i].m256_f32[3]),
									   hit_dist8[i].m256_f32[3], Shape::getMat(hit_mat8[i].m256i_i32[3]), hit_id8[i].m256i_i32[3] };
		hitResult[i * 8 + 4] = HitInfo{ Vec3Df(hit_nx8[i].m256_f32[4], hit_ny8[i].m256_f32[4], hit_nz8[i].m256_f32[4]),
									   Vec3Df(hit_px8[i].m256_f32[4], hit_py8[i].m256_f32[4], hit_pz8[i].m256_f32[4]),
									   hit_dist8[i].m256_f32[4], Shape::getMat(hit_mat8[i].m256i_i32[4]), hit_id8[i].m256i_i32[4] };
		hitResult[i * 8 + 5] = HitInfo{ Vec3Df(hit_nx8[i].m256_f32[5], hit_ny8[i].m256_f32[5], hit_nz8[i].m256_f32[5]),
									   Vec3Df(hit_px8[i].m256_f32[5], hit_py8[i].m256_f32[5], hit_pz8[i].m256_f32[5]),
									   hit_dist8[i].m256_f32[5], Shape::getMat(hit_mat8[i].m256i_i32[5]), hit_id8[i].m256i_i32[5] };
		hitResult[i * 8 + 6] = HitInfo{ Vec3Df(hit_nx8[i].m256_f32[6], hit_ny8[i].m256_f32[6], hit_nz8[i].m256_f32[6]),
									   Vec3Df(hit_px8[i].m256_f32[6], hit_py8[i].m256_f32[6], hit_pz8[i].m256_f32[6]),
									   hit_dist8[i].m256_f32[6], Shape::getMat(hit_mat8[i].m256i_i32[6]), hit_id8[i].m256i_i32[6] };
		hitResult[i * 8 + 7] = HitInfo{ Vec3Df(hit_nx8[i].m256_f32[7], hit_ny8[i].m256_f32[7], hit_nz8[i].m256_f32[7]),
									   Vec3Df(hit_px8[i].m256_f32[7], hit_py8[i].m256_f32[7], hit_pz8[i].m256_f32[7]),
									   hit_dist8[i].m256_f32[7], Shape::getMat(hit_mat8[i].m256i_i32[7]), hit_id8[i].m256i_i32[7] };
	}

	return hitResult;
}
