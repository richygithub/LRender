#pragma once

#include <vector>
#include "glm\glm.hpp"

namespace DCEL{



	struct Edge
	{
		int id;
		int twin;   // twin edge id
		int prev;    // pre edge id
		int next;   // succ edge id
		int face;   // face id;
		int orig;    // orig vertex id
		Edge():id(-1),orig(-1),face(-1){}
	};

	struct Vertex {
		int idx;
		int inc; // one edge from vertex
		glm::vec3 pos;
		Vertex(int idx,int inc,glm::vec3 pos):inc(inc),idx(idx),pos(pos){}
	};

	struct Face {
		int id;  // face id;
		int inc; // one edge concturct face

	};

}

struct Delaunay2d_t {
	std::vector<glm::vec3>& points;
	std::vector<DCEL::Edge> edges;
	std::vector<DCEL::Vertex> verts;
	std::vector<DCEL::Face> faces;
	Delaunay2d_t(std::vector<glm::vec3>& p):points(p){}
	inline void clear() {
		edges.clear();
		verts.clear();
		faces.clear();
	}
};

void delaunay2d( std::vector<glm::vec3>& points ,Delaunay2d_t& input);
int addContrainedEdge(int start, int end, Delaunay2d_t& del);
void addOutline(std::vector<int>& points, Delaunay2d_t& del);
void removeHoles(std::vector<int>& points, Delaunay2d_t& del);

std::vector<int> traveral_delaunay(std::vector<glm::vec3>& points, std::vector<DCEL::Edge>& edges, std::vector<DCEL::Face>& face);
std::vector<int> extractTri(Delaunay2d_t& del);
