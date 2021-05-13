/*
 * @Author: libl
 * @Date: 2021-04-08 10:29:37
 * @LastEditors: libl
 * @Description: 
 */
#pragma once 

#include <vector>
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
	Cfg() :tileSize(32), cellSize(0.2), gridSize(0.5),lineError(2), removeHoles(true), minBlock(4){}
};
extern Cfg gBuildCfg;
struct NavmeshBuilder {
	Tile* _tiles;
	int _width;
	int _height;

	float _cellSize;
	int _tileSize;
	glm::vec3 _max;
	glm::vec3 _min;


	void init(Cfg cfg);

	std::vector<glm::vec3>  rasterize(const std::vector<glm::vec3>& verts, const std::vector<uint32_t>& tris, float y);
	void build(Cfg cfg );
	void build_Tri(Cfg cfg );
	void debug_addOutline(Cfg cfg );
	void debug_removeHole(Cfg cfg );





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
extern NavmeshBuilder gMeshBuilder;

