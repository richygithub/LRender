/*
 * @Author: libl
 * @Date: 2021-05-18 20:21:24
 * @LastEditors: libl
 * @Description: 
 */
#pragma once

#if defined(_MSC_VER)
 //  Microsoft 
#define EXPORT __declspec(dllexport)
#define IMPORT __declspec(dllimport)
#elif defined(__GNUC__)
 //  GCC
#define EXPORT __attribute__((visibility("default")))
#define IMPORT
#else
 //  do nothing and hope for the best?
#define EXPORT
#define IMPORT
#pragma warning Unknown dynamic link import/export semantics.
#endif

#ifndef WIN32 // or something like that...
#define __stdcall
#endif

#if MY_LIB_COMPILING
#   define MY_LIB_PUBLIC EXPORT
#else
#   define MY_LIB_PUBLIC IMPORT
#endif

#include "navmeshBuild.h"
#include "navmesh.h"

struct BuildSetting2D
{
    float cellSize;
    int tileSize;
    int agentRadius;
    int minBlock;
    int lineError;
};


extern "C" {
#pragma region Editor


    EXPORT NavmeshBuilder* __stdcall CreateNavmeshBuilder();
    EXPORT void __stdcall DeleteNavmeshBuilder(NavmeshBuilder* navmeshBuilder);
    EXPORT void __stdcall ClearNavmeshBuilder(NavmeshBuilder* navmeshBuilder);



    EXPORT void __stdcall BuildNavMesh(NavmeshBuilder* navmeshBuilder, BuildSetting2D buildSetting, const char* path);
    EXPORT void __stdcall GetNavMeshTris(NavmeshBuilder* navmeshBuilder, glm::vec3*& verts, int* vertCount,int*&tris,int* triCount);

    EXPORT int __stdcall FindPathEditor(NavmeshBuilder* navmeshBuilder, NavMesh_RunTime::Vec3 startPos, NavMesh_RunTime::Vec3 endPos, NavMesh_RunTime::Vec3* path, int maxPath, int* pathCount);

    EXPORT void __stdcall  AddUnityObj(NavmeshBuilder* navmeshBuilder, const glm::vec3* verts, int vertNum, const int* tris, int triNum);
    EXPORT void __stdcall  AddUnityGround(NavmeshBuilder* navmeshBuilder, const glm::vec3* verts, int vertNum, const int* tris, int triNum);

#pragma endregion

#pragma region runtime


    EXPORT NavMesh_RunTime::NavMesh* __stdcall LoadNavmesh(const char* bytes,int len);
    EXPORT void __stdcall DeleteNavmeshData(NavMesh_RunTime::NavMesh* navmeshData);

    EXPORT NavMesh_RunTime::NavMeshQuery* __stdcall CreateNavmeshQuery(NavMesh_RunTime::NavMesh* data,int maxNode);
    EXPORT void __stdcall DeleteNavmeshQuery(NavMesh_RunTime::NavMeshQuery* query);
    EXPORT void __stdcall InitNavmeshQuery(NavMesh_RunTime::NavMeshQuery* query,NavMesh_RunTime::NavMesh* data,int maxNode);
 

    EXPORT int __stdcall FindPath(NavMesh_RunTime::NavMeshQuery* query, NavMesh_RunTime::Vec3 startPos, NavMesh_RunTime::Vec3 endPos, NavMesh_RunTime::Vec3* path, int maxPath, int* pathCount);

#pragma endregion





};
