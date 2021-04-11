/*
 * @Author: libl
 * @Date: 2021-04-08 10:33:47
 * @LastEditors: libl
 * @Description: 
 */
#include "tile.h"

Tile::~Tile() {
	if (cells != nullptr) {
		delete[]cells;
	}
	if (dist != nullptr) {
		delete[] dist;
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

	//first pass
	for (int y = 0; y < size;y++) {
		for (int x = 0; x < size; x++) {
			int idx = x + y * size;
			if (cells[idx].value == 1)
				continue;

			if (x == 0 || y == 0 || x == size - 1 || y == size - 1) {
				dist[idx] = 0;
			}
			else {
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

		}
	}


//	delete[]dist;

}
