#include <GL\glew.h>
#include <GLFW/glfw3.h>

#include "input.h"

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "scene\scene.h"
using namespace glm;



double preX, preY;
bool leftButtonDown = false;
bool rightButtonDown = false;


static void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {

		if (GLFW_PRESS == action)
			rightButtonDown = true;
		else if (GLFW_RELEASE == action)
			rightButtonDown = false;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (GLFW_PRESS == action)
			leftButtonDown = true;
		else if (GLFW_RELEASE == action)
			leftButtonDown = false;
	}
}


void InputSystem::init(GLFWwindow* window,Scene* scene) {
	_win = window;
	_scene = scene;

	glfwSetMouseButtonCallback(window, mouse_callback);
}



void InputSystem::update() {

	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);
	lastTime = currentTime;

	double xpos, ypos;
	
	glfwGetCursorPos(_win, &xpos, &ypos);

	auto& camera = _scene->getCamera();

	if (rightButtonDown) {

		//printf("move %.3f %.3f --> %.3f,%3.f\n",xpos,ypos,xpos - preX,ypos - preY);
		vec3 diff = vec3(xpos - preX, ypos - preY, 0);
		if (length(diff) > 0) {
			diff = normalize(diff);
		}

		//	auto matrixP = camera.getMatrixP();
		//auto matrixV = camera.getMatrixV();
		//glm::ivec4 viewport;
		//glGetIntegerv(GL_VIEWPORT, (int*)&viewport);
		//ypos = (float)viewport[3] - ypos;


		//auto wpos = glm::unProject(glm::vec3(xpos, ypos, 0), matrixV, matrixP, viewport);

		camera._lookAt += diff * camera._moveSpeed;
	}

	preX = xpos;
	preY = ypos;
	vec3 dir;
	if (glfwGetKey(_win, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(_win, GLFW_KEY_W) == GLFW_PRESS ) {
		//position += direction * deltaTime * speed;
		dir = normalize(camera._lookAt - camera._position);

		auto offset = dir * camera._moveSpeed * deltaTime;
		camera._position += offset;
		camera._lookAt += offset;

	}
	if (glfwGetKey(_win, GLFW_KEY_DOWN) == GLFW_PRESS || glfwGetKey(_win, GLFW_KEY_S) == GLFW_PRESS ) {
		//position -= direction * deltaTime * speed;
		dir = normalize(camera._position- camera._lookAt  );

		auto offset = dir * camera._moveSpeed * deltaTime;
		camera._position += offset;
		camera._lookAt += offset;

	}
	// Strafe right
	if (glfwGetKey(_win, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(_win, GLFW_KEY_D) == GLFW_PRESS) {
		//position += right * deltaTime * speed;
		if (camera._position.z > 0) {
			dir = vec3(1, 0, 0);
		}else
			dir = vec3(-1, 0, 0);
	
		auto offset=dir * camera._moveSpeed*deltaTime;
		camera._position += offset;
		camera._lookAt += offset;

	}
	// Strafe left
	if (glfwGetKey(_win, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(_win, GLFW_KEY_A) == GLFW_PRESS) {
		//position -= right * deltaTime * speed;
		if (camera._position.z > 0) {
			dir = vec3(-1, 0, 0);
		}else
			dir = vec3(1, 0, 0);
	
		auto offset = dir * camera._moveSpeed*deltaTime;
		camera._position += offset;
		camera._lookAt += offset;


	}

}

