/*
 * @Author: libl
 * @Date: 2021-05-07 10:50:01
 * @LastEditors: libl
 * @Description: 
 */
#pragma once
#include "navmesh-common.h"

namespace NavMesh_RunTime {

	const int PolyVertNum = 3;
	const int POLY_BIT = 16;
	const int POLY_MASK = 0xFFFF;


	inline short extractPoly(unsigned int pid, int& tid) {
		tid = pid >> (32 - POLY_BIT);
		return pid & POLY_MASK;
	}


	inline unsigned int encodePolyId(short tid, short pid) {
		return (tid << POLY_BIT) | (pid & POLY_MASK);
	}
	inline void decodePolyId(unsigned int polyId,short& pid, short& tid) {
		tid = polyId >> (32 - POLY_BIT);
		pid =  polyId & POLY_MASK;
	}

	struct MeshPoly {
		int verts[PolyVertNum];
		unsigned int conn[PolyVertNum];
		unsigned int id;
		//void setId(int tid, int pid) {
		//	id = getPolyId(tid, pid);
		//}

	};

	struct MeshTile {
		Vec3* _verts;
		unsigned short _vertNum;

		MeshPoly* _polys;
		unsigned short _polyNum;

		int findNearestPoint(Vec3 point, Vec3& dst)const;
		MeshTile();
		~MeshTile();
	};


	struct NavMesh {

		MeshTile* _tiles;
		int _width;
		int _height;

		float _cellSize;
		int _tileSize;
		Vec3 _max;
		Vec3 _min;
		//glm::vec3 _max;
		//glm::vec3 _min;

		void findTile(Vec3 point,int&x,int& y)const;
	
		int findPoly(Vec3 point,Vec3& location)const;
		bool extractPolyTile(int polyId, const MeshTile** tile, const MeshPoly** poly)const;
		void load(const char* bytes, int len);
		NavMesh() :_tiles(nullptr) {};
		~NavMesh();


	};

	struct NavMeshQuery {
		const NavMesh* _meshData;
		class dtNodePool* _nodePool;		///< Pointer to node pool.
		class dtNodeQueue* _openList;		///< Pointer to open list queue.

		NavMeshQuery() :_meshData(nullptr),_nodePool(nullptr),_openList(nullptr){}
		~NavMeshQuery();
		void init(const NavMesh* pdata, const int maxNodes);

		
		dtStatus findPath(Vec3 start,Vec3 end, Vec3* pointPath, const int maxPoint, int& pointCount,
			unsigned int* polyPath, int maxPathCount, int* pathCount
			);


		dtStatus findPathPoint(Vec3 start,Vec3 end,Vec3* pointPath,const int maxPoint,int& pointCount);


	private:

		dtStatus getPath(struct dtNode* endNode,unsigned int* polyPath, int maxPathCount, int& pathCount );
		dtStatus getPathPoint(Vec3 start, Vec3 end, const struct dtNode* endNode, Vec3* pointPath, const int maxPoint, int& pointCount);
		bool getPortalPoint(const struct dtNode* node,Vec3& left,Vec3& right);


	};
}


