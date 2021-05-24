/*
 * @Author: libl
 * @Date: 2021-04-08 10:29:37
 * @LastEditors: libl
 * @Description: 
 */
#pragma once 

#include <vector>
#include <string>
#include "glm\glm.hpp"
#include "tile.h"
#include "navmesh.h"


struct Cfg {
	int tileSize;
	float cellSize;
	float gridSize;
	int lineError;
	bool removeHoles;
	int minBlock;
	int agentRadius;
	Cfg() :tileSize(128), cellSize(1), gridSize(0.5),lineError(2), removeHoles(true), minBlock(4), agentRadius(1){}
};
struct NavmeshBuilder {
	Tile* _tiles;
	int _width;
	int _height;

	float _cellSize;
	int _tileSize;
	glm::vec3 _max;
	glm::vec3 _min;

	std::vector<glm::vec3> debug_allVerts;
	std::vector<int> debug_allTris;



	void clear();

	std::vector<glm::vec3>  rasterize(const std::vector<glm::vec3>& verts, const std::vector<uint32_t>& tris, float y);

	void buildAll(Cfg cfg );
	
	void build(Cfg cfg );
	void build_Tri(Cfg cfg );
	void debug_addOutline(Cfg cfg );
	void debug_removeHole(Cfg cfg );

	void generateTris();

	void addUnityObj(const glm::vec3* verts,int vertNum, const int* tris,int triNum);
	void addUnityGround(const glm::vec3* verts, int vertNum, const int* tris, int triNum);

	NavmeshBuilder();
	~NavmeshBuilder();

	void setPlane(const std::vector<glm::vec3>& verts);

	void clearGround();
	void addGround(const std::vector<glm::vec3>& verts);

	inline void addObj(const std::vector<glm::vec3>& verts, const std::vector<uint32_t>& tris) { 
		_objVerts.push_back(verts); 
		_objTris.push_back(tris);
	}
	inline void clearObj() {
		_objVerts.clear();
		_objTris.clear();
	}
	std::vector< std::vector<glm::vec3> > _debug;

	NavMesh_RunTime::NavMesh* _rtData;
	NavMesh_RunTime::NavMeshQuery _query;

	void debug();
	void seralize();
	void save(std::string path);
	void load(const char* bytes,int len);
private:

	std::vector<glm::vec3> _planes;

	std::vector< std::vector<glm::vec3> > _objVerts;
	std::vector< std::vector<uint32_t> >  _objTris;

	void setCellBlock(float x,float y);
	void setCellBlock(int x,int y);
	glm::ivec2 getCellXY(float x, float y);
	void rasterizeFlatBottomTri(const std::vector<glm::vec3>& verts );
	void rasterizeFlatTopTri(const std::vector<glm::vec3>& verts );

	void linkTile(Tile& tile);

};

