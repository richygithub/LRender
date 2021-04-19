#pragma once
#include <GLFW/glfw3.h>
class GLFWwindow;
class Scene;
class InputSystem {

	GLFWwindow* _win;
	Scene* _scene;
public:
	void init(GLFWwindow* window, Scene* scene);
	void update();

};