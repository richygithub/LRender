/*
 * @Author: libl
 * @Date: 2021-05-18 20:21:46
 * @LastEditors: libl
 * @Description: 
 */
#include <string>
#include "export_unity.h"



NavmeshBuilder* CreateNavmeshBuilder() {
	return new NavmeshBuilder();
}

void ClearNavmeshBuilder(NavmeshBuilder* navmeshBuilder) {
	navmeshBuilder->clear();
}
void DeleteNavmeshBuilder(NavmeshBuilder* navmeshBuilder) {
	delete navmeshBuilder;
}
int FindPathEditor(NavmeshBuilder* navmeshBuilder, NavMesh_RunTime::Vec3 startPos, NavMesh_RunTime::Vec3 endPos, NavMesh_RunTime::Vec3* path, int maxPath, int* pathCount) {
	return navmeshBuilder->_query.findPathPoint(startPos, endPos, path, maxPath, *pathCount);
}
void AddUnityObj(NavmeshBuilder* navmeshBuilder, const glm::vec3* verts, int vertNum, const int* tris, int triNum) {
	navmeshBuilder->addUnityObj(verts, vertNum, tris, triNum);
}
void AddUnityGround(NavmeshBuilder* navmeshBuilder, const glm::vec3* verts, int vertNum, const int* tris, int triNum) {
	navmeshBuilder->addUnityGround(verts, vertNum, tris, triNum);
}

void BuildNavMesh(NavmeshBuilder* navmeshBuilder, BuildSetting2D buildSetting, const char* path) {
	Cfg cfg;
	cfg.cellSize = buildSetting.cellSize;
	cfg.tileSize = buildSetting.tileSize;
	cfg.agentRadius = buildSetting.agentRadius;
	cfg.minBlock = buildSetting.minBlock;
	cfg.lineError = buildSetting.lineError;
	navmeshBuilder->buildAll(cfg);
	navmeshBuilder->save(path);
}

void GetNavMeshTris(NavmeshBuilder* navmeshBuilder, glm::vec3*& verts, int* vertCount, int*& tris, int* triCount) {
	navmeshBuilder->generateTris();
	verts = navmeshBuilder->debug_allVerts.data();
	*vertCount = navmeshBuilder->debug_allVerts.size();
	tris = navmeshBuilder->debug_allTris.data();
	*triCount = navmeshBuilder->debug_allTris.size();

}

int GetNavMeshTris(NavMesh_RunTime::NavMesh* navmeshData, NavMesh_RunTime::Vec3* tris, int* trisCount) {


	return 0;
}

NavMesh_RunTime::NavMesh* LoadNavmesh(const char* bytes, int len) {
	NavMesh_RunTime::NavMesh* mesh = new NavMesh_RunTime::NavMesh();
	mesh->load(bytes, len);
	return mesh;
}

void DeleteNavmeshData(NavMesh_RunTime::NavMesh* navmeshData) {
	delete navmeshData;
}

NavMesh_RunTime::NavMeshQuery* CreateNavmeshQuery(NavMesh_RunTime::NavMesh* data, int maxNode) {
	NavMesh_RunTime::NavMeshQuery* query = new NavMesh_RunTime::NavMeshQuery();
	query->init(data, maxNode);
	return query;
}
void InitNavmeshQuery(NavMesh_RunTime::NavMeshQuery* query,NavMesh_RunTime::NavMesh* data, int maxNode) {
	query->init(data, maxNode);
}
 
void DeleteNavmeshQuery(NavMesh_RunTime::NavMeshQuery* query) {
	delete query;
}

int FindPath(NavMesh_RunTime::NavMeshQuery* query, NavMesh_RunTime::Vec3 startPos, NavMesh_RunTime::Vec3 endPos, NavMesh_RunTime::Vec3* path,int maxPath, int* pathCount) {
	return query->findPathPoint(startPos, endPos, path,maxPath,*pathCount);
}
