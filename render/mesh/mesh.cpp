#include "mesh.h"


Mesh::Mesh(std::vector<glm::vec3>& verts,
	std::vector<glm::vec3>& normals,
	std::vector<glm::vec2>& uvs,
	std::vector<idxType>& tris
) {
	_verts = verts;
	_normals = normals;
	_uvs = uvs;
	_tris = tris;
	for (int idx = 0; idx < _tris.size(); idx += 3) {
		idxType v0 = _tris[idx];
		idxType v1 = _tris[idx + 1];
		idxType v2 = _tris[idx + 2];

		_lines.push_back(v0);
		_lines.push_back(v1);

		_lines.push_back(v1);
		_lines.push_back(v2);

		_lines.push_back(v2);
		_lines.push_back(v0);


	}

}

const glm::vec3* Mesh::getVertsData() const{
	return _verts.data();
}

const std::vector<idxType>& Mesh::getLines()const {
	return _lines;
}
	

