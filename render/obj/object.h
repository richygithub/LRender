#pragma once
#include "GL\glew.h"

#include "glm\glm.hpp"
class Mesh;
class Material;

class Object {

	friend class Scene;
private:
	Mesh* _mesh;
	Material* _material;
	glm::mat3 _position;
	glm::mat4 _matrixM;
	int _id;

	GLuint _vaoId;
	GLuint _vboId;

	Object(const Object&);
public:
	//Object(Object&& obj);
	Object(Mesh* mesh, Material* mat);
	~Object();
	inline const glm::mat4& getMatrixM() { return _matrixM; }
	void setPosition(const glm::mat3& position);
	inline const Mesh* getMesh()const { return _mesh; }
	

};