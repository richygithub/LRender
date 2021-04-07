#include "object.h"
#include "mesh\mesh.h"
#include "material\material.h"

Object::Object(Mesh* mesh, Material* mat):
		_mesh(mesh),
		_material(mat), 
		_matrixM(glm::mat4(1.0f)), 
		_position(glm::mat3(0.0f)),
		_id(0),
		_vaoId(0),
		_vboId(0),
		_eleId(0),
		_uvId(0),
		_normalId(0),
		visiable(true)
{
	//_matrixM[0][0] = -1;

}

//Object::Object(Object&& obj):_mesh(obj._mesh),
//	_material(obj._material),
//	_id(obj._id),
//	_position(obj._position),
//	_matrixM(obj._matrixM),
//	_vaoId(obj._vaoId),
//	_vboId(obj._vboId)
//{
//	
//	obj._mesh = nullptr;
//	obj._material = nullptr;
//}
Object::~Object() {
	if (_mesh != nullptr) {
		delete _mesh;
	}
	if (_material != nullptr) {
		delete _material;
	}
	if (_vaoId != 0) {
		glDeleteVertexArrays(1, &_vaoId);
	}
	if (_vboId != 0) {
		glDeleteBuffers(1, &_vboId);
	}
	if (_eleId != 0) {
		glDeleteBuffers(1, &_eleId);
	}
	if (_uvId!= 0) {
		glDeleteBuffers(1, &_uvId);
	}
	if (_normalId!= 0) {
		glDeleteBuffers(1, &_normalId);
	}

}

void Object::setPosition(const glm::mat3& position) {
	_position = position;
	//update matrix M;
}
	
