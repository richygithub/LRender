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
	void build(const std::vector<glm::vec3>& planeVerts, Cfg cfg);

	NavmeshBuilder();
	~NavmeshBuilder();

private:
	void setCellBlock(float x,float y);
	void rasterizeFlatBottomTri(const std::vector<glm::vec3>& verts );
	void rasterizeFlatTopTri(const std::vector<glm::vec3>& verts );



};
extern NavmeshBuilder gMeshBuilder;

