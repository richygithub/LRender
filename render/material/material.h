#pragma once
#include <string>
//#include <GL\glew.h>
class Material {

	std::string _shaderName;
public:
	Material() {};
	Material(const std::string& name):_shaderName(name){}
	inline void setShaderName(std::string& name) {
		_shaderName = name;
	}
	inline const std::string& getShaderName() {
		return _shaderName;
	}

	
};