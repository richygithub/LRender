/*
 * @Author: libl
 * @Date: 2021-05-07 10:50:13
 * @LastEditors: libl
 * @Description: 
 */

#include "DetourNode.h"
#include "navmesh.h"

namespace NavMesh_RunTime {

#pragma region MeshTile
	//to do -- bvh优化
	int MeshTile::findNearestPoint(Vec3 point, Vec3& dst)const {
		//遍历
		for (int idx = 0; idx < _polyNum; idx++) {

			int p0 = _polys[idx].verts[0];
			int p1 = _polys[idx].verts[1];
			int p2 = _polys[idx].verts[2];

			if (inTriangle(_verts[p0], _verts[p1], _verts[p2], point)) {
				dst = point;
				return _polys[idx].id;
			}

		}
		return INVALID_ID;
	}

	MeshTile::MeshTile():
		_verts(nullptr),_vertNum(0),
		_polys(nullptr),_polyNum(0)
	{

	}
	MeshTile::~MeshTile() {
		if (_verts != nullptr) {
			delete[] _verts;
		}
		if (_polys != nullptr) {
			delete[] _polys;
		}
	}
#pragma endregion

#pragma region NavMesh
	NavMesh::~NavMesh() {
		if (_tiles != nullptr) {
			delete[] _tiles;
			_tiles = nullptr;
		}
	}

	
	int NavMesh::findPoly(Vec3 point,Vec3& location)const {
		int tileX = (point.x - _min.x) / (_tileSize * _cellSize);
		int tileZ = (point.z - _min.z) / (_tileSize * _cellSize);
		if (tileX < 0 || tileX >= _width || tileZ < 0 || tileZ >= _height) {
			return INVALID_ID;
		}
		int idx = tileZ * _width + tileX;
		MeshTile& mt = _tiles[idx];
		return mt.findNearestPoint(point, location);
		

	}
	bool NavMesh::extractPolyTile(int polyId,const MeshTile** tile,const MeshPoly** poly)const {
		short pid, tid;
		decodePolyId(polyId, pid, tid);
		if (tid >= 0 && tid < _width * _height) {
			*tile = &_tiles[tid];
			if (pid >= 0 && pid < (*tile)->_polyNum) {
				*poly = &(*tile)->_polys[pid];
				return true;
			}
		}
		return false;
	}
#pragma endregion

#pragma region NavMeshQuery
	NavMeshQuery::~NavMeshQuery() {

	}

	void NavMeshQuery::init(const NavMesh* pdata, const int maxNodes) {
		_meshData = pdata;
		if (_nodePool == nullptr || _nodePool->getMaxNodes() < maxNodes) {
			if (_nodePool != nullptr) {
				_nodePool->~dtNodePool();
				delete(_nodePool);
				_nodePool = nullptr;
			}
			_nodePool = new dtNodePool(maxNodes, dtNextPow2(maxNodes / 4));
		}
		else {
			_nodePool->clear();
		}


		if (_openList == nullptr || _openList->getCapacity() < maxNodes)
		{
			if (_openList != nullptr)
			{
				_openList->~dtNodeQueue();
				delete(_openList);
				_openList = nullptr;
			}
			_openList = new dtNodeQueue(maxNodes);
		}
		else
		{
			_openList->clear();
		}


	}
	Vec3 getEdgeMidPoint(const MeshTile* tile,const MeshPoly* poly,int edgeId) {
		int p0 = poly->verts[edgeId];
		int p1 = poly->verts[edgeId % PolyVertNum];
		return (tile->_verts[p0] + tile->_verts[p1])*0.5f;

	}
	dtStatus NavMeshQuery::getPath(dtNode* endNode, unsigned int* polyPath,int maxPathCount, int& pathCount) {


		dtStatus s = DT_SUCCESS;
		while (endNode->hops >= maxPathCount) {
			endNode = _nodePool->getNodeAtIdx(endNode->pidx);	
			s |= DT_BUFFER_TOO_SMALL;
		}

		int count = 0;
		while (endNode ) {
			polyPath[count] = endNode->id;
			endNode = _nodePool->getNodeAtIdx(endNode->pidx);
			count++;
		}
		pathCount = count;
		return s;
	}
	dtStatus NavMeshQuery::findPath(Vec3 start, Vec3 end,unsigned int* polyPath,int maxPath,int& pathCount) {
		Vec3 sp, ep;
		int startPolyId, endPolyId;
		if ((startPolyId = _meshData->findPoly(start, sp)) == INVALID_ID)
			return 0;

		if ((endPolyId = _meshData->findPoly(end, ep)) == INVALID_ID)
			return 0;
		//
		const MeshTile* startTile, endTile;
		const MeshPoly* startPoly, endPoly;
		if (!_meshData->extractPolyTile(startPolyId, &startTile, &startPoly) ){
			return 0;	
		}
		//A*
		_nodePool->clear();
		_openList->clear();


		dtNode* stNode = _nodePool->getNode(startPolyId);
		stNode->pos = sp;
		stNode->id = startPolyId;
		stNode->cost = 0;
		stNode->total = dtVdist2D(sp, ep);

		_openList->push(stNode);

		dtNode* lastNode = stNode;

		const MeshTile* tile = nullptr;
		const MeshPoly* poly = nullptr;

		bool outOfNode = false;
		dtStatus ret =0;
		while (!_openList->empty()) {

			dtNode* bestNode = _openList->pop();
			bestNode->flags &= ~DT_NODE_OPEN;
			bestNode->flags |= DT_NODE_CLOSED;

			if (bestNode->id == endPolyId) {
				//输出
				lastNode = bestNode;
				break;
			}

			if (!_meshData->extractPolyTile(bestNode->id, &tile, &poly)) {
				//不应该出现	
				break;
			}

			// Get parent poly and tile.
			unsigned int parentRef = INVALID_ID;
/*			const MeshTile* parentTile = 0;
			const MeshPoly* parentPoly = 0;
	*/		
			if (bestNode->pidx)
				parentRef = _nodePool->getNodeAtIdx(bestNode->pidx)->id;
			//if (parentRef)
			//	_meshData->extractPolyTile(parentRef, &parentTile, &parentPoly);

			for (int idx = 0; idx < PolyVertNum; idx++) {
				auto neighbourId = poly->conn[idx];
				if (neighbourId == INVALID_ID || neighbourId == parentRef )
					continue;

				dtNode* neighbourNode = _nodePool->getNode(neighbourId);
				if (neighbourNode == nullptr) {
					outOfNode = true;
					ret |= DT_OUT_OF_NODES;
					break;
				}



				//If the node is visited the first time, calculate node position.
				if (neighbourNode->flags == 0) {
					neighbourNode->pos = getEdgeMidPoint(tile, poly, idx);
				}
				float cost = 0;
				float heuristic = 0;
				if (neighbourId == endPolyId) {
					float curCost = dtVdist2D(bestNode->pos, neighbourNode->pos);
					float endCost = dtVdist2D(neighbourNode->pos, ep);
					cost = bestNode->cost + curCost + endCost;
					heuristic = 0;
				}
				else {
					float curCost = dtVdist2D(bestNode->pos, neighbourNode->pos);
					cost = bestNode->cost + curCost;
					heuristic = dtVdist2D(neighbourNode->pos,ep);
				}
				float total = cost + heuristic;
				if ((neighbourNode->flags & DT_NODE_OPEN) && total >= neighbourNode->total)
					continue;

				if ((neighbourNode->flags & DT_NODE_CLOSED) && total >= neighbourNode->total)
					continue;

				neighbourNode->pidx = _nodePool->getNodeIdx(bestNode);
				neighbourNode->id = neighbourId;
				neighbourNode->flags = (neighbourNode->flags & ~DT_NODE_CLOSED);
				neighbourNode->cost = cost;
				neighbourNode->total = total;
				neighbourNode->hops = bestNode->hops + 1;



				if (neighbourNode->flags & DT_NODE_OPEN)
				{
					// Already in open, update node location.
					_openList->modify(neighbourNode);
				}
				else
				{
					// Put the node in open list.
					neighbourNode->flags |= DT_NODE_OPEN;
					_openList->push(neighbourNode);
				}


			}

		}
		//get path

		 ret |= getPath(lastNode, polyPath, maxPath, pathCount);



		 return ret;


	}
#pragma endregion



}


