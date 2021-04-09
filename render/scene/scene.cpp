#include <string>
#include "GL\glew.h"

#include "scene.h"
#include "obj\object.h"
#include "mesh\mesh.h"
#include "shader\shader.h"
#include "material\material.h"
#include "utils\meshLoader.h"


Scene::Scene() :_seqnum(0) {
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
void Scene::addMesh(const std::vector<glm::vec3>& verts){
	_mesh.push_back(verts);
}
void Scene::removeObject(int id) {
	_objects.erase(id);
}

void Scene::init() {

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//glEnable(GL_CULL_FACE);
	//test
	//std::vector<glm::vec3> verts;
	//verts.push_back(glm::vec3(-1.0f, -1.0f, 0.0f));
	//verts.push_back(glm::vec3(1.0f, -1.0f, 0.0f));
	//verts.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

	//std::vector<idxType> tris;
	//tris.push_back(0);
	//tris.push_back(1);
	//tris.push_back(2);
	//
	//std::vector<glm::vec3> normals;
	//normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	//normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	//normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));

	//std::vector<glm::vec2> uvs;
	//uvs.push_back(glm::vec2(0.0f, 0.0f));
	//uvs.push_back(glm::vec2(1.0f, 0.0f));
	//uvs.push_back(glm::vec2(0.5f, 1.0f));


	//Mesh* mesh = new Mesh(verts,normals,uvs,tris);
	////mesh->setVerts(verts);
	////mesh->setTris(tris);

	//Object* obj = new Object(mesh,new Material("st") );
	//obj->setName("simple triangle");
	//addObject(obj);
	std::string objfile = "res/mesh/unitytest.obj";
	auto objs = loadObj(objfile);
	for (auto& obj : objs) {
		addObject(obj);
	}


}

void Scene::renderTris(const std::vector<glm::vec3>& verts) {
	GLuint programId = Shader::getProgram("tris");
	glUseProgram(programId);
	GLuint MatrixId = glGetUniformLocation(programId, "MVP");

	glm::mat4 MVP = _matrixVP;

	glUniformMatrix4fv(MatrixId, 1, GL_FALSE, &MVP[0][0]);

	GLuint vao;
	GLuint vbo;



	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * verts.size(), verts.data(), GL_STATIC_DRAW);



	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glDrawArrays(GL_TRIANGLES, 0, verts.size()); // 3 indices starting at 0 -> 1 triangle
	glDisableVertexAttribArray(0);

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);

}
void Scene::renderQuads(const std::vector< glm::vec3 >& verts) {
	GLuint programId = Shader::getProgram("tris");
	glUseProgram(programId);
	GLuint MatrixId = glGetUniformLocation(programId, "MVP");

	glm::mat4 MVP = _matrixVP;

	glUniformMatrix4fv(MatrixId, 1, GL_FALSE, &MVP[0][0]);

	GLuint vao;
	GLuint vbo;



	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * verts.size(), verts.data(), GL_STATIC_DRAW);



	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glDrawArrays(GL_QUADS, 0, verts.size()); // 3 indices starting at 0 -> 1 triangle
	glDisableVertexAttribArray(0);

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);

}

void Scene::renderLines(const std::vector< glm::vec3 >& verts) {
	GLuint programId = Shader::getProgram("line");
	glUseProgram(programId);
	GLuint MatrixId = glGetUniformLocation(programId, "MVP");

	glm::mat4 MVP = _matrixVP;

	glUniformMatrix4fv(MatrixId, 1, GL_FALSE, &MVP[0][0]);

	GLuint vao;
	GLuint vbo;



	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * verts.size(), verts.data(), GL_STATIC_DRAW);



	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glDrawArrays(GL_LINES, 0, verts.size()); // 3 indices starting at 0 -> 1 triangle
	glDisableVertexAttribArray(0);

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);

}

void Scene::renderLines(const std::vector< glm::vec3 >& verts, const std::vector<idxType>& lines) {

	GLuint programId = Shader::getProgram("line");
	glUseProgram(programId);
	GLuint MatrixId = glGetUniformLocation(programId, "MVP");

	glm::mat4 MVP = _matrixVP ;

	glUniformMatrix4fv(MatrixId, 1, GL_FALSE, &MVP[0][0]);
	
	GLuint vao;
	GLuint vbo;
	GLuint ele;



	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * verts.size(), verts.data(), GL_STATIC_DRAW);
	

	glGenBuffers(1, &ele);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ele);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, lines.size() * sizeof(idxType), lines.data(), GL_STATIC_DRAW);


	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ele);
	glDrawElements(
		GL_LINES,      // mode
		lines.size(),    // count
		GL_UNSIGNED_INT,   // type
		(void*)0           // element array buffer offset
	);

	glDisableVertexAttribArray(0);

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ele);

}





void Scene::renderWireFrame(Object& obj) {
	auto mesh = obj.getMesh();
	auto& verts=mesh->getVerts();
	auto& lines = mesh->getLines();
	renderLines(verts, lines);

}


void Scene::render(Object& obj) {
	//load shader
	//obj._material->
	GLuint programId = Shader::getProgram(obj._material->getShaderName());
	glUseProgram(programId);
	GLuint MatrixId = glGetUniformLocation(programId, "MVP");
	GLuint Matrix_M = glGetUniformLocation(programId, "M");
	GLuint Matrix_VP = glGetUniformLocation(programId, "VP");
	GLuint eyePos = glGetUniformLocation(programId, "eyePosition");


	const glm::mat4& Model = obj.getMatrixM();
	glm::mat4 MVP = _matrixVP * Model;
	glUniformMatrix4fv(MatrixId, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(Matrix_M, 1, GL_FALSE, &Model[0][0]);
	glUniformMatrix4fv(Matrix_VP, 1, GL_FALSE, &_matrixVP[0][0]);
	auto cameraPos = _camera.getPosition();
	glUniform3f(eyePos, cameraPos.x, cameraPos.y, cameraPos.z);


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

	if (obj._uvId == 0) {
		glGenBuffers(1, &obj._uvId);
		glBindBuffer(GL_ARRAY_BUFFER, obj._uvId);
		auto uvs = obj.getMesh()->getUVs();
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * uvs.size(), uvs.data(), GL_STATIC_DRAW);
	}

	if (obj._normalId == 0) {
		glGenBuffers(1, &obj._normalId);
		glBindBuffer(GL_ARRAY_BUFFER, obj._normalId);
		auto normals= obj.getMesh()->getNormals();
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
	}




	auto tris = obj.getMesh()->getTris();
	if (obj._eleId == 0 ) {
		glGenBuffers(1, &obj._eleId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj._eleId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, tris.size() * sizeof(idxType), tris.data() , GL_STATIC_DRAW);
	}

	glBindVertexArray(obj._vaoId);
	glBindBuffer(GL_ARRAY_BUFFER, obj._vboId);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	//glBindBuffer(GL_ARRAY_BUFFER, obj._vboId);
	//// Draw the triangle !
	//glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, obj._uvId);
	glVertexAttribPointer(
		1,                                // attribute
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, obj._normalId);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj._eleId);
	glDrawElements(
		GL_TRIANGLES,      // mode
		tris.size(),    // count
		GL_UNSIGNED_INT,   // type
		(void*)0           // element array buffer offset
	);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);




	//

}
void Scene::update() {
	//set MVP
	_matrixVP = _camera.getVPMatrix();
	auto clearColor = _camera.getClearColor();
	glClearColor(clearColor.r,clearColor.g,clearColor.b,clearColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto& obj : _objects) {
		if (obj.second->visiable) {

			//renderWireFrame(*obj.second);
			render(*obj.second);

		}
	}

	for (auto& tris : _mesh) {
		renderTris(tris);
	}

	for (auto& line: _lines) {
		renderLines(line);
	}
	//glDisable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);

	//glBegin(GL_LINES);
	//glColor4ub(255, 0, 0, 255); glVertex3f(-0.5f, -0.25f, -2.5f);
	//glColor4ub(0, 0, 255, 255); glVertex3f(0.5f, -0.25f, -2.5f);
	//glColor4ub(0, 255, 0, 255); glVertex3f(1.0f, 0.5f, -2.5f);
	//glColor4ub(255, 0, 0, 255); glVertex3f(0.5f, 0.5f, -2.5f);
	//glEnd();



}
