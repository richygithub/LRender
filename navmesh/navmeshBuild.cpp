/*
 * @Author: libl
 * @Date: 2021-04-08 10:29:04
 * @LastEditors: libl
 * @Description: 
 */

#include <vector>
#include "navmeshBuild.h"
#include "tile.h"
#include "glm\glm.hpp"
NavmeshBuilder gMeshBuilder;
Cfg gBuildCfg;


using namespace glm;
using namespace std;

//divide
void divideTri(vec3 top,vec3 mid,vec3 bottom, vector<vec3>& flatBottomTri, vector<vec3>& flatTopTri ) {
	flatBottomTri.clear();
	flatTopTri.clear();

	if (top.z < mid.z) {
		swap(top, mid);
	}
	if (top.z < bottom.z) {
		swap(top, bottom);
	}
	if (mid.z < bottom.z) {
		swap(mid, bottom);
	}

	if (top.z == mid.z) {
		flatTopTri.push_back(top);
		flatTopTri.push_back(mid);
		flatTopTri.push_back(bottom);
		return;
	}
	if (mid.z == bottom.z) {
		flatBottomTri.push_back(top);
		flatBottomTri.push_back(mid);
		flatBottomTri.push_back(bottom);
		return;
	}

	//divide

	float t =(top.z - mid.z)/ (top.z - bottom.z);
	float diffx = bottom.x - top.x;
	float nx = bottom.x + diffx * t;
	float ny = bottom.y + (bottom.y - top.y)* t;


	vec3 nvert(nx,ny,mid.z);

	flatBottomTri.push_back(top);
	flatBottomTri.push_back(mid);
	flatBottomTri.push_back(nvert);

	flatTopTri.push_back(mid);
	flatTopTri.push_back(nvert);
	flatTopTri.push_back(bottom);
}

void NavmeshBuilder::rasterizeFlatBottomTri(const std::vector<glm::vec3>& verts) {

}
void NavmeshBuilder::rasterizeFlatTopTri(const std::vector<glm::vec3>& verts) {

}



std::vector<glm::vec3>  NavmeshBuilder::rasterize(const std::vector<glm::vec3>& verts,const std::vector<uint32_t>&tris,float y ) {
	
	std::vector<glm::vec3> planeVerts;

	vector<vec3> flatBottomTris;
	vector<vec3> bottomTris;


	// 投影
	for (int idx = 0; idx < tris.size(); idx += 3) {
		auto v0 = verts[tris[idx]];
		auto v1 = verts[tris[idx+1]];
		auto v2 = verts[tris[idx+2]];

		divideTri(v0, v1, v2, flatBottomTris, bottomTris);

		//直接投影
		if (v0.y > y && v1.y > y && v2.y > y) {
			v0.y = v1.y = v2.y = y;
			planeVerts.push_back(v0);
			planeVerts.push_back(v1);
			planeVerts.push_back(v2);
		}
		//裁剪
	}
	return planeVerts;
}

NavmeshBuilder::NavmeshBuilder():_tiles(nullptr) {

}


void NavmeshBuilder::setCellBlock(float x, float y) {
	int cellX = (x - _min.x) / _cellSize;
	int cellY = (y - _min.y) / _cellSize;

	int tileX = cellX / _tileSize;
	int tileY = cellY / _tileSize;
	
	if (tileX >= _width || tileY >= _height) {
		printf("overflow (%d,%d) > (%d,%d)",tileX,tileY,_width,_height);
		return;
	}
	int tileIdx = tileX + tileY * _width;

	Tile& tile = _tiles[tileIdx];
	int ix = cellX % _tileSize;
	int iy = cellY % _tileSize;

	tile.setCell(ix, iy);
	
}


void NavmeshBuilder::build(const std::vector<glm::vec3>& planeVerts,Cfg cfg) {

	_max = planeVerts[0];
	_min = _max;
	_tileSize = cfg.tileSize;
	_cellSize = cfg.cellSize;

	for (auto vert : planeVerts) {
		for (int idx = 0; idx < 3; idx++) {
			if (vert[idx] > _max[idx]) {
				_max[idx] = vert[idx];
			}
			else if (vert[idx]< _min[idx]) {
				_min[idx] = vert[idx];
			}
		}
	}
	//

	if (_tiles != nullptr) {
		delete[] _tiles;
	}
	_width = (_max.x - _min.x + 0.5) / (_tileSize*_cellSize);
	_height = (_max.z - _min.z + 0.5) / (_tileSize*_cellSize);

	_tiles = new Tile[_width * _height];
	for (int x = 0; x < _width; x++) {
		for (int y = 0; y < _height; y++) {
			int idx = y * _width + x;
			_tiles->init(x, y, _cellSize);
			//_tiles[idx].x = x;
			//_tiles[idx].y = y;
		}
	}

}
NavmeshBuilder::~NavmeshBuilder() {
	if (_tiles != nullptr) {
		delete[] _tiles;
	}

}

void NavmeshBuilder::init(Cfg cfg) {

}


