/*
 * @Author: libl
 * @Date: 2021-04-08 10:33:47
 * @LastEditors: libl
 * @Description: 
 */
#include "tile.h"
#include <vector>
#include <stack>
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
	cells[idx].value = 1;
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
		if (cells[cidx].value == 1)
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
		if (cells[cidx].value == 1)
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
	return cells[idx].value == 0;
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
			if (cells[idx].value == 1)
				continue;
			if (x == 0 || y == 0 || x == size - 1 || y == size - 1) {
				dist[idx] = 0;
			}
			else {
				for (int dir = 0; dir < 4; dir++) {
					int ax = x + offset_x[dir] + (y + offset_y[dir]) * size;
					if (cells[ax].value == 1) {
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
			if (cells[idx].value == 1)
				continue;

/*			if (x == 0 || y == 0 || x == size - 1 || y == size - 1) {
				dist[idx] = 0;
			}
			else */{
				//left
				int left = (x - 1) + y * size;
				if (cells[left].value == 0) {
					if (dist[left] + 2 < dist[idx]) {
						dist[idx] = dist[left] + 2;
					}
					//left down
					int ld = (x - 1) + (y - 1) * size;
					if (cells[ld].value == 0) {
						if (dist[ld] + 3 < dist[idx]) {
							dist[idx] = dist[ld] + 3;
						}
					}

				}

				//down
				int down = x + (y - 1) * size;
				if (cells[down].value == 0) {
					if (dist[down] + 2 < dist[idx]) {
						dist[idx] = dist[down] + 2;
					}
					//down right
					int rd = (x + 1) + (y - 1) * size;
					if (cells[rd].value == 0) {
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
			if (cells[idx].value == 1)
				continue;
			//right
			int right = x + 1 + y * size;

			if (cells[right].value == 0) {
				if (dist[right] + 2 < dist[idx]) {
					dist[idx] = dist[right] + 2;
				}
				//right up
				int rd = (x + 1) + (y + 1) * size;
				if (cells[rd].value == 0) {
					if (dist[rd] + 3 < dist[idx]) {
						dist[idx] = dist[rd] + 3;
					}
				}

			}

			//up
			int up = x + (y + 1) * size;
			if (cells[up].value == 0) {
				if (dist[up] + 2 < dist[idx]) {
					dist[idx] = dist[up] + 2;
				}
				//up left 
				int ul = (x - 1) + (y + 1) * size;
				if (cells[ul].value == 0) {
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


//	delete[]dist;

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
				if (cells[idx].value == 1 || region[idx] != 0) {
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

void floodRegion(Element st,Tile* tile, uint16_t* dist, uint16_t* region, int size, uint16_t regionId,int level) {
	stack<Element> eles;
	eles.push(st);
	region[st.x + st.y * size] = regionId;
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
			region[st.x + st.y * size] = 0;
			continue;
		}

		//四邻域
		for (int dir = 0; dir < 4; dir++) {
			int nx = offset_x[dir] + ele.x;
			int ny = offset_y[dir] + ele.y;
			int nidx = nx + ny * size;

			if (!isBoader(tile->cells, size, nx, ny)) {
				//add
				if (tile->dist[nidx] >= level && region[nidx] == 0) {
					//need 

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

	for(int y=0;y<size;y++){
		for (int x = 0; x < size; x++) {
			//
			int idx = x + y * size;
			if (cells[idx].value == 0) {
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


		}

	}




	delete[] dist;
}

