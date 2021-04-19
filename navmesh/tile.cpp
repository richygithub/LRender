/*
 * @Author: libl
 * @Date: 2021-04-08 10:33:47
 * @LastEditors: libl
 * @Description: 
 */
#include "tile.h"
#include <vector>
#include <stack>
using namespace glm;
using namespace std;

Tile::~Tile() {
	if (cells != nullptr) {
		delete[]cells;
	}
	if (dist != nullptr) {
		delete[] dist;
	}
	if (region != nullptr) {
		delete[] region;
	}
}
	
void Tile::init(int x, int y, int size) {
	if (cells != nullptr) {
		delete[]cells;
	}
	// cells = new Cell[size * size];
	this->x = x;
	this->y = y;
	this->size = size;
}
void Tile::setCell(int x, int y) {
	if (cells == nullptr) {
		cells = new Cell[size * size];
	}
	assert(x >= 0 && y >= 0 && x < size&& y < size);
	int idx = x + y * size;
	cells[idx].block = 1;
}


//0-- left, 1--up, 2--right, 3--down
static const int offset_x[4] = { -1, 0, 1,  0 };
static const int offset_y[4] = {  0, 1, 0, -1 };

bool isBoader(Cell* cells,int size,int x,int y) {
	for (int dir = 0; dir < 4; dir++) {
		int nx = x + offset_x[dir];
		int ny = y + offset_y[dir];
		if (nx < 0 || ny < 0 || nx >= size || ny >= size) {
			return true;
		}
		int cidx = nx + ny * size;
		if (cells == nullptr)
			return false;
		if (cells[cidx].block == 1)
			return true;
	}
	return false;
}

bool Tile::isBoader(int x, int y) {

	for (int dir = 0; dir < 4; dir++) {
		int nx = x + offset_x[dir];
		int ny = y + offset_y[dir];
		if (nx < 0 || ny < 0 || nx >= size || ny >= size) {
			return true;
		}
		int cidx = nx + ny * size;
		if (cells == nullptr)
			return false;
		if (cells[cidx].block == 1)
			return true;
	}
	return false;
}

bool Tile::isCellConnectedDir(int x, int y, int dir) {
	int nx = x + offset_x[dir];
	int ny = y + offset_y[dir];
	if (nx < 0 || nx >= size || ny < 0 || ny >= size)
		return false;
	int idx = nx + ny * size;
	return cells[idx].block == 0;
}


uint16_t* boxBlur(Cell*cells,uint16_t*dist,int size){
	uint16_t* newDist = new uint16_t[size * size];
	memset(newDist, 0, sizeof(uint16_t) * size * size);
	for (int y = 1; y < size-1; ++y)
	{
		for (int x = 1; x < size-1; ++x)
		{
			int idx = x + y * size;
			uint16_t cd = dist[idx];
			if (cd <= 2) {
				//保持边缘
				newDist[idx] = cd;
				continue;
			}

			int d = cd;
			for (int dir = 0; dir < 4; dir++) {

				int nidx = x + offset_x[dir] + (y + offset_y[dir]) * size;
				if (cells[nidx].block == 1) {
					d += cd*2;
				}
				else {
					d += dist[nidx];

					int dir2 = (dir + 1) % 4;
					int nidx2 = x + offset_x[dir2] + (y + offset_y[dir2]) * size;
					if (cells[nidx2].block == 1) {
						d += cd;
					}
					else {
						d += dist[nidx2];
					}

				}

			}
			newDist[idx] = (d + 5) / 9;
		}
	}
	return newDist;

}

void Tile::calcDistField() {
	if (cells == nullptr)
		return;
	dist = new uint16_t[size*size];
	for (int idx = 0; idx < size * size; idx++) {
		dist[idx] = 0xFFFF;
	}
	maxDist = 0;
	//mark border
	for (int y = 0; y < size; y++) {
		for (int x = 0; x < size; x++) {
			int idx = x + y * size;
			if (cells[idx].block == 1)
				continue;
			if (x == 0 || y == 0 || x == size - 1 || y == size - 1) {
				dist[idx] = 0;
			}
			else {
				for (int dir = 0; dir < 4; dir++) {
					int ax = x + offset_x[dir] + (y + offset_y[dir]) * size;
					if (cells[ax].block == 1) {
						dist[idx] = 0;
						break;
					}
				}
			}
		}
	}


	//first pass
	for (int y = 1; y < size-1;y++) {
		for (int x = 1; x < size-1; x++) {
			int idx = x + y * size;
			if (cells[idx].block == 1)
				continue;

/*			if (x == 0 || y == 0 || x == size - 1 || y == size - 1) {
				dist[idx] = 0;
			}
			else */{
				//left
				int left = (x - 1) + y * size;
				if (cells[left].block == 0) {
					if (dist[left] + 2 < dist[idx]) {
						dist[idx] = dist[left] + 2;
					}
					//left down
					int ld = (x - 1) + (y - 1) * size;
					if (cells[ld].block == 0) {
						if (dist[ld] + 3 < dist[idx]) {
							dist[idx] = dist[ld] + 3;
						}
					}

				}

				//down
				int down = x + (y - 1) * size;
				if (cells[down].block == 0) {
					if (dist[down] + 2 < dist[idx]) {
						dist[idx] = dist[down] + 2;
					}
					//down right
					int rd = (x + 1) + (y - 1) * size;
					if (cells[rd].block == 0) {
						if (dist[rd] + 3 < dist[idx]) {
							dist[idx] = dist[rd] + 3;
						}
					}
				}
			}
		}
	}
	//seconde pass

	for (int y = size - 2; y > 0; y--) {
		for (int x = size - 2; x > 0; x--) {
			int idx = x + y * size;
			if (cells[idx].block == 1)
				continue;
			//right
			int right = x + 1 + y * size;

			if (cells[right].block == 0) {
				if (dist[right] + 2 < dist[idx]) {
					dist[idx] = dist[right] + 2;
				}
				//right up
				int rd = (x + 1) + (y + 1) * size;
				if (cells[rd].block == 0) {
					if (dist[rd] + 3 < dist[idx]) {
						dist[idx] = dist[rd] + 3;
					}
				}

			}

			//up
			int up = x + (y + 1) * size;
			if (cells[up].block == 0) {
				if (dist[up] + 2 < dist[idx]) {
					dist[idx] = dist[up] + 2;
				}
				//up left 
				int ul = (x - 1) + (y + 1) * size;
				if (cells[ul].block == 0) {
					if (dist[ul] + 3 < dist[idx]) {
						dist[idx] = dist[ul] + 3;
					}
				}
			}
			if (dist[idx] > maxDist) {
				maxDist = dist[idx];
			}

		}
	}

	uint16_t* ndist = boxBlur(cells, dist, size);
	delete[]dist;
	dist = ndist;
}

struct Element {
	int x;
	int y;
	bool used;
	Element(int x, int y) :x(x), y(y), used(false){}
};

void expandRegion(Cell* cells, uint16_t* dist, uint16_t* region, int size, uint16_t regionId, vector<Element>& eles) {

	const int maxIter = 8;
	int iter = 0;
	while (++iter<=maxIter) {
		int count = 0;
		for (auto& ele : eles) {
			//int idx = ele.x + ele.y * size;
			if (ele.used) {
				count++;
				continue;
			}

			int cidx = ele.x + ele.y* size;
	
			uint16_t rid = 0;
			uint16_t d = 0xFFFF;
			for (int dir = 0; dir < 4; dir++) {
				int idx = ele.x + offset_x[dir] + (ele.y + offset_y[dir] )* size;
				if (cells[idx].block == 1 || region[idx] != 0) {
					if (dist[idx] + 2 < d) {
						d = dist[idx] + 2;
						rid = region[idx];
					}
				}	
			}
			if (rid != 0) {
				ele.used = true;
				dist[cidx] = d;
				region[cidx] = rid;
			}
			else {
				count++;
			}
		}

		if (count == eles.size()) {
			break;
		}
	}
}

bool floodRegion(Element st,Tile* tile, uint16_t* dist, uint16_t* region, int size, uint16_t regionId,int level) {
	stack<Element> eles;
	eles.push(st);
	region[st.x + st.y * size] = regionId;
	dist[st.x + st.y * size] = 0;
	int count = 0;
	while (eles.size() > 0) {

		Element ele = eles.top();
		eles.pop();
		//

		int nr = 0;
		for (int dir = 0; dir < 4; dir++) {

			int nx = offset_x[dir] + ele.x;
			int ny = offset_y[dir] + ele.y;
			int nidx = nx + ny * size;


			if (isBoader(tile->cells, size, nx, ny)) {
				continue;
			}
			if (region[nidx] != 0 && region[nidx] != regionId) {
				nr = region[nidx];
				break;
			}
			//8方向

			int ax = nx + offset_x[(dir + 1) % 4];
			int ay = ny + offset_y[(dir + 1) % 4];
			int aidx = ax + ay * size;
			if (region[aidx] != 0 && region[aidx]!=region[regionId] ) {
				nr = region[nidx];
				break;
			}
		}
		if (nr != 0) {
			//边界
			//region[st.x + st.y * size] = 0xffff;
			region[st.x + st.y * size] = 0;


			continue;
		}

		count++;
		//四邻域
		for (int dir = 0; dir < 4; dir++) {
			int nx = offset_x[dir] + ele.x;
			int ny = offset_y[dir] + ele.y;
			int nidx = nx + ny * size;

			if (!isBoader(tile->cells, size, nx, ny)) {
				//add
				if (tile->dist[nidx] >= level && region[nidx] == 0) {
					//need 

					region[nidx] = regionId;
					dist[nidx] = 0;
					eles.push(Element(nx,ny));

				}
			}
		}
	}
	//?
	return count > 0;

}


void mergeSmallRegion(Tile& tile) {
	for (int y = 1; y < tile.size-1; y++) {
		for (int x = 1; x < tile.size-1; x++) {
			uint16_t rid = tile.region[x + y * tile.size];
			if (rid == 0)continue;
			for (int dir = 0; dir <= 1; dir++) {
				uint16_t r1 = tile.region[x + offset_x[dir] + (y + offset_y[dir]) * tile.size];
				uint16_t r2 = tile.region[x + offset_x[dir+2] + (y + offset_y[dir+2]) * tile.size];
				if (r1 != 0 && r2 != 0 && r1 != rid && r2 != rid) {
					uint16_t nr = r1 < r2 ? r1 : r2;
					tile.region[x + y * tile.size] = nr;
				}
			}
		}
	}
}
void Tile::buildContour() {
	if (cells == nullptr) {
		//todo -- 四边形
		return;
	}
	for (int y = 0; y < size; y++) {
		for (int x = 0; x < size; x++) {
			int cidx = x + y * size;
			if (cells[cidx].border != 0 && cells[cidx].contourVisited == 0) {
				//
				walkContour(x,y);

			}
		}

	}

}
void Tile::walkContour(int cx,int cy) {

	int fdir = 1;

	vector<vec3> contours;


	int idx= cx + cy * size;
	cells[idx].contourVisited = 1;

	for (int sdir = fdir-1; sdir < 4; sdir++) {
		if ((cells[idx].border & (1 << sdir)) != 0) {
			//


		}
	}



	


	

}

void Tile::buildSimpleRegion() {
	if (cells == nullptr)
		return;

	if (region == nullptr) {
		region = new uint16_t[size * size];
		memset(region, 0, sizeof(uint16_t) * size * size);
	}	
}


void Tile::calcBorder() {
	if (cells == nullptr)
		return;
	for (int y = 0; y < size; y++) {
		for (int x = 0; x < size; x++) {
			int cidx = x + y * size;
			if (cells[cidx].block == 1)
				continue;
			for (int dir = 0; dir < 4; dir++) {
				int nx = x + offset_x[dir];
				int ny = y + offset_y[dir];
				if (  (nx <0 || ny <0 || nx >= size || ny >= size)  ||
					cells[nx+ny*size].block == 1
					) 
				{
					cells[cidx].border |= (1 << dir);
				}
			}
		}
	}

}

void Tile::buildRegion() {
	if (cells == nullptr)
		return;
	if (region == nullptr) {
		region = new uint16_t[size * size];
		memset(region, 0, sizeof(uint16_t) * size * size);
	}
	uint16_t* tmpdist = new uint16_t[size * size];
	//分层
	memset(tmpdist, 0, sizeof(uint16_t) * size * size);

	vector < vector<Element> > lvVectors;
	lvVectors.reserve( maxDist/2 +1 );
	for (int idx = 0; idx < (maxDist / 2 + 1); idx++) {
		lvVectors.push_back(vector<Element>());
	}

	for(int y=0;y<size;y++){
		for (int x = 0; x < size; x++) {
			//
			int idx = x + y * size;
			if (cells[idx].block == 0) {
				int lv = dist[idx] / 2;
				lvVectors[lv].push_back(Element(x,y));
			}
		}
	}
	
	//
	int level = maxDist / 2;
	int regionId = 1;
	while (level > 0) {
		if (level < maxDist / 2) {
			//copy
			auto& preLvs = lvVectors[level + 1];
			for (int idx = 0; idx < preLvs.size(); idx++) {
				int cidx = preLvs[idx].x + preLvs[idx].y * size;
				if (region[cidx] == 0) {
					lvVectors[level].push_back(preLvs[idx]);
				}
			}

		}
		//expand region
		expandRegion(cells, tmpdist, region, size, regionId, lvVectors[level]);

		for (auto& ele : lvVectors[level]) {
			//flood
			int idx = ele.x + ele.y * size;
			if (region[idx] == 0) {
				if (floodRegion(ele, this, tmpdist, region, size, regionId,level*2 )) {
					regionId++;
				}
			}
		

		}

		level--;

	}


	mergeSmallRegion(*this);


	delete[] tmpdist;
}

