#pragma once
#include <map>
#include <string>
#include <GL/glew.h>
class Shader {

	static std::map< std::string, GLuint> _shaders;
	static std::string path;
public:
	static GLuint load(const std::string& name);
	static GLuint getProgram(const std::string& name);

};