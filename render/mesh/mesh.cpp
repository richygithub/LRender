#include "mesh.h"


Mesh::Mesh(std::vector<glm::vec3>& verts,
	std::vector<glm::vec3>& normals,
	std::vector<glm::vec2>& uvs,
	std::vector<glm::ivec3>& tris
) {
	_verts = verts;
	_normals = normals;
	_uvs = uvs;
	_tris = tris;

}

const glm::vec3* Mesh::getVertsData() const{
	return _verts.data();
}

