/*
 * @Author: libl
 * @Date: 2021-04-26 14:59:03
 * @LastEditors: libl
 * @Description: 
 */
#include <algorithm>
#include <vector>
#include <stack>
#include "utils.h"
#include "delaunay.h"

using namespace DCEL;
using namespace std;
using namespace glm;


struct delaunay_s {
	int rightmost_e;
	int leftmost_e;
	int start_point;
	int end_point;
	delaunay_s(){}
};

int del_get_lower_tangent(delaunay_s& left, delaunay_s& right, Delaunay2d_t& output);

bool pointCmp(vec3 a,vec3 b) {
	if (a.x < b.x) {
		return true;
	}
	else if (a.x > b.x) {
		return false;
	}
	else if (a.z < b.z) {
		return true;
	}
	else{
		return false;
	}
}
bool vertCmp(Vertex a, Vertex b) {
	if (a.pos.x < b.pos.x) {
		return true;
	}
	else if (a.pos.x > b.pos.x) {
		return false;
	}
	else if (a.pos.z < b.pos.z) {
		return true;
	}
	else {
		return false;
	}
}
int del_init_tri(delaunay_s& del, int start, Delaunay2d_t& output) {

	vec3 p0 = output.points[start];
	vec3 p1 = output.points[start+1];
	vec3 p2 = output.points[start+2];


	auto& edges = output.edges;
	auto& verts = output.verts;
	//ccw
	int seq[3];
	if (toLeft2D(p0, p1, p2)) {
		seq[0] = start;
		seq[1] = start+1;
		seq[2] = start+2;
	}
	else {
		seq[0] = start;
		seq[1] = start+2;
		seq[2] = start+1;	
	}
	del.start_point = start;
	del.end_point = start + 2;


	int id = edges.size();


	Edge es[3];
	Edge cwes[3];
	//ccw 初始化
	for (int idx = 0; idx < 3; idx++) {
		es[idx].orig = seq[idx];
		es[idx].id = id + idx;
		verts[seq[idx]].inc = es[idx].id;
	}
	id += 3;
	//cw 初始化
	for (int idx = 0; idx < 3; idx++) {
		cwes[idx].orig = seq[idx];
		cwes[idx].id = id + idx;
	}

	//设置前驱、后继、twin
	for (int idx = 0; idx < 3; idx++) {
		es[idx].next = es[idx].prev =  cwes[idx].id;
		es[idx].twin = cwes[(idx + 1) % 3].id;

		cwes[idx].next = cwes[idx].prev = es[idx].id;
		cwes[idx].twin = es[(idx + 2) % 3].id;

	}

	for (int idx = 0; idx < 3; idx++) {
		edges.push_back(es[idx]);
	}
	for (int idx = 0; idx < 3; idx++) {
		edges.push_back(cwes[idx]);
	}

	del.leftmost_e = es[0].id;
	del.rightmost_e = es[ seq[2] - start ].id;

	return 0;

}

 int del_init_seg(delaunay_s& del, int start, Delaunay2d_t& output) {
	del.start_point = start;
	del.end_point = start + 1;
	auto& ed = output.edges;
	auto& edges = output.edges;
	auto& faces = output.faces;
	auto& verts = output.verts;
	int id = edges.size();

	Edge e[2];
	e[0].id = id;
	e[1].id = id + 1;
	e[0].orig = start;
	e[1].orig = start + 1;
	for (int idx = 0; idx < 2; idx++) {
		e[idx].next = e[idx].prev = e[idx].id;
		e[idx].twin = e[(idx + 1)%2].id;
		verts[start + idx].inc = e[idx].id;
	}


	del.leftmost_e = e[0].id;
	del.rightmost_e = e[1].id;

	edges.push_back(e[0]);
	edges.push_back(e[1]);




	return 0;
}

Rel relation_point_edge(int eid,int pid,Delaunay2d_t& output) {
	
	auto& edges = output.edges;
	auto& points = output.points;
	auto& p = points[pid];
	auto& edge = edges[eid];
	auto& a = points[edge.orig];
	auto& b = points[ edges[edge.twin].orig ];
	return relation_point_seg(a, b, p);
}
//void del_add_edge(int p0, int p1, vector<Edge>& edges, vector<Vertex>& verts) {
//	Edge e0, e1;
//	e0.id = edges.size();
//	e1.id = e0.id + 1;
//
//	e0.orig = p0;
//	e0.twin = e1.id;
//	e1.orig = p1;
//	e1.twin = 
//
//
//	edges.push_back(e0);
//	edges.push_back(e1);
//
//}
//	

void del_remove_edge(Edge base, vector<Edge>& edges, vector<Vertex>& verts ) {
	//self
	edges[base.next].prev = base.prev;
	edges[base.prev].next = base.next;
	if (verts[base.orig].inc == base.id) {
		//update
		verts[base.orig].inc = base.next;
	}

	//del
	edges[base.id].id = -1;

	base = edges[base.twin];
	//twin;
	edges[base.next].prev = base.prev;
	edges[base.prev].next = base.next;

	if (verts[base.orig].inc == base.id) {
		//update
		verts[base.orig].inc = base.next;
	}


	//del
	edges[base.id].id = -1;


}


Edge del_link_left(Edge base,vector<Edge>&edges,vector<vec3>&points,vector<Vertex>& verts) {

	//base.orig
	auto& pl = points[ base.orig];
	auto& pr = points[ edges[base.twin].orig];

	Edge s = edges[base.next];

	int p1 =  edges[s.twin].orig;
	int p2 = edges[edges[s.next].twin].orig;

	if (relation_point_seg(pl, pr, points[p1]) == Rel::LEFT ) {
		while ( p1 != p2 && relation_point_seg(pl, pr, points[p2]) == Rel::LEFT && inCircle2D(pl,pr,points[p1],points[p2]) == Rel::INSIDE ) {

			//remove 
			del_remove_edge(s, edges,verts);

			//verts

			s = edges[s.next];
			p1 = edges[s.twin].orig;
			p2 = edges[edges[s.next].twin].orig;
		}

		return edges[s.twin];
	}
	else {
		return base;
	}

}

Edge del_link_right(Edge base, vector<Edge>& edges, vector<vec3>& points, vector<Vertex>& verts) {
	//base.orig
	auto& pl = points[base.orig];
	auto& pr = points[edges[base.twin].orig];

	Edge s = edges[ edges[base.twin].prev ] ;

	int p1 = edges[s.twin].orig;
	int p2 = edges[edges[s.prev].twin].orig;

	if (relation_point_seg(pl, pr, points[p1]) == Rel::LEFT) {
		while (p1 != p2 && relation_point_seg(pl, pr, points[p2]) == Rel::LEFT && inCircle2D(pl, pr, points[p1], points[p2]) == Rel::INSIDE) {

			//remove 
			del_remove_edge(s, edges,verts);
			s = edges[s.prev];
			p1 = edges[s.twin].orig;
			p2 = edges[edges[s.prev].twin].orig;
		}

		return edges[s.twin];
	}
	else {
		return base;
	}
}

void insert_ccw(const Edge& e,Edge& newEdge,vector<Edge>& edges) {
	newEdge.prev = e.id;
	newEdge.next = e.next;
	edges[e.next].prev = newEdge.id;
	edges[e.id].next = newEdge.id;
}

void insert_cw(const Edge& e,Edge& newEdge,vector<Edge>& edges) {
	newEdge.prev = e.prev;
	newEdge.next = e.id;
	edges[e.prev].next= newEdge.id;
	edges[e.id].prev= newEdge.id;
}


int del_valid_link(int eid, vector<Edge>& edges, vector<vec3>& points,vector<Vertex>& verts, delaunay_s& left, delaunay_s& right) {

	Edge ll = del_link_left( edges[eid], edges, points,verts);

	Edge rr = del_link_right(edges[eid], edges, points,verts);

	if (ll.id == eid && ll.id == rr.id) {

		//finish
		return eid;
	}
	else {

		Edge lr, rl;
		lr.id = edges.size();
		rl.id = lr.id + 1;

		Edge insertL, insertR;
		if (ll.id == eid) {
			//rr-->baseL
			insertL = edges[eid];
			insertR = rr;
		}
		else if (rr.id == eid) {
			//ll-->baseR
			insertL = ll;
			insertR = edges[ edges[eid].twin];	
		}
		else {
			auto& bl = points[edges[eid].orig];
			auto& br = points[edges[edges[eid].twin].orig];
			if (inCircle2D(bl, br, points[ll.orig], points[rr.orig]) == Rel::OUTSIDE) {
				//ll-->baseR
				insertL = ll;
				insertR = edges[edges[eid].twin];;
			}
			else {
				//rr-->baseL
				insertL = edges[eid];
				insertR = rr;
			}
		}

		lr.orig = insertL.orig;
		lr.twin = rl.id;

		rl.orig = insertR.orig;
		rl.twin = lr.id;

		insert_ccw(insertL, lr, edges);

		insert_cw(insertR, rl, edges);
		if (insertR.id == right.rightmost_e) {
			right.rightmost_e = rl.id;
		}

		edges.push_back(lr);
		edges.push_back(rl);

		return lr.id;
	}

}

static void del_merge(delaunay_s& del, delaunay_s& left, delaunay_s& right, Delaunay2d_t& output) {
	int base_e= del_get_lower_tangent(left, right, output);
	auto& edges = output.edges;
	auto& points= output.points;

	int pl =  edges[edges[ edges[base_e].next ].twin].orig;
	int pr =  edges[edges[edges[ edges[base_e].twin].prev].twin].orig ;

	while (  relation_point_edge(base_e,pl,output)== Rel::LEFT||
		relation_point_edge(base_e, pr, output) == Rel::LEFT) {
		int lr = del_valid_link(base_e, edges, points, output.verts,left,right);
		if (lr == base_e) {
			break;
		}
		base_e = lr;
		pl = edges[edges[edges[base_e].next].twin].orig;
		pr = edges[edges[edges[edges[base_e].twin].prev].twin].orig;
	}

	del.start_point = left.start_point;
	del.end_point = right.end_point;
	del.leftmost_e = left.leftmost_e;
	del.rightmost_e = right.rightmost_e;

}

int del_get_lower_tangent(delaunay_s& left ,delaunay_s& right, Delaunay2d_t& output) {

	auto& edges = output.edges;
	auto& points = output.points;

	auto left_e = edges[left.rightmost_e];
	auto right_e = edges[right.leftmost_e];

	Rel sl, rl;

	do {

		int pl = edges[edges[left_e.prev].twin].orig;
		int pr = edges[right_e.twin].orig;

		sl = relation_point_seg(points[left_e.orig], points[right_e.orig], points[pl]);
		
		if (sl == Rel::RIGHT) {
			left_e = edges[edges[left_e.prev].twin];
		}

		rl = relation_point_seg(points[left_e.orig], points[right_e.orig], points[pr]);
		if (rl == Rel::RIGHT) {
			right_e = edges[ edges[right_e.twin].next ] ;
		}
	
	} while (sl == Rel::RIGHT || rl == Rel::RIGHT);

	Edge btl,btr;
	btl.id = edges.size();
	btr.id = btl.id + 1;

	btl.orig = left_e.orig;
	btl.twin = btr.id;

	//btl.next = left_e.id;
	
	insert_cw(left_e,btl,edges);
	if (left_e.id == left.leftmost_e) {
		//
		left.leftmost_e = btl.id;
	}


	//btl.prev = left_e.prev;
	//edges[btl.prev].next = btl.id;
	//left_e.prev = btl.id;


	btr.orig = right_e.orig;
	btr.twin = btl.id;

	insert_cw(right_e, btr, edges);


	//btr.next = right_e.id;
	//edges[right_e.prev].next= btr.id;
	//btr.prev = right_e.prev;
	//right_e.prev= btr.id;

	edges.push_back(btl);
	edges.push_back(btr);


	return btl.id;
}

 void del_divide_conquer(delaunay_s& del,int start,int end, Delaunay2d_t& output) {
	delaunay_s	left, right;
	int			i, num;

	num = (end - start + 1);

	if (num > 3) {
		i = (num / 2) + (num & 1);
		del_divide_conquer(left, start, start + i - 1, output);
		del_divide_conquer(right,start + i, end, output);
		del_merge(del, left, right, output);

	}
	else if (num == 3) {
		del_init_tri(del, start,output);
	}
	else if (num == 2) {
		del_init_seg(del, start,output);
	}

}

int contrainedTri_left(int pl,int pr,Edge se,Delaunay2d_t& del ) {
	auto& edges = del.edges;
	auto& points = del.points;
	auto& verts = del.verts;

	//vector<Edge> stack = {pl, se.orig};
	vector<Edge> stack = {se};

	Edge next = se;

	int newEdgeId = -1;
	while (next.orig != pr) {
		next = edges[edges[next.next].twin];

		if (next.orig == pr) {
			//退栈
			while (stack.size()>=1) {
				Edge prev_e = stack[stack.size() - 1];
				Edge e0, e1;
				e0.id = edges.size();
				e1.id = e0.id + 1;
				e0.orig = next.orig;
				e1.orig = edges[prev_e.twin].orig;
				e0.twin = e1.id;
				e1.twin = e0.id;

				insert_ccw(next, e0, edges);
				insert_cw(edges[prev_e.twin], e1, edges);

				edges.push_back(e0);
				edges.push_back(e1);

				newEdgeId = e1.id;

				stack.pop_back();
			}

			return newEdgeId;
		}
		
		//不考虑退化共线的情况
		if (relation_point_seg(points[pl], points[pr], points[next.orig]) == Rel::LEFT) {


			Edge prev_e = stack[stack.size()-1];

			//push
			if (relation_point_edge(prev_e.id, next.orig, del) == Rel::LEFT) {
				//三角化

				Edge top;
				do{


					Edge e0, e1;
					e0.id = edges.size();
					e1.id = e0.id + 1;
					e0.orig = next.orig;
					e1.orig = edges[prev_e.twin].orig;
					e0.twin = e1.id;
					e1.twin = e0.id;

					insert_ccw(next, e0, edges);
					insert_cw(edges[prev_e.twin], e1, edges);

					edges.push_back(e0);
					edges.push_back(e1);
					stack.pop_back();

					top = e0;

				} while ( stack.size()>0 && relation_point_edge( stack[stack.size()-1].id, top.orig, del) == Rel::LEFT);

				stack.push_back(top);

			}
			else {
				//pop
				stack.push_back( next);
			}

		}
		else {
			//delete
			del_remove_edge(next, edges, verts);
			next = edges[next.twin];

		}
	}
	return newEdgeId;
}
void contrainedTri_right(int pl, int pr, Edge se, Delaunay2d_t& del) {
	auto& edges = del.edges;
	auto& points = del.points;
	auto& verts = del.verts;

	//vector<Edge> stack = {pl, se.orig};
	vector<Edge> stack = { se };

	Edge next = se;

	while (next.orig != pr) {
		next = edges[edges[next.prev].twin];

		if (next.orig == pr) {
			//退栈
			while (stack.size() >= 2) {
				Edge prev_e = stack[stack.size() - 1];
				Edge e0, e1;
				e0.id = edges.size();
				e1.id = e0.id + 1;
				e0.orig = next.orig;
				e1.orig = edges[prev_e.twin].orig;
				e0.twin = e1.id;
				e1.twin = e0.id;

				insert_cw(next, e0, edges);
				insert_ccw(edges[prev_e.twin], e1, edges);

				edges.push_back(e0);
				edges.push_back(e1);
				stack.pop_back();
			}

			return;
		}

		//不考虑退化共线的情况
		if (relation_point_seg(points[pl], points[pr], points[next.orig]) == Rel::RIGHT) {


			Edge prev_e = stack[stack.size() - 1];

			//push
			if (relation_point_edge(prev_e.id, next.orig, del) == Rel::RIGHT) {
				//三角化

				Edge top;
				do {
					Edge e0, e1;
					e0.id = edges.size();
					e1.id = e0.id + 1;
					e0.orig = next.orig;
					e1.orig = edges[prev_e.twin].orig;
					e0.twin = e1.id;
					e1.twin = e0.id;

					insert_cw(next, e0, edges);
					insert_ccw(edges[prev_e.twin], e1, edges);

					edges.push_back(e0);
					edges.push_back(e1);
					stack.pop_back();

					top = e0;

				} while (stack.size() > 0 && relation_point_edge(stack[stack.size() - 1].id, top.orig, del) == Rel::RIGHT);
				stack.push_back(top);
			}
			else {
				//pop
				stack.push_back(next);
			}

		}
		else {
			//delete
			del_remove_edge(next, edges, verts);
			next = edges[next.twin];
		}
	}
}

int addContrainedEdge(int start,int end, Delaunay2d_t& del) {
	if (start < 0 || end < 0 || start >= del.verts.size() || end >= del.verts.size()) {
		return -1;
	}

	auto& edges = del.edges;
	auto& points = del.points;
	auto& pl = points[start];
	auto& pr = points[end];

	Edge e0 = edges[del.verts[start].inc];
	Edge e1 = edges[e0.prev];
	int startEdge= e0.id;
	while (e1.id != startEdge) {
		//
		int p0 = edges[edges[e0.id].twin].orig;
		int p1 = edges[edges[e1.id].twin].orig;
		if ( p0 == end ) {
			return e0.id;
		}
		if (p1 == end) {
			return e1.id;
		}
		Rel s0 = relation_point_seg(pl, pr, points[p0]);
		Rel s1 = relation_point_seg(pl, pr, points[p1]);

		if (s0 == Rel::LEFT && s1 == Rel::RIGHT) {
			//split
			
			
			int id = contrainedTri_left(start, end, edges[e0.twin], del);
			contrainedTri_right(start, end, edges[e1.twin], del);
			return id;

		}
		//else if (s0 == Rel::RIGHT && s1 == Rel::LEFT) {
		//	int id = contrainedTri_left(start, end, edges[e1.twin], del);
		//	contrainedTri_right(start, end, edges[e0.twin], del);
		//	return id;
		//}

		e0 = e1;
		e1 = edges[e0.prev];
	}
	return -1;
}

vector<int> extractTri(Delaunay2d_t& del) {
	auto& edges = del.edges;
	auto& face = del.faces;


	vector<int> tris;
	if (face.size()==0) {
		return tris;
	}
	for (auto& f : face) {
		int eid = f.inc;
		int count = 0;
		while ( ++count <= 3) {
			Edge& e = edges[eid];
			tris.push_back(e.orig);
			eid = e.next;
		}
	}
	return tris;

}

void delaunay2d(std::vector<glm::vec3>& points, Delaunay2d_t& del) {

	auto& verts = del.verts;
	//if (points.size() < 3) {
	//	return;
	//}

	if (verts.size() < 3) {
		return;
	}


	//sort(points.begin(), points.end(), vertCmp);
	sort(verts.begin(), verts.end(), vertCmp);
	for (int idx = 0; idx < verts.size(); idx++) {
		points[idx] = verts[idx].pos;
	}


	//del.verts = vector<Vertex>(points.size(), Vertex());

	delaunay_s result;

	del_divide_conquer(result, 0, points.size() - 1,del);

}

// outline points 顺时针
void addOutline(vector<int>& points,Delaunay2d_t& del) {

	int count = points.size();
	auto& edges = del.edges;
	auto& verts = del.verts;
	for (int idx = 0; idx < count; idx++) {

		int p0 = points[idx];
		int p1 = points[ (idx+1)%count ];
		int p2 = points[ (idx+2)%count];

		int e0 = addContrainedEdge(p0, p1, del);
		int e1 = addContrainedEdge(p1, p2, del);

		edges[e0].face = DCEL::OUTSIDE;
		edges[e1].face = DCEL::OUTSIDE;


		int e_prev = edges[edges[e0].twin].prev;
		while ( e_prev != e1) {
			//del
			del_remove_edge(edges[e_prev], edges, verts);
			e_prev = edges[e_prev].prev;
		}

	}
}

//holes contours 逆时针
void removeHoles(vector<int>& points, Delaunay2d_t& del) {
	int count = points.size();
	auto& edges = del.edges;
	auto& verts = del.verts;
	for (int idx = 0; idx < count; idx++) {

		int p0 = points[idx];
		int p1 = points[(idx + 1) % count];
		int p2 = points[(idx + 2) % count];

		int e0 = addContrainedEdge(p0, p1, del);
		int e1 = addContrainedEdge(p1, p2, del);
		assert( edges[e0].id != -1);
		assert(edges[e1].id != -1);

		edges[e0].face = DCEL::INNER_HOLE;
		edges[e1].face = DCEL::INNER_HOLE;


		//int e_next= edges[edges[e0].twin].next;
		//while (e_next!= e1) {
		//	//del
		//	del_remove_edge(edges[e_next], edges, verts);
		//	e_next= edges[e_next].next;
		//}

		int e_prev = edges[edges[e0].twin].prev;
		while (e_prev != e1) {
			//del
			del_remove_edge(edges[e_prev], edges, verts);
			e_prev = edges[e_prev].prev;
		}
	}
}


vector<int> traveral_delaunay(vector<vec3>&points,vector<Edge>& edges,vector<Face>& face) {
	vector<int> tris;

	vector<bool> visit(edges.size(), false);
	int outfaceId = -1;
	for (auto& e : edges) {
		if (e.id >= 0 && e.face == DCEL::INIT) {

			int vertnum = 0;

			Edge cur = e;
			Face f;
			f.inc = e.id;
			f.id = face.size();
			do {

				//
				//cur.face = f.id;
				edges[cur.id].face = f.id;

				tris.push_back(cur.orig);
				vertnum++;
				cur = edges[edges[cur.twin].prev];
			} while (cur.id != e.id);			
			face.push_back(f);

			if (vertnum > 3) {
				//out face
				//assert(outfaceId == -1);
				outfaceId = f.id;
				for (int idx = 0; idx < vertnum; idx++) {
					tris.pop_back();
				}
			}

		}

	}
	return tris;

}
