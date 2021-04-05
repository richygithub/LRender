#pragma once
#include "glm\glm.hpp"
#include <vector>
class Mesh {
	std::vector<glm::vec3> _verts;
	std::vector<glm::vec3> _normals;
	std::vector<glm::vec2> _uvs;
	std::vector<glm::ivec3> _tris;
public:
	Mesh(std::vector<glm::vec3>& verts, 
		std::vector<glm::vec3>& normals, 
		std::vector<glm::vec2>& uv,
		std::vector<glm::ivec3>& tris
		);
	
	Mesh() {};
	inline void setVerts(std::vector<glm::vec3>& verts) { _verts = verts; };
	const glm::vec3* getVertsData()const;
	const std::vector<glm::vec3>& getVerts()const { return _verts; };



};