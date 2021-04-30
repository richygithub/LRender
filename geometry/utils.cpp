/*
 * @Author: libl
 * @Date: 2021-04-26 15:19:16
 * @LastEditors: libl
 * @Description: 
 */
#include "utils.h"


using namespace glm;




float det3(mat3 m) {

	float	res = m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1])
		- m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0])
		+ m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);

	return res;

}

float area2D(vec3 a, vec3 b, vec3 c) {
	return (a.x - c.x) * (b.z - c.z) - (a.z - c.z) * (b.x - c.x);
}

//does c lie on, to the left of ab?
bool toLeft2D(vec3 a,vec3 b, vec3 c) {
	return area2D(a, b, c) > 0;
}

bool onLeft2D(vec3 a,vec3 b, vec3 c) {
	return area2D(a, b, c) >= 0;
}
Rel relation_point_seg(glm::vec3 a, glm::vec3 b, glm::vec3 p) {
	float res = area2D(a, b, p);
	if (res > 0) {
		return Rel::LEFT;
	}
	else if (res < 0) {
		return Rel::RIGHT;
	}
	return Rel::ON;
}

// test if point p is inside a circle given by a,b,c
// ref: https://www.cs.cmu.edu/~quake/robust.html
// return: 1 -- inside; -1 -- outside ; 0 -- oncircle
Rel inCircle2D(vec3 a,vec3 b, vec3 c,vec3 p) {
	mat3 m;
	float p0p_x = a.x - p.x;
	float p0p_y = a.z - p.z;

	float p1p_x = b.x - p.x;
	float p1p_y = b.z - p.z;

	float p2p_x = c.x - p.x;
	float p2p_y = c.z - p.z;

	float p0p = p0p_x * p0p_x + p0p_y * p0p_y;
	float p1p = p1p_x * p1p_x + p1p_y * p1p_y;
	float p2p = p2p_x * p2p_x + p2p_y * p2p_y;

	m[0][0] = p0p_x;
	m[0][1] = p0p_y;
	m[0][2] = p0p;

	m[1][0] = p1p_x;
	m[1][1] = p1p_y;
	m[1][2] = p1p;

	m[2][0] = p2p_x;
	m[2][1] = p2p_y;
	m[2][2] = p2p;

	float res = -det3(m);
	if (res < -REAL_ZERO ) {
		return Rel::INSIDE;
	}
	else if (res > REAL_ZERO ) {
		return Rel::OUTSIDE;
	}
	return Rel::ON;
}
