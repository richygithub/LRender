#pragma once
#include <vector>
#include <map>
#include "obj\object.h"
#include "camera\camera.h"
class Scene {

	std::map<int, Object*> _objects;
	int _seqnum;

	Camera _camera;
	glm::mat4 _matrixVP;
private:
	void render(Object& obj);
public:
	inline Camera& getCamera() { return _camera; }

	Scene();
	~Scene();

	void init();
	void update();

	void addObject(Object* obj);
	inline const std::map<int, Object*>& getObjs() const { return _objects; };
	void removeObject(int id);


};
