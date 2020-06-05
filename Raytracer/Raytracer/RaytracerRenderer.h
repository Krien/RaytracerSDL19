#pragma once
class Shape;
class Light;

class RaytracerRenderer : public Renderer
{
public:
	RaytracerRenderer(Screen* screen);
	~RaytracerRenderer();
	void draw(int iteration) override;
private:
	int width, height;
	Vec3Df trace(Ray r, int depth);
	Vec3Df calculateLight(HitInfo hitI, Vec3Df direction);
	// Temps might move
	size_t shapeSize;
	std::vector<Shape*> shapes;
	size_t lightSize;
	std::vector<Light*> lights;
	int lastId = -1;
	// Helper functions
	Ray mirrorRay(Vec3Df originalDir, HitInfo hit);
	float rayCosTheta(Ray r, Vec3Df normal, float rIndex);
	Vec3Df refractRay(Ray r, Vec3Df normal, float rIndex, float rayCosTheta);
};

