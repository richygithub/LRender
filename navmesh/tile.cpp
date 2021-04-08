/*
 * @Author: libl
 * @Date: 2021-04-08 10:33:47
 * @LastEditors: libl
 * @Description: 
 */
#include "tile.h"

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
	

