#pragma once
#include "GL\glew.h"
#include "GLFW/glfw3.h"
class InputSystem;
class Scene;
class UI {

	GLFWwindow* _win;
	Scene* _scene;
	InputSystem* _input;
	
private: 
	void updateCamera();

public:

	void lbuttondown(double x,double y);
	void update();
	void init(GLFWwindow* window,Scene* scene, InputSystem* input);



};