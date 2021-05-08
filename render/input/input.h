#pragma once
#include <functional>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
class GLFWwindow;
class Scene;

typedef  std::function<void(double, double)> mouseEvent;
 
class InputSystem {

	GLFWwindow* _win;
	Scene* _scene;

	
//	static mouseEvent _lButtonDown;
	static mouseEvent _lButtonDown;
	static mouseEvent _rButtonDown;

public:
	InputSystem(){
	}
	
public:

	static void LButtonDown(double x, double y) {
		if (_lButtonDown) {
			_lButtonDown(x, y);
		}
	}
	static void RButtonDown(double x, double y) {
		if (_rButtonDown) {
			_rButtonDown(x, y);
		}
	}


	inline void registerLButtoneDown(mouseEvent func) {
		_lButtonDown = func;
	}

	inline void registerRButtoneDown(mouseEvent func) {
		_rButtonDown = func;
	}

	void init(GLFWwindow* window, Scene* scene);
	void update();

};