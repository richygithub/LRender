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

NavmeshBuilder::NavmeshBuilder():_tiles(nullptr), _rtData(nullptr){

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
void NavmeshBuilder::clearGround() {
	_max = vec3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
	_min = vec3(FLT_MAX, FLT_MAX, FLT_MAX);
}
void NavmeshBuilder::addGround(const std::vector<glm::vec3>& verts) {
	for (auto& vert : verts) {
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
				printf("----------(%d %d)-----------",x,y);
				for (int cy = 0; cy < tile.size; cy++) {
					for (int cx = 0; cx < tile.size; cx++) {
						if (tile.cells[cx + cy * tile.size].block == 0) {
								//push
								//printf("cell (%d,%d) at tile(%d,%d)\n", cx, cy, x, y);
								printf("%2d ", tile.dist[cx + cy * tile.size]);
							}
							else {
								printf("-1");
						}
					}
					printf("\n");
				}
				printf("\n");
	
			}
		}
	}


}

void NavmeshBuilder::seralize() {

	if (_rtData != nullptr) {
		delete _rtData;
	}
	_rtData = new NavMesh_RunTime::NavMesh();

	_rtData->_cellSize = _cellSize;
	_rtData->_tileSize = _tileSize;
	_rtData->_width = _width;
	_rtData->_height = _height;
	_rtData->_min = NavMesh_RunTime::Vec3(_min.x,_min.y,_min.z);
	_rtData->_max = NavMesh_RunTime::Vec3(_max.x,_max.y,_max.z);


	_rtData->_tiles = new NavMesh_RunTime::MeshTile[_width * _height];
	for (int idx = 0; idx < _width * _height; idx++) {

		auto& mt = _rtData->_tiles[idx];
		auto& t = _tiles[idx];
		int vtNum = t.verts.size();
		if (vtNum > 0) {
			mt._vertNum = vtNum;
			mt._verts = new NavMesh_RunTime::Vec3[vtNum];
			vec3 offset(t.minx,0,t.miny );
			for (int count = 0; count < vtNum; count++) {
				vec3 realPos = t.verts[count] * t.cellsize + offset;
				
				mt._verts[count].x = realPos.x;
				mt._verts[count].y = realPos.y;
				mt._verts[count].z = realPos.z;
				//= NavMesh_RunTime::Vec3(t.verts[idx].x, t.verts[idx].y, t.verts[idx].z);
			}
		}
		if (t.polyNum > 0) {
			mt._polyNum = t.polyNum;
			mt._polys = new NavMesh_RunTime::MeshPoly[t.polyNum];
			for (int count = 0; count < mt._polyNum; count++) {
				mt._polys[count].id = t.polys[count].id;
				for (int vCount = 0; vCount < 3; vCount++) {
					mt._polys[count].verts[vCount] = t.polys[count].verts[vCount];
					mt._polys[count].conn[vCount] = t.polys[count].conn[vCount];
				}
			}
		}
	}

	_query.init(_rtData,2048);


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
	
	for (int y = 0; y < _height; y++) {
		for (int x = 0; x < _width; x++) {

			int idx = y * _width + x;

			float minx = _min.x + x * _tileSize * _cellSize;
			float miny = _min.z + y * _tileSize * _cellSize;


			_tiles[idx].init(x, y, _tileSize,_cellSize,minx,miny);
			_tiles[idx].id = idx;
		}
	}
	//设置边界
	for (int y = 0; y < _height; y++) {

		int idx = y * _width + _width - 1;
		int mx = (_max.x - _min.x) / _cellSize - (_width - 1) * _tileSize;
		
		for (int cy = 0; cy < _tileSize; cy++) {
			for (int cx = mx; cx < _tileSize; cx++) {
				_tiles[idx].setCell(cx, cy);
			}
		}

	}

	for (int x = 0; x < _width; x++) {

		int idx = (_height - 1) * _width + x;
		int my = (_max.z - _min.z) / _cellSize - (_height - 1) * _tileSize;
	
		for (int cy = my; cy < _tileSize; cy++) {
			for (int cx = 0; cx < _tileSize; cx++) {
				_tiles[idx].setCell(cx, cy);
			}
		}

	}


	_debug.clear();
	for (int idx = 0; idx < _objVerts.size(); idx++) {

		auto vs = rasterize(_objVerts[idx], _objTris[idx], _max.y );
		_debug.push_back(vs);
	}
	//calc dist field
	for (int x = 0; x < _width; x++) {
		for (int y = 0; y < _height; y++) {
			int idx = y * _width + x;
			_tiles[idx].calcDistField(cfg.agentRadius);
			_tiles[idx].calcBorder(cfg.minBlock);
			//_tiles[idx].buildRegion();
			_tiles[idx].buildContour();
			_tiles[idx].simplifyContour(cfg.lineError);

			//_tiles[idx].buildPolyMesh(cfg.removeHoles);
		}
	}




}

void NavmeshBuilder::debug_addOutline(Cfg cfg) {
	for (int x = 0; x < _width; x++) {
		for (int y = 0; y < _height; y++) {
			int idx = y * _width + x;
			_tiles[idx].addOutlines();
		}
	}
}
void NavmeshBuilder::debug_removeHole(Cfg cfg) {
	for (int x = 0; x < _width; x++) {
		for (int y = 0; y < _height; y++) {
			int idx = y * _width + x;
			_tiles[idx].removeHoles();
		}
	}

	for (int x = 0; x < _width; x++) {
		for (int y = 0; y < _height; y++) {
			int idx = y * _width + x;
			_tiles[idx].connectPoly();
		}
	}

	for (int x = 0; x < _width; x++) {
		for (int y = 0; y < _height; y++) {
			int idx = y * _width + x;
			linkTile(_tiles[idx]);
		}
	}

	seralize();
}
void NavmeshBuilder::build_Tri(Cfg cfg) {
	//calc dist field
	for (int x = 0; x < _width; x++) {
		for (int y = 0; y < _height; y++) {
			int idx = y * _width + x;
			_tiles[idx].buildPolyMesh(cfg.removeHoles);
		}
	}

	//linkTile

}
void connectTile(Tile& src,Tile& dst,int dir) {

	for (auto& se : src.links[dir]) {

		for (auto& de : dst.links[(dir + 2) % 4]) {

			auto& se0 = src.del.edges[se.eid];
			auto& se1 = src.del.edges [ src.del.edges[se.eid].twin];

			auto& sp0 = src.verts[se0.orig];
			auto& sp1 = src.verts[se1.orig];

			auto& de0 = dst.del.edges[de.eid];
			auto& de1 = dst.del.edges [ dst.del.edges[de.eid].twin];

			auto& dp0 = dst.verts[de0.orig];
			auto& dp1 = dst.verts[de1.orig];

			bool connected = false;
			if (dir == 0) {
				connected = (sp1.z >= dp0.z && sp1.z < dp1.z) || (sp1.z <= dp0.z && sp0.z > dp0.z);
			}
			else if (dir == 1) {
				connected = (sp1.x >= dp0.x && sp1.x < dp1.x) || (sp1.x <= dp0.x && sp0.x > dp0.x);
			}
			else if (dir == 2) {
				//connected = (sp1.z > dp1.z && sp1.z < dp0.z) || (dp1.z > sp0.z && dp1.z < sp1.z);
				connected = (sp1.z >= dp0.z && sp0.z < dp0.z) || (sp1.z <= dp0.z && sp1.z > dp1.z);
	
			}
			else if (dir == 3) {
				connected = (sp1.x >= dp0.x && sp0.x < dp0.x) || (sp1.x <= dp0.x && sp1.x > dp1.x);
			}
			if (connected) {
				src.polys[se0.face].conn[se.connId] = dst.polys[de0.face].id;
				dst.polys[de0.face].conn[de.connId] = src.polys[se0.face].id;
				continue;
			}
		}

	}

}
//0-- left, 1--up, 2--right, 3--down
static const int offset_x[4] = { -1, 0, 1,  0 };
static const int offset_y[4] = { 0, 1, 0, -1 };
void NavmeshBuilder::linkTile(Tile& tile) {

	for (int dir = 0; dir < 4; dir++) {
		int x = offset_x[dir] + tile.x;
		int y = offset_y[dir] + tile.y;
		if (x >= 0 && x < _width && y >= 0 && y < _height) {
			Tile& dst = _tiles[y * _width + x];
			connectTile(tile, dst, dir);
		}
	}
}

NavmeshBuilder::~NavmeshBuilder() {
	if (_tiles != nullptr) {
		delete[] _tiles;
		_tiles = nullptr;
	}
	if (_rtData != nullptr) {
		delete _rtData;
		_rtData = nullptr;
	}


}

void NavmeshBuilder::init(Cfg cfg) {

}


