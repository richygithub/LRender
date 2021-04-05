#pragma once
#include "glm\glm.hpp"
class Camera {

	glm::vec3 _position;
	glm::vec3 _lookAt;


	float _fov;
	float _aspect;
	float _near;
	float _far;
	glm::vec4 _clearColor;
	friend class UI;
public:

	Camera();

	inline void setFOV(float fov) {		_fov = fov; 	}
	inline void setAspect(float aspect) {		_aspect = aspect; 	}
	inline void setNearFar(float near, float far) { _near = near; _far = far; }
	inline void setClearColor(glm::vec4 color) { _clearColor = color; }
	inline void setPosition(glm::vec3 position) { _position = position; }
	inline void setLookAt(glm::vec3 lookAt) { _lookAt = lookAt; }

	inline float getFOV() {		return _fov; 	}
	inline float getAspect() {		return _aspect ; 	}
	inline float getNear() {return _near;	}
	inline float getFar() { return  _far ; }
	inline glm::vec4 getClearColor() { return _clearColor ; }
	inline glm::vec3 getPosition() { return _position ; }
	inline glm::vec3 getLookAt() { return _lookAt ; }

	glm::mat4 getVPMatrix();




};