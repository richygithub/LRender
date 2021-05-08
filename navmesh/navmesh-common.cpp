/*
 * @Author: libl
 * @Date: 2021-05-07 11:33:52
 * @LastEditors: libl
 * @Description: 
 */
#include "navmesh-common.h"
namespace NavMesh_RunTime {
	static float area2D(Vec3 a, Vec3 b, Vec3 c) {
		return (a.x - c.x) * (b.z - c.z) - (a.z - c.z) * (b.x - c.x);
	}

	static bool toLeft2D(Vec3 a, Vec3 b, Vec3 c) {
		return area2D(a, b, c) > 0;
	}
	static bool onLeft2D(Vec3 a, Vec3 b, Vec3 c) {
		return area2D(a, b, c) >= 0;
	}



	bool inTriangle(Vec3 a, Vec3 b, Vec3 c, Vec3 p) {
		return onLeft2D(a, b, p) && onLeft2D(b, c, p) && onLeft2D(c, a, p);
	}
}

