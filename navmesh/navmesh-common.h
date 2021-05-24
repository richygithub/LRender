/*
 * @Author: libl
 * @Date: 2021-05-07 11:33:42
 * @LastEditors: libl
 * @Description: 
 */

#pragma once

#include <math.h>
namespace NavMesh_RunTime {
	const int INVALID_ID = -1;

	typedef unsigned int dtStatus;
	const unsigned int DT_FAILURE = 1u << 31;			// Operation failed.
	const unsigned int DT_SUCCESS = 1u << 30;			// Operation succeed.

	const unsigned int DT_STATUS_DETAIL_MASK = 0x0ffffff;

	//const unsigned int DT_WRONG_MAGIC = 1 << 0;		// Input data is not recognized.
	//const unsigned int DT_WRONG_VERSION = 1 << 1;	// Input data is in wrong version.
	//const unsigned int DT_OUT_OF_MEMORY = 1 << 2;	// Operation ran out of memory.
	const unsigned int DT_INVALID_POSIITON = 1 << 3;	// An input parameter was invalid.
	const unsigned int DT_BUFFER_TOO_SMALL = 1 << 4;	// Result buffer for the query was too small to store all results.
	const unsigned int DT_OUT_OF_NODES = 1 << 5;		// Query ran out of nodes during search.
	const unsigned int DT_PARTIAL_RESULT = 1 << 6;	// Query did not reach the end location, returning best guess. 
	//const unsigned int DT_ALREADY_OCCUPIED = 1 << 7;	// A tile has already been assigned to the given x,y coordinate

	struct Vec3 {
		float x;
		float y;
		float z;
		Vec3() {};
		Vec3(float x,float y,float z):x(x),y(y),z(z){}

	};
	
	inline Vec3 operator+(const Vec3& v1, const Vec3& v2) {
		return Vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
	}
	inline Vec3 operator-(const Vec3& v1, const Vec3& v2) {
		return Vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
	}
	inline Vec3 operator*(const Vec3& v1, float scale) {
		return Vec3(v1.x*scale, v1.y*scale, v1.z*scale);
	}
	
	bool inTriangle(Vec3 a,Vec3 b,Vec3 c,Vec3 p);

	bool toLeft2D(Vec3 a, Vec3 b, Vec3 c);
	bool onLeft2D(Vec3 a, Vec3 b, Vec3 c);
	/// @}
	/// @name Miscellanious functions.
	/// @{

	inline unsigned int dtNextPow2(unsigned int v)
	{
		v--;
		v |= v >> 1;
		v |= v >> 2;
		v |= v >> 4;
		v |= v >> 8;
		v |= v >> 16;
		v++;
		return v;
	}

	inline float dtVdistSquare2D(const Vec3& v1, const Vec3& v2)
	{
		const float dx = v2.x - v1.x;
		const float dz = v2.z - v1.z;
		return (dx * dx + dz * dz);
	}
	inline float dtVdist2D(const Vec3& v1, const Vec3& v2)
	{
		const float dx = v2.x - v1.x;
		const float dz = v2.z - v1.z;
		return sqrtf(dx * dx + dz * dz);
	}
	inline float cross2D(const Vec3& a, const Vec3& b) {
		return a.x * b.z - a.z * b.x;
	}


	inline bool IntersectSeg2D(const Vec3& a1, const Vec3& a2, const Vec3& b1, const Vec3& b2, float& t1, float& t2) {
		Vec3 a = a2 - a1;
		Vec3 b = b2 - b1;
		float d = cross2D(a, b);
		if (d< 1e-6f && d > -1e-6f)
			return false;

		Vec3 w = b1 - a1;
		t1 = cross2D(w, b) / d;
		t2 = cross2D(w, a) / d;

	}

}

