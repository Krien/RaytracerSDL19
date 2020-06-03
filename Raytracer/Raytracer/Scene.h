#pragma once
class Scene
{
public:
	// DEBUG
	Scene();
	Scene(std::vector<Shape*> objects);
	~Scene();
	std::vector<Shape*> objects;
	std::vector<Light*> lights;
};

