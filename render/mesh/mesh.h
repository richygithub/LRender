#pragma once
#include "glm\glm.hpp"
#include <vector>

typedef uint32_t idxType;

class Mesh {
	std::vector<glm::vec3> _verts;
	std::vector<glm::vec3> _normals;
	std::vector<glm::vec2> _uvs;
	std::vector<idxType> _tris;
public:
	Mesh(std::vector<glm::vec3>& verts, 
		std::vector<glm::vec3>& normals, 
		std::vector<glm::vec2>& uv,
		std::vector<idxType>& tris
		);
	
	Mesh() {};
	inline void setVerts(std::vector<glm::vec3>& verts) { _verts = verts; };
	inline void setTris(std::vector< idxType>& tris) { _tris= tris; };


	const glm::vec3* getVertsData()const;
	inline const std::vector<glm::vec3>& getVerts()const { return _verts; };
	inline const std::vector<idxType>& getTris()const { return _tris; };
	inline const std::vector<glm::vec2>& getUVs()const { return _uvs; };
	inline const std::vector<glm::vec3>& getNormals()const { return _normals; };
	






};