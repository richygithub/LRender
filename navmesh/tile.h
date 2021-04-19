/*
 * @Author: libl
 * @Date: 2021-04-08 10:33:39
 * @LastEditors: libl
 * @Description: 
 */
#pragma once

#include "glm\glm.hpp"

struct Cell {
	unsigned char block:1;
	unsigned char border:4;  //�Ƿ�Ϊ�߽�
	unsigned char contourVisited:1;

	Cell() {
		block = 0;
		border = 0;
		contourVisited = 0;
	}
};

struct Tile {
	unsigned int x;
	unsigned int y;
	unsigned int size;
	float minx;
	float miny;
	float cellsize;
	Cell* cells;
	uint16_t maxDist;
	uint16_t* dist;
	uint16_t* region;


	void init(int x,int y,int size);
	void setCell(int x, int y);
	Tile() :cells(nullptr), dist(nullptr),region(nullptr){};

	~Tile();
	void calcDistField();

	bool isCellConnectedDir(int x,int y,int dir);
	bool isBoader(int x,int y);

	void buildRegion();
	void buildSimpleRegion();
	void buildContour();
	
	void walkContour(int x,int y);
	void calcBorder();


};


