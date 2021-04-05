#include <string>
#include "GL\glew.h"

#include "scene.h"
#include "obj\object.h"
#include "mesh\mesh.h"
#include "shader\shader.h"
#include "material\material.h"


Scene::Scene():_seqnum(0) {
	//init();
}
Scene::~Scene() {
	for (auto& obj : _objects) {
		delete obj.second;
	}
	_objects.clear();
}

void Scene::addObject(Object* obj) {
	obj->_id = ++_seqnum;
	_objects.insert(std::make_pair(obj->_id, obj));

}
void Scene::removeObject(int id) {
	_objects.erase(id);
}

void Scene::init() {

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);


	//test
	std::vector<glm::vec3> verts;
	verts.push_back(glm::vec3(-1.0f, -1.0f, 0.0f));
	verts.push_back(glm::vec3(1.0f, -1.0f, 0.0f));
	verts.push_back(glm::vec3(0.0f, 1.0f, 0.0f));


	Mesh* mesh = new Mesh();
	mesh->setVerts(verts);

	Object* obj = new Object(mesh,new Material("simple") );

	addObject(obj);

}

void Scene::render(Object& obj) {
	//load shader
	//obj._material->
	GLuint programId = Shader::getProgram(obj._material->getShaderName());
	glUseProgram(programId);
	GLuint MatrixId = glGetUniformLocation(programId, "MVP");
	const glm::mat4& Model = obj.getMatrixM();
	glm::mat4 MVP = _matrixVP * Model;
	glUniformMatrix4fv(MatrixId, 1, GL_FALSE, &MVP[0][0]);
	//set mvp matrix

	//load verts
	if (obj._vaoId == 0) {
		glGenVertexArrays(1, &obj._vaoId);
	}
	if (obj._vboId == 0) {
		glGenBuffers(1, &obj._vboId);
		glBindBuffer(GL_ARRAY_BUFFER, obj._vboId);
		auto verts = obj.getMesh()->getVerts();
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*verts.size(), verts.data(), GL_STATIC_DRAW);
	}
	glBindVertexArray(obj._vaoId);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, obj._vboId);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle

	glDisableVertexAttribArray(0);


	//

}
void Scene::update() {
	//set MVP
	_matrixVP = _camera.getVPMatrix();
	auto clearColor = _camera.getClearColor();
	glClearColor(clearColor.r,clearColor.g,clearColor.b,clearColor.a);
	glClear(GL_COLOR_BUFFER_BIT);


	for (auto& obj : _objects) {
		render(*obj.second);
	}
}
