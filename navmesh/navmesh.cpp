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
	void NavMesh::findTile(Vec3 point, int& x, int& y)const {
		x = (point.x - _min.x) / (_tileSize * _cellSize);
		y= (point.z - _min.z) / (_tileSize * _cellSize);
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
		int p1 = poly->verts[(edgeId+1) % PolyVertNum];
		return (tile->_verts[p0] + tile->_verts[p1])*0.5f;

	}

	Vec3 getEdgeNearPoint(const MeshTile* tile, const MeshPoly* poly, int edgeId,const Vec3& spos,const Vec3& epos) {
		int p0 = poly->verts[edgeId];
		int p1 = poly->verts[(edgeId + 1) % PolyVertNum];
		const Vec3& v0 = tile->_verts[p0];
		const Vec3& v1 = tile->_verts[p1];

		float t0=0, t1 = 0;
		if (IntersectSeg2D(v0, v1, spos, epos, t0, t1)) {
			
			if (t0 >= 0 && t0 <= 1) {				
				return v0 + (v1 - v0) * t0;
			}

		}

		float d1 = dtVdist2D(epos, v0);
		float d2 = dtVdist2D(epos, v1);
		if (d1 < d2) {
			return v0;
		}
		else {
			return v1;
		}
		//return (tile->_verts[p0] + tile->_verts[p1]) * 0.5f;

	}

	static void swapVert(Vec3& v0,Vec3& v1) {
		Vec3 tmp = v0;
		v0 = v1;
		v1 = tmp;
	}
	bool NavMeshQuery::getPortalPoint(const dtNode* node, Vec3& left, Vec3& right) {
		if (node == nullptr)
			return false;
		int conn = node->conn;
		node = _nodePool->getNodeAtIdx(node->pidx);
		if (node == nullptr)
			return false;
		const MeshTile* tile = nullptr;
		const MeshPoly* poly = nullptr;
		_meshData->extractPolyTile(node->id, &tile, &poly);
		int vi0 = poly->verts[conn];
		int vi1 = poly->verts[(conn + 1) % PolyVertNum];
		Vec3 v0 = tile->_verts[vi0];
		Vec3 v1 = tile->_verts[vi1];
		left = v0;
		right = v1;
		//if (v0.x > v1.x) {
		//	left = v1;
		//	right = v0;
		//}
		//else {
		//	left = v0;
		//	right = v1;
		//}
		return true;
	}

	dtStatus NavMeshQuery::getPathPoint(Vec3 start, Vec3 end,  const dtNode* endNode, Vec3* pointPath, const int maxPoint, int& pointCount) {
		dtStatus s = DT_SUCCESS;
		//while (endNode->hops >= maxPoint) {
		//	endNode = _nodePool->getNodeAtIdx(endNode->pidx);
		//	s |= DT_BUFFER_TOO_SMALL;
		//}

		int conn = endNode->conn;
		Vec3 curPoint = end;

		endNode = _nodePool->getNodeAtIdx(endNode->pidx);
		const MeshTile* tile = nullptr;
		const MeshPoly* poly = nullptr;

		_meshData->extractPolyTile(endNode->id, &tile, &poly);

		int vi0 = poly->verts[conn];
		int vi1 = poly->verts[(conn+1)% PolyVertNum];


		Vec3 portalLeft = tile->_verts[vi0];
		Vec3 portalRight = tile->_verts[vi1];
		//if (portalLeft.x > portalRight.x) {
		//	swapVert(portalLeft, portalRight);
		//}

		int pCount = 0;
		pointPath[pCount++] = end;


		const dtNode* leftNode = endNode;
		const dtNode* rigthNode = endNode;

		while (endNode!=nullptr && pCount< maxPoint) {
			conn = endNode->conn;
			endNode = _nodePool->getNodeAtIdx(endNode->pidx);
			Vec3 v0, v1;
			if (endNode == nullptr) {
				//
				v0 = start;
				v1 = start;
			}
			else {
				_meshData->extractPolyTile(endNode->id, &tile, &poly);

				int vi0 = poly->verts[conn];
				int vi1 = poly->verts[(conn + 1) % PolyVertNum];
				v0 = tile->_verts[vi0];
				v1 = tile->_verts[vi1];

				//if (v0.x > v1.x) {
				//	swapVert(v0, v1);
				//}
			}

			if (onLeft2D(curPoint, v0,portalLeft)) {
				portalLeft = v0;
				leftNode = endNode;
				if (toLeft2D(curPoint, portalLeft, portalRight)) {
					//点
					pointPath[pCount++] = portalRight;
					curPoint = portalRight;
					endNode = rigthNode;
					getPortalPoint(rigthNode, portalLeft, portalRight);
				}
			}

			if (onLeft2D(curPoint, portalRight, v1)) {
				portalRight = v1;
				rigthNode = endNode;
				if (toLeft2D(curPoint, portalLeft, portalRight)) {
					//点
					pointPath[pCount++] = portalLeft;
					curPoint = portalLeft;
					endNode = leftNode;
					getPortalPoint(leftNode, portalLeft, portalRight);
	
				}

			}

		}

		pointPath[pCount++] = start;
		pointCount = pCount;

		return s;

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
				//if (neighbourNode->flags == 0) {
				//	neighbourNode->pos = getEdgeMidPoint(tile, poly, idx);
				//}

				neighbourNode->pos = getEdgeNearPoint(tile, poly, idx, bestNode->pos, ep);


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
				neighbourNode->conn = idx;



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
		 //const int maxPathPoint = 2048;
		 //Vec3 pathPoints[maxPathPoint];
		 //int pointCount = 0;
		 if (pathCount > 1) {
			getPathPoint(sp, ep, lastNode, _pathPoint, MaxPathPoint,  _pathCount);
		 }
		 else {
			 _pathCount = 2;
			 _pathPoint[0] = ep;
			 _pathPoint[1] = sp;

		 }


		 return ret;


	}

	dtStatus NavMeshQuery::findPathPoint(Vec3 start, Vec3 end, unsigned int* polyPath, const int pathCount, Vec3* pointPath, const int maxPoint, int& pointCount) {
		return 0;
	}
	
#pragma endregion



}


