#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>


Camera::Camera():_fov(45.0f),
	_aspect(4.0f/3.0f),
	_near(0.1f),
	_far(1000.0f),
	_position(0.0f,0.0f,5.0f),
	_lookAt(0.0f,0.0f,0.0f),
	_clearColor(0.23f,0.23f,0.33f,1.0f)
{

}



glm::mat4 Camera::getVPMatrix() {
	glm::mat4 Projection = glm::perspective(glm::radians(_fov), _aspect, _near, _far);

	glm::mat4 View = glm::lookAt(
		_position, // Camera is at (4,3,3), in World Space
		_lookAt,
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	return Projection * View;
}

