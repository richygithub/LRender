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

	if (verts.size() < 3) {
		return;
	}

	auto v0 = verts[0];
	auto v1 = verts[1];
	auto v2 = verts[2];
	if (v1.x > v2.x) {
		swap(v1, v2);
	}
	ivec2 v0Cell = getCellXY(v0.x,v0.z);
	ivec2 v1Cell = getCellXY(v1.x,v1.z);
	ivec2 v2Cell = getCellXY(v2.x,v2.z);
	float step1x = (v0.x - v1.x) / (v0.z - v1.z) * _cellSize;
	float step2x = (v0.x - v2.x) / (v0.z - v2.z) * _cellSize;


	for (int y = v1Cell.y; y <= v0Cell.y; y++) {

		for (int x = v1Cell.x; x <= v2Cell.x; x++) {
			setCellBlock(x, y);
		}
		//update v1Cell,v2Cell
		v1.x += step1x;
		v1.z += _cellSize;
		v2.x += step2x;
		v2.z += _cellSize;

		v1Cell = getCellXY(v1.x,v1.z);
		v2Cell = getCellXY(v2.x,v2.z);
	
	}

}
void NavmeshBuilder::rasterizeFlatTopTri(const std::vector<glm::vec3>& verts) {
	if (verts.size() < 3) {
		return;
	}

	auto v0 = verts[0];
	auto v1 = verts[1];
	auto v2 = verts[2];
	if (v0.x > v1.x) {
		swap(v0, v1);
	}
	ivec2 v0Cell = getCellXY(v0.x, v0.z);
	ivec2 v1Cell = getCellXY(v1.x, v1.z);
	ivec2 v2Cell = getCellXY(v2.x, v2.z);
	float step1x = (v2.x - v0.x) / (v2.z - v1.z) * (-_cellSize);
	float step2x = (v2.x - v1.x) / (v2.z - v1.z) * (-_cellSize);


	for (int y = v0Cell.y; y >= v2Cell.y; y--) {

		for (int x = v0Cell.x; x <= v1Cell.x; x++) {
			setCellBlock(x, y);
		}
		//update v1Cell,v2Cell
		v0.x += step1x;
		v0.z += _cellSize;
		v1.x += step2x;
		v1.z += _cellSize;

		v0Cell = getCellXY(v0.x, v0.z);
		v1Cell = getCellXY(v1.x, v1.z);

	}
}



std::vector<glm::vec3>  NavmeshBuilder::rasterize(const std::vector<glm::vec3>& verts,const std::vector<uint32_t>&tris,float y ) {
	
	std::vector<glm::vec3> planeVerts;

	vector<vec3> flatBottomTris;
	vector<vec3> flatTopTris;


	// 投影
	for (int idx = 0; idx < tris.size(); idx += 3) {
		auto v0 = verts[tris[idx]];
		auto v1 = verts[tris[idx+1]];
		auto v2 = verts[tris[idx+2]];

		//直接投影
		if (v0.y > y && v1.y > y && v2.y > y) {
			v0.y = v1.y = v2.y = y;
			planeVerts.push_back(v0);
			planeVerts.push_back(v1);
			planeVerts.push_back(v2);


			divideTri(v0, v1, v2, flatBottomTris, flatTopTris);
			rasterizeFlatBottomTri(flatBottomTris);
			rasterizeFlatTopTri(flatTopTris);
		}
		//裁剪



	}
	return planeVerts;
}

NavmeshBuilder::NavmeshBuilder():_tiles(nullptr) {

}


glm::ivec2 NavmeshBuilder::getCellXY(float x, float y) {
	int cellX = (x - _min.x) / _cellSize;
	int cellY = (y - _min.z) / _cellSize;
	return ivec2(cellX, cellY);
}

void NavmeshBuilder::setCellBlock(int cellX, int cellY) {
	int tileX = cellX / _tileSize;
	int tileY = cellY / _tileSize;

	if (tileX >= _width || tileY >= _height) {
		printf("overflow (%d,%d) > (%d,%d)", tileX, tileY, _width, _height);
		return;
	}
	int tileIdx = tileX + tileY * _width;

	Tile& tile = _tiles[tileIdx];
	int ix = cellX % _tileSize;
	int iy = cellY % _tileSize;

	tile.setCell(ix, iy);
}
void NavmeshBuilder::setCellBlock(float x, float y) {
	int cellX = (x - _min.x) / _cellSize;
	int cellY = (y - _min.z) / _cellSize;

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

void NavmeshBuilder::setPlane(const std::vector<glm::vec3>& verts) {
	_planes = verts;
	_max = _planes[0];
	_min = _max;
	for (auto vert : _planes) {
		for (int idx = 0; idx < 3; idx++) {
			if (vert[idx] > _max[idx]) {
				_max[idx] = vert[idx];
			}
			else if (vert[idx] < _min[idx]) {
				_min[idx] = vert[idx];
			}
		}
	}
}

void NavmeshBuilder::debug() {
	for (int x = 0; x < gMeshBuilder._width; x++) {
		for (int y = 0; y < gMeshBuilder._height; y++) {
			auto& tile = gMeshBuilder._tiles[x + y * gMeshBuilder._width];
			if (tile.cells != nullptr) {

				for (int cx = 0; cx < tile.size; cx++) {
					for (int cy = 0; cy < tile.size; cy++) {
						if (tile.cells[cx + cy * tile.size].value == 1) {
							//push
							printf("cell (%d,%d) at tile(%d,%d)\n", cx, cy, x, y);

						}
					}
				}
			}
		}
	}


}

void NavmeshBuilder::build(	Cfg cfg) {
	_tileSize = cfg.tileSize;
	_cellSize = cfg.cellSize;


	if (_tiles != nullptr) {
		delete[] _tiles;
	}
	_width = ceil(  (_max.x - _min.x ) / (_tileSize * _cellSize) );
	_height = ceil( (_max.z - _min.z ) / (_tileSize * _cellSize) );

	_tiles = new Tile[_width * _height];
	for (int x = 0; x < _width; x++) {
		for (int y = 0; y < _height; y++) {
			int idx = y * _width + x;
			_tiles[idx].init(x, y, _tileSize);
		}
	}

	_debug.clear();
	for (int idx = 0; idx < _objVerts.size(); idx++) {

		auto vs = rasterize(_objVerts[idx], _objTris[idx], _max.y );
		_debug.push_back(vs);

	}

}
void build(const std::vector<glm::vec3>& planeVerts,Cfg cfg) {




}
NavmeshBuilder::~NavmeshBuilder() {
	if (_tiles != nullptr) {
		delete[] _tiles;
	}

}

void NavmeshBuilder::init(Cfg cfg) {

}


