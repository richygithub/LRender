/*
 * @Author: libl
 * @Date: 2021-04-08 10:33:39
 * @LastEditors: libl
 * @Description: 
 */
#pragma once

#include "glm\glm.hpp"

struct Cell {
	char value;
	Cell() {
		value = 0;
	}
};

struct Tile {
	unsigned int x;
	unsigned int y;
	unsigned int size;
	Cell* cells;
	uint16_t* dist;
	void init(int x,int y,int size);
	void setCell(int x, int y);
	Tile() :cells(nullptr), dist(nullptr){};

	~Tile();
	void calcDistField();

	bool isCellConnectedDir(int x,int y,int dir);

};


