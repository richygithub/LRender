/*
 * @Author: libl
 * @Date: 2021-04-08 10:33:39
 * @LastEditors: libl
 * @Description: 
 */
#pragma once

#include "glm\glm.hpp"
#include "delaunay.h"

#include "navmesh.h"

#include <vector>
#include <map>
struct Cell {
	unsigned short block:1;
	unsigned short border:4;  //是否为边界
	unsigned short contourVisited:4;
	unsigned short area : 7;
	Cell() {
		block = 0;
		border = 0;
		contourVisited = 0;
		area = 0;
	}
};



struct Poly {
	int verts[NavMesh_RunTime::PolyVertNum];
	int conn[NavMesh_RunTime::PolyVertNum];
	int id;
	void setId(int tid,int pid) {
		id = NavMesh_RunTime::encodePolyId(tid,pid);
	}
	
};


struct Link {
	int eid;
	int connId;
};

struct Tile {
	unsigned int x;
	unsigned int y;
	int id;
	unsigned int size;
	float minx;
	float miny;
	float cellsize;
	Cell* cells;
	uint16_t maxDist;
	uint16_t* dist;
	uint16_t* region;

	Poly* polys;
	int polyNum;

	std::vector< std::vector<glm::ivec3> > rawCountours;
	std::vector< std::vector<glm::ivec3> > simpleCountours;
	std::vector< std::vector<glm::vec3> > showCountours;

	std::vector<int> tris;
	std::vector<glm::vec3> verts;


	//4方向连接边
	std::vector<Link> links[4];
	Delaunay2d_t del;

	inline int getId()const { return id; };

	void init(int x,int y,int size,float cellsize,float minx,float miny);
	void setCell(int x, int y);
	Tile() :cells(nullptr), dist(nullptr),region(nullptr),polys(nullptr),polyNum(0), del(verts){};

	~Tile();
	void calcDistField(int agentRadius);

	bool isCellConnectedDir(int x,int y,int dir);
	bool isBoader(int x,int y);

	void buildRegion();
	void mergeSmallBlock();
	void buildSimpleRegion();
	void buildContour();
	void buildPolyMesh(bool removeHole=true);

	void addOutlines();
	void removeHoles();
	void connectPoly();
	
	void walkContour(int cx, int cy, int fdir, std::vector<glm::ivec3>& contours);
	void simplifyContour(float maxError);
	void mergeHoles();

	void calcBorder(int minBlock);
private:
	void calcArea();
	void removeSmallBlock(int minBlock);
	std::map<int,int> areaBlockNum;
	std::map<int,std::vector<int>> areaLists;



};


