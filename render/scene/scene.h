#pragma once
#include <vector>
#include <map>
#include "obj\object.h"
#include "camera\camera.h"
#include "glm\glm.hpp"
#include "mesh\mesh.h"
class Scene {

	std::map<int, Object*> _objects;
	int _seqnum;

	Camera _camera;
	glm::mat4 _matrixVP;
	std::vector<  std::vector< glm::vec3 > > _mesh;

	std::vector<  std::vector< glm::vec3 > > _lines;

private:
	void render(Object& obj);
	
	void renderWireFrame(Object& obj);

	void renderLines( const std::vector< glm::vec3 > & verts,const std::vector<idxType>& lines);

	void renderTris(const std::vector<glm::vec3>&verts);


public:

	void renderLines(const std::vector< glm::vec3 >& verts);

	inline Camera& getCamera() { return _camera; }

	Scene();
	~Scene();

	void init();
	void update();

	void addObject(Object* obj);
	inline const std::map<int, Object*>& getObjs() const { return _objects; };
	void removeObject(int id);
	void addMesh(const std::vector<glm::vec3>& verts);
	void addLines(const std::vector<glm::vec3>& verts) { _lines.push_back(verts); };




};
