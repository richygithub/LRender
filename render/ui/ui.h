#pragma once
#include "GL\glew.h"
#include "GLFW/glfw3.h"
class Scene;
class UI {

	GLFWwindow* _win;
	Scene* _scene;
	
private: 
	void updateCamera();

public:
	void update();
	void init(GLFWwindow* window,Scene* scene);



};