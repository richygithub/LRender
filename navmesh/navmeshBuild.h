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

struct Cfg {
	int tileSize;
	float cellSize;
	float gridSize;
	Cfg() :tileSize(32), cellSize(0.2), gridSize(0.5) {}
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

	NavmeshBuilder();
	~NavmeshBuilder();

	void setPlane(const std::vector<glm::vec3>& verts);

	inline void addObj(const std::vector<glm::vec3>& verts, const std::vector<uint32_t>& tris) { 
		_objVerts.push_back(verts); 
		_objTris.push_back(tris);
	}
	inline void clearObj() {
		_objVerts.clear();
		_objTris.clear();
	}
	std::vector< std::vector<glm::vec3> > _debug;

	void debug();
private:

	std::vector<glm::vec3> _planes;

	std::vector< std::vector<glm::vec3> > _objVerts;
	std::vector< std::vector<uint32_t> >  _objTris;

	void setCellBlock(float x,float y);
	void setCellBlock(int x,int y);
	glm::ivec2 getCellXY(float x, float y);
	void rasterizeFlatBottomTri(const std::vector<glm::vec3>& verts );
	void rasterizeFlatTopTri(const std::vector<glm::vec3>& verts );



};
extern NavmeshBuilder gMeshBuilder;

