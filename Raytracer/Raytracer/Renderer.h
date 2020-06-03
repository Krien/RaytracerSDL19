#pragma once
// Forward declaration
class Scene;
class Camera;
struct Material
{
	Vec3Df ambientColor;
	Vec3Df diffuseColor;
	Vec3Df specularColor;
	float mirror;
	float refracIndex;
};


struct HitInfo {
	Vec3Df normal;
	Vec3Df hitPos;
	float distance;
	Material material;
};

struct Ray
{
	Vec3Df origin;
	Vec3Df direction;
	float length;
};

class Renderer {
public:
	Renderer(Screen* screen);
	~Renderer();
	virtual void draw(int iteration);
	void flush();
	void setScene(Scene* scene);
	void setCamera(Camera* camera);
protected:
	virtual void initRenderer();
	constexpr void setPixel(SDL_Color c, int i)
	{
		*(pixelBuffer + i) = c.b;
		*(pixelBuffer + i + 1) = c.g;
		*(pixelBuffer + i + 2) = c.r;
		*(pixelBuffer + i + 3) = c.a;
	};
	Screen* screen = NULL;
	Pixel* pixelBuffer;
	Scene* currentScene = NULL;
	Camera* camera = NULL;
	int pixelsSize;
	int width, height;
};