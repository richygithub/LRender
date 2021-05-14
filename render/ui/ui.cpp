#include "ui.h"
#include "imgui.h"
//#include "imgui_filebrowser.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui_filebrowser.h"
#include "scene/scene.h"
#include <iostream>
#include "utils\meshLoader.h"

#include <GLFW/glfw3.h>
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "navmeshBuild.h"
#include <time.h>
#include "geometry\delaunay.h"
#include "geometry\utils.h"
#include "input\input.h"
#include "navmesh\navmesh.h"
#include <algorithm>

using namespace glm;
using namespace std;


void LButtonDown(double x, double y) {


}

bool bPathFind = false;
bool bPathPoint = false;
int pairTurn = 0;
vector<vec3> clickPos(2);
const int MaxPolyPath = 2048;
unsigned int polyPath[MaxPolyPath];
int polyCount = 0;

int clickTileX = 0;
int clickTileY = 0;
void UI::lbuttondown(double xpos, double ypos) {
	Camera& camera = _scene->getCamera();

	auto matrixP = camera.getMatrixP();
	auto matrixV = camera.getMatrixV();
	glm::ivec4 viewport;
	glGetIntegerv(GL_VIEWPORT, (int*)&viewport);
	ypos = (float)viewport[3] - ypos;

	GLfloat depth;
	glReadPixels(xpos, ypos, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

	auto wpos = glm::unProject(glm::vec3(xpos, ypos, depth), matrixV, matrixP, viewport);

	//printf("this LButtonDown :");
	//printf("(%.3f,%.3f,%.3f)-->\n(%.3f,%.3f,%.3f)", xpos, ypos, depth, wpos.x, wpos.y, wpos.z);
	{
		NavMesh_RunTime::Vec3 pos = NavMesh_RunTime::Vec3(wpos.x, wpos.y, wpos.z);
		NavMesh_RunTime::Vec3 realPos;
	
		//int x, y;
		if (wpos.x >= gMeshBuilder._min.x && wpos.x <= gMeshBuilder._max.x &&
			wpos.z >= gMeshBuilder._min.z && wpos.z <= gMeshBuilder._max.z ) {
			clickTileX = (wpos.x - gMeshBuilder._min.x) / (gBuildCfg.tileSize * gBuildCfg.cellSize);
			clickTileY = (wpos.z - gMeshBuilder._min.z) / (gBuildCfg.tileSize * gBuildCfg.cellSize);
			printf("click tile (%d,%d)", clickTileX, clickTileY);

		}


	}
	if (bPathFind) {

		NavMesh_RunTime::Vec3 pos = NavMesh_RunTime::Vec3(wpos.x,wpos.y,wpos.z);
		NavMesh_RunTime::Vec3 realPos;
		if ((gMeshBuilder._rtData->findPoly(pos, realPos)) == NavMesh_RunTime::INVALID_ID)
			return;

		//gMeshBuilder._query
		clickPos[pairTurn++] = wpos;
		NavMesh_RunTime::dtStatus ret;
		if (pairTurn == 2) {
			pairTurn = 0;

			ret =gMeshBuilder._query.findPath(NavMesh_RunTime::Vec3(clickPos[0].x, clickPos[0].y, clickPos[0].z),
				NavMesh_RunTime::Vec3(clickPos[1].x, clickPos[1].y, clickPos[1].z),
				polyPath,
				MaxPolyPath,
				polyCount
				);


		}


	}

}
void UI::init(GLFWwindow*window,Scene*scene, InputSystem* input) {
	const char* glsl_version = "#version 130";
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	_win = window;
	_scene = scene;
	_input = input;
	_input->registerLButtoneDown([this](double x, double y) {
		this->lbuttondown(x, y);
	});
}
imgui_addons::ImGuiFileBrowser file_dialog;

int mainPlaneId = -1;
void showNavmesh(Scene* scene);
void showObjects(Scene* scene);
void showTileRasterize(Scene* scene);
void showGeometri(Scene* scene);
void showGround(Scene* scene);
void showTris(Scene* scene);
void showLinks(Scene* scene);

void showPath(Scene* scene);
void showPathPoint(Scene* scene);

void UI::updateCamera() {

	Camera& camera = _scene->getCamera();

	ImGui::Begin("Camera");                          // Create a window called "Hello, world!" and append into it.
	//ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)




	double xpos, ypos;
	glfwGetCursorPos(_win, &xpos, &ypos);
	//glm::unProject()
	auto matrixP = camera.getMatrixP();
	auto matrixV = camera.getMatrixV();
	glm::ivec4 viewport;
	glGetIntegerv(GL_VIEWPORT, (int*)&viewport);
	ypos = (float)viewport[3] - ypos;

	GLfloat depth;
	glReadPixels(xpos, ypos, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

	auto wpos = glm::unProject(glm::vec3(xpos,ypos, depth), matrixV, matrixP,viewport);

	ImGui::Text("(%.3f,%.3f,%.3f)-->\n(%.3f,%.3f,%.3f)", xpos, ypos,depth,wpos.x,wpos.y,wpos.z);


	ImGui::ColorEdit3("clear color", (float*)&camera._clearColor);
	ImGui::InputFloat3("camera position",(float*)&camera._position);
	ImGui::InputFloat3("camera lookAt",(float*)&camera._lookAt);
	ImGui::SliderFloat("speed", &camera._moveSpeed, 1.0f, 100.f);

	//load
	if (ImGui::CollapsingHeader("Objects")) {
		showObjects(_scene);
	}
	if (ImGui::CollapsingHeader("Navmesh")) {
		showNavmesh(_scene);
	}

	if (ImGui::CollapsingHeader("Geo")) {
		showGeometri(_scene);
	}
	if (bPathFind) {
		showPath(_scene);
	}
	if (bPathPoint) {
		showPathPoint(_scene);
	}



	//showGround(_scene);

	ImGui::End();

}


void UI::update() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	updateCamera();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


}


void showObjects(Scene* scene) {
	if (ImGui::Button("load obj")) {
		ImGui::OpenPopup("Open File");
	}
	if (file_dialog.showFileDialog("Open File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), ".obj"))
	{
		std::cout << file_dialog.selected_fn << std::endl;      // The name of the selected file or directory in case of Select Directory dialog mode
		std::cout << file_dialog.selected_path << std::endl;    // The absolute path to the selected file
		//

		auto objs = loadObj(file_dialog.selected_path);
		for (auto& obj : objs) {
			scene->addObject(obj);
		}
	}

	//obj
	if (ImGui::BeginTable("split", 3)) {

		auto objs = scene->getObjs();

		for (auto& it : objs) {
			int id = it.first;
			Object& obj = *it.second;
			ImGui::PushID(id);
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::AlignTextToFramePadding();
			//bool node_open = ImGui::TreeNode("Object", "%d",  idx);
			ImGui::Text("%s", obj.getName().c_str());

			ImGui::TableSetColumnIndex(1);
			//ImGui::Text("my sailor is rich");
			ImGui::Checkbox("visiable", &obj.visiable);

			ImGui::TableSetColumnIndex(2);
			//ImGui::Text("my sailor is rich");
			int select = mainPlaneId;
			if (ImGui::RadioButton("main plane", &select, id)) {
				if (select != mainPlaneId) {
					gMeshBuilder.setPlane(obj.getMesh()->getVerts());
					mainPlaneId = select;
				}
			}

			if (obj.getName().find("water") == 0) {
				gMeshBuilder.addGround(obj.getMesh()->getVerts());
			}


			ImGui::PopID();

		}
	}
	ImGui::EndTable();

}

void showTileBorder(Scene*scene) {
	std::vector< glm::vec3 > verts;
	auto minVert = gMeshBuilder._min;
	auto maxVert = gMeshBuilder._max;
	int width = ceil( (maxVert.x - minVert.x ) / ( gBuildCfg.tileSize*gBuildCfg.cellSize) );
	int height = ceil( (maxVert.z - minVert.z ) / ( gBuildCfg.tileSize*gBuildCfg.cellSize) );


	auto stVert = minVert;
	float maxz = minVert.z + height* gBuildCfg.tileSize * gBuildCfg.cellSize;
	float maxx = minVert.x + width*gBuildCfg.tileSize * gBuildCfg.cellSize;

	for (int x = 0; x <= width; x++) {
		stVert.x = minVert.x + x * gBuildCfg.tileSize * gBuildCfg.cellSize;
		auto endVert = glm::vec3(stVert.x, stVert.y, maxz);

		verts.push_back(stVert);
		verts.push_back(endVert);

	}

	stVert = minVert;
	for (int z = 0; z <= height; z++) {
		stVert.z = minVert.z + z * gBuildCfg.tileSize * gBuildCfg.cellSize;
		auto endVert = glm::vec3(maxx, stVert.y, stVert.z);

		verts.push_back(stVert);
		verts.push_back(endVert);

	}
	scene->renderLines(verts);
}


void showGround(Scene* scene) {
	std::vector< glm::vec3 > verts;	

	int width = 10;
	int step = 10;
	int minx = -width * 10;
	int maxx = width * 10;

	int height = width;
	int minz = minx;
	int maxz = maxx;

	for (int x = -width; x <= width; x++) {
		verts.push_back( glm::vec3(x*step,0,minz  ));
		verts.push_back( glm::vec3(x*step,0,maxz));

	}

	for (int z = -height; z <= height; z++) {
		verts.push_back(glm::vec3(minx,0,z*step));
		verts.push_back(glm::vec3(maxx,0,z*step));

	}
	scene->renderLines(verts);
}




const vec4 red = vec4(1.0f, 0.0f, 0.0f, 1.0f);
const vec4 green = vec4(0.0f, 1.0f, 0.0f, 1.0f);
const vec4 blue = vec4(0.0f, 0.0f, 1.0f, 1.0f);
const vec4 yellow = vec4(1.0f, 1.0f, 0.0f, 1.0f);
const vec4 purple = vec4(1.0f, 0.0f, 1.0f, 1.0f);
const vec4 cyan  = vec4(0.0f, 1.0f, 1.0f, 1.0f);
const vec4 white = vec4(1.0f, 1.0f, 1.0f, 1.0f);
const vec4 black = vec4(0.0f, 0.0f, 0.0f, 1.0f);








static vec4 rc[] = { red,green,blue,yellow,cyan,purple,white };








void showTileRegion(Scene* scene) {
	float cellSize = gMeshBuilder._cellSize;
	vector<vec3> verts;
	vector<vec4> colors;


	for (int x = 0; x < gMeshBuilder._width; x++) {
		for (int y = 0; y < gMeshBuilder._height; y++) {
			auto& tile = gMeshBuilder._tiles[x + y * gMeshBuilder._width];
			if (tile.cells != nullptr && tile.region != nullptr) {
				float offsetX = tile.x * gMeshBuilder._tileSize * cellSize + gMeshBuilder._min.x;
				float offsetY = tile.y * gMeshBuilder._tileSize * cellSize + gMeshBuilder._min.z;


				for (int cx = 0; cx < tile.size; cx++) {
					for (int cy = 0; cy < tile.size; cy++) {
						uint16 rid = tile.region[cx + cy * tile.size];
						if (tile.cells[cx + cy * tile.size].block == 1)
							continue;
						//if (rid == 0)continue;
						//if (rid != 0 ) 
						{
							//push

							float minx = cx * cellSize + offsetX;
							float miny = cy * cellSize + offsetY;
							vec3 v0(minx, 0, miny);
							//auto c0 = vec4(cx/(float)tile.size,cy/(float)tile.size,1.0,1.0);

							vec3 v1(minx + cellSize, 0, miny);
							//auto c1 = vec4((cx+1)/(float)tile.size,cy/(float)tile.size,1.0,1.0);

							vec3 v2(minx + cellSize, 0, miny + cellSize);
							//auto c2 = vec4((cx+1)/(float)tile.size,(cy+1)/(float)tile.size,1.0,1.0);

							vec3 v3(minx, 0, miny + cellSize);
							//auto c3 = vec4((cx)/(float)tile.size,(cy+1)/(float)tile.size,1.0,1.0);
							//float depth = tile.dist[cx + cy * tile.size] / (float)tile.maxDist;
							vec4 c = white;
							if( rid != 0)
								c = rc[ rid%( sizeof(rc)/sizeof(rc[0]) )];

							verts.push_back(v0);
							verts.push_back(v1);
							verts.push_back(v2);

							verts.push_back(v0);
							verts.push_back(v2);
							verts.push_back(v3);

							colors.push_back(c);
							colors.push_back(c);
							colors.push_back(c);

							colors.push_back(c);
							colors.push_back(c);
							colors.push_back(c);


						}
					}
				}
			}
		}
	}
	scene->renderTris(verts, colors);

}
void showTileTris(Scene*scene) {
	if (clickTileY >= 0 && clickTileY < gMeshBuilder._height && clickTileX >= 0 && clickTileX < gMeshBuilder._width) {
		auto& tile = gMeshBuilder._tiles[clickTileX + clickTileY * gMeshBuilder._width];
		auto& tris = tile.tris;
		auto& verts = tile.verts;
		vector<vec3> points;
		for (auto& v : verts) {
			points.push_back(vec3(tile.minx + v.x * tile.cellsize, 0, tile.miny + v.z * tile.cellsize));
		}
		if (points.size() > 0) {
			scene->renderTrisWire(points, tris, GL_LINES, green);
			scene->renderPoints(points, 3.0f, red);

		}

	 }
}
void showTris(Scene* scene) {
	for (int x = 0; x < gMeshBuilder._width; x++) {
		for (int y = 0; y < gMeshBuilder._height; y++) {
			auto& tile = gMeshBuilder._tiles[x + y * gMeshBuilder._width];
			if (tile.cells != nullptr) {

				auto& tris = tile.tris;
				auto& verts = tile.verts;
				vector<vec3> points;
				for (auto& v : verts) {
					points.push_back(vec3(tile.minx + v.x * tile.cellsize, 0, tile.miny + v.z * tile.cellsize));
				}
				if (points.size() > 0) {
					scene->renderTrisWire(points, tris, GL_LINES, green);
					scene->renderPoints(points, 3.0f, red);

				}
			}
		}
	}

}

vec3 getPolyCenter(const Tile& tile,const Poly& poly ) {
	vec3 offset = vec3(tile.minx, 0, tile.miny);
	vec3 a = tile.verts[ poly.verts[0]] * tile.cellsize + offset;
	vec3 b = tile.verts[ poly.verts[1]] * tile.cellsize + offset;
	vec3 c = tile.verts[ poly.verts[2]] * tile.cellsize + offset;

	vec3 center = triCenter2D(a, b, c);
	return center;
}
void showPathPoint(Scene* scene) {
	vector<vec3> path;
	for (int idx = 0; idx < gMeshBuilder._query._pathCount; idx++) {
		auto v = gMeshBuilder._query._pathPoint[idx];
		path.push_back(vec3(v.x, v.y, v.z));
	}
	if (path.size() > 0) {
		scene->renderLines(path, GL_LINE_STRIP, yellow);
	}

}

void showPath(Scene* scene) {
	scene->renderPoints(clickPos, 0.5);

	vector<vec3> verts;
	for (int idx = 0; idx < polyCount; idx++) {
		const NavMesh_RunTime::MeshTile* tile;
		const NavMesh_RunTime::MeshPoly* poly;

		gMeshBuilder._rtData->extractPolyTile(polyPath[idx], &tile, &poly);

		auto v0 = tile->_verts[poly->verts[0]];
		auto v1 = tile->_verts[poly->verts[1]];
		auto v2 = tile->_verts[poly->verts[2]];

		verts.push_back( vec3(v0.x,v0.y,v0.z));
		verts.push_back( vec3(v1.x,v1.y,v1.z));
		verts.push_back( vec3(v2.x,v2.y,v2.z));
	}

	if (verts.size() > 0) {
		scene->renderTris(verts );
	}
	


	
}

void showLinks(Scene* scene) {
	std::vector< glm::vec3 > verts;

	for (int x = 0; x < gMeshBuilder._width; x++) {
		for (int y = 0; y < gMeshBuilder._height; y++) {
			auto& tile = gMeshBuilder._tiles[x + y * gMeshBuilder._width];
			for (int idx = 0; idx < tile.polyNum; idx++) {
				auto& poly = tile.polys[idx];
				vec3 center = getPolyCenter(tile, poly);
				for (int cidx = 0; cidx < 3; cidx++) {
					short tid = 0;
					short pid = 0;
					NavMesh_RunTime::decodePolyId(poly.conn[cidx], pid,tid);
					if (pid >= 0) {
						auto& tarTile = gMeshBuilder._tiles[tid];
						auto& tarPoly = tarTile.polys[pid];
						vec3 tarCenter = getPolyCenter(tarTile, tarPoly);
						verts.push_back(center);
						verts.push_back(tarCenter);
					}
				}
				


			}

		}
	}

	scene->renderLines(verts, GL_LINES, yellow);

}

void showTileRasterize(Scene* scene) {

	if (clickTileY >= 0 && clickTileY < gMeshBuilder._height && clickTileX >= 0 && clickTileX < gMeshBuilder._width) {
		auto& tile = gMeshBuilder._tiles[clickTileX + clickTileY * gMeshBuilder._width];
		if (tile.cells != nullptr) {

			float cellSize = gMeshBuilder._cellSize;
			vector<vec3> verts;
			vector<vec4> colors;

			float offsetX = tile.x * gMeshBuilder._tileSize * cellSize + gMeshBuilder._min.x;
			float offsetY = tile.y * gMeshBuilder._tileSize * cellSize + gMeshBuilder._min.z;


			for (int cx = 0; cx < tile.size; cx++) {
				for (int cy = 0; cy < tile.size; cy++) {
					//int idx = cx + cy * tile.size;
					//if (tile.cells[cx + cy * tile.size].block == 0 && tile.cells[cx + cy * tile.size].border !=0 ) 
					//if(tile.cells[cx + cy * tile.size].border != 0)
					//bool b1 = tile.cells[cx + cy * tile.size].block == 0 && tile.cells[cx + cy * tile.size].border != 0;
					//bool b2 = (cx == 0 || cy == 0 || cx == tile.size - 1 || cy == tile.size - 1);

					////if(cx==0||cy==0||cx==tile.size-1||cy==tile.size-1)
					//if (b2 == true && b1 == false) {
					//	int x = 0;
					//}
					vec4 c = blue;
					auto& cell = tile.cells[cx + cy * tile.size];
					if (cell.block != 0 && cell.border != 0) {
						c = yellow;
					}
					else if (cell.border != 0)
					{
						//auto c = blue;
					}
					else if (cell.block != 0) {
						c = red;
					}
					else
					{
						continue;
					}
					//push
					float minx = cx * cellSize + offsetX;
					float miny = cy * cellSize + offsetY;
					vec3 v0(minx, 0, miny);
					//auto c0 = vec4(cx/(float)tile.size,cy/(float)tile.size,1.0,1.0);

					vec3 v1(minx + cellSize, 0, miny);
					//auto c1 = vec4((cx+1)/(float)tile.size,cy/(float)tile.size,1.0,1.0);

					vec3 v2(minx + cellSize, 0, miny + cellSize);
					//auto c2 = vec4((cx+1)/(float)tile.size,(cy+1)/(float)tile.size,1.0,1.0);

					vec3 v3(minx, 0, miny + cellSize);
					//auto c3 = vec4((cx)/(float)tile.size,(cy+1)/(float)tile.size,1.0,1.0);
					//float depth = tile.dist[cx + cy * tile.size]/(float)tile.maxDist;
					//auto c = vec4(depth , depth, depth, 1);

					verts.push_back(v0);
					verts.push_back(v1);
					verts.push_back(v2);

					verts.push_back(v0);
					verts.push_back(v2);
					verts.push_back(v3);

					colors.push_back(c);
					colors.push_back(c);
					colors.push_back(c);

					colors.push_back(c);
					colors.push_back(c);
					colors.push_back(c);


				}
			}


			scene->renderTris(verts, colors);

		}


	}

	////gMeshBuilder.
	//float cellSize = gMeshBuilder._cellSize;
	//vector<vec3> verts;
	//vector<vec4> colors;


	//for (int x = 0; x < gMeshBuilder._width; x++) {
	//	for (int y = 0; y<gMeshBuilder._height; y++) {
	//		auto& tile = gMeshBuilder._tiles[x + y * gMeshBuilder._width];
	//		if (tile.cells != nullptr) {
	//			float offsetX = tile.x * gMeshBuilder._tileSize * cellSize + gMeshBuilder._min.x;
	//			float offsetY = tile.y * gMeshBuilder._tileSize * cellSize + gMeshBuilder._min.z;


	//			for (int cx = 0; cx < tile.size; cx++) {
	//				for (int cy = 0; cy < tile.size; cy++) {
	//					//int idx = cx + cy * tile.size;
	//					//if (tile.cells[cx + cy * tile.size].block == 0 && tile.cells[cx + cy * tile.size].border !=0 ) 
	//					//if(tile.cells[cx + cy * tile.size].border != 0)
	//					//bool b1 = tile.cells[cx + cy * tile.size].block == 0 && tile.cells[cx + cy * tile.size].border != 0;
	//					//bool b2 = (cx == 0 || cy == 0 || cx == tile.size - 1 || cy == tile.size - 1);

	//					////if(cx==0||cy==0||cx==tile.size-1||cy==tile.size-1)
	//					//if (b2 == true && b1 == false) {
	//					//	int x = 0;
	//					//}
	//					vec4 c = blue;
	//					auto& cell = tile.cells[cx + cy * tile.size];
	//					if (cell.block != 0 && cell.border != 0) {
	//						c = yellow;
	//					}else if (cell.border != 0)
	//					{
	//						//auto c = blue;
	//					}else if (cell.block != 0) {
	//						c = red;
	//					}else
	//					{
	//						continue;
	//					}
	//						//push
	//						float minx = cx * cellSize + offsetX;
	//						float miny = cy * cellSize + offsetY;
	//						vec3 v0(minx, 0, miny);
	//						//auto c0 = vec4(cx/(float)tile.size,cy/(float)tile.size,1.0,1.0);

	//						vec3 v1(minx+cellSize, 0, miny);
	//						//auto c1 = vec4((cx+1)/(float)tile.size,cy/(float)tile.size,1.0,1.0);

	//						vec3 v2(minx+cellSize, 0, miny+cellSize);
	//						//auto c2 = vec4((cx+1)/(float)tile.size,(cy+1)/(float)tile.size,1.0,1.0);

	//						vec3 v3(minx, 0, miny+cellSize);
	//						//auto c3 = vec4((cx)/(float)tile.size,(cy+1)/(float)tile.size,1.0,1.0);
	//						//float depth = tile.dist[cx + cy * tile.size]/(float)tile.maxDist;
	//						//auto c = vec4(depth , depth, depth, 1);
	//						
	//						verts.push_back(v0);
	//						verts.push_back(v1);
	//						verts.push_back(v2);

	//						verts.push_back(v0);
	//						verts.push_back(v2);
	//						verts.push_back(v3);

	//						colors.push_back(c);
	//						colors.push_back(c);
	//						colors.push_back(c);

	//						colors.push_back(c);
	//						colors.push_back(c);
	//						colors.push_back(c);


	//				}
	//			}
	//		}
	//	}
	//}
	//scene->renderTris(verts,colors);
}

void showTileContour(Scene* scene) {
	std::vector< glm::vec3 > verts;
	//c = rc[rid % (sizeof(rc) / sizeof(rc[0]))];

	int rcLen = sizeof(rc) / sizeof(rc[0]);
	for (int x = 0; x < gMeshBuilder._width; x++) {
		for (int y = 0; y < gMeshBuilder._height; y++) {
			auto& tile = gMeshBuilder._tiles[x + y * gMeshBuilder._width];

			int idx = 0;
			//for (auto& contour : tile.rawCountours) 
			for (auto& contour : tile.showCountours)
			{

				auto c = rc[(idx++) % rcLen];
				scene->renderLines(contour,GL_LINE_LOOP,c);
			}
			//for (auto& contour : tile.simpleCountours) {

			//	//auto c = rc[(idx++) % rcLen];
			//	scene->renderLines(contour, GL_LINE_LOOP, white);
			//}

		}
	}

}
void showNavmesh(Scene*scene) {

	ImGui::InputInt("TileSize", &gBuildCfg.tileSize);

/*	if (ImGui::SliderInt("TileSize", &gBuildCfg.tileSize, 1, 1024)) {

	}
	*/
	ImGui::InputFloat("CellSize", &gBuildCfg.cellSize, 0.1, 2);


	//if (ImGui::SliderFloat("CellSize", &gBuildCfg.cellSize, 0.1, 2)) {

	//}

	ImGui::InputInt("minBlock", &gBuildCfg.minBlock);


	ImGui::SliderInt("Line Error", &gBuildCfg.lineError, 0, 10);

	ImGui::SliderInt("agent radius", &gBuildCfg.agentRadius, 1, 10);





	int cellX = ceil( (gMeshBuilder._max.x - gMeshBuilder._min.x) / gBuildCfg.cellSize) ;
	int cellZ = ceil( (gMeshBuilder._max.z - gMeshBuilder._min.z) / gBuildCfg.cellSize) ;

	int TileX = ceil( cellX / gBuildCfg.tileSize );
	int TileZ = ceil( cellZ / gBuildCfg.tileSize );

	ImGui::Text("Tile: %d X %d",TileX,TileZ);
	//std::vector<glm::vec3> quads;
	////quads.push_back(vec3(6.6, 0.2, 0));
	////quads.push_back(vec3(3.6, 0.2, 0));
	////quads.push_back(vec3(6.6, 3.2, 0));
	////quads.push_back(vec3(3.6, 3.2, 0));

	//quads.push_back(vec3(3.6, 0.2, 0));
	//quads.push_back(vec3(3.6, 3.2, 0));
	//quads.push_back(vec3(6.6, 0.2, 0));
	//quads.push_back(vec3(6.6, 3.2, 0));
	//quads.push_back(vec3(6.6, 0.2, 0));
	//quads.push_back(vec3(3.6, 3.2, 0));
	//scene->renderTris(quads);


	ImGui::Checkbox("remove holes",&gBuildCfg.removeHoles);

	if (ImGui::Button("SetGround")) {
		gMeshBuilder.clearGround();
		auto objs = scene->getObjs();
		for (auto& it : objs) {
			Object& obj = *it.second;
			if (obj.getName().find("water") == 0) {
				gMeshBuilder.addGround(obj.getMesh()->getVerts());
			}
		}

	}


	if (ImGui::Button("build")) {
		for (auto it : scene->getObjs()) {

			auto mesh = it.second->getMesh();
			if (it.first != mainPlaneId) {

				if(it.second->visiable)
				gMeshBuilder.addObj(mesh->getVerts(), mesh->getTris());
				//auto verts = gMeshBuilder.rasterize(mesh->getVerts(), mesh->getTris(), 0);

				//scene->addMesh(verts);
			}
			else {
				//std::vector< glm::vec3 > verts;
				//auto minVert = gMeshBuilder._min;
				//auto maxVert = gMeshBuilder._max;
				//auto stVert = minVert;
				//for (int x = 0; x < gMeshBuilder._width; x++) {
				//	stVert.x = minVert.x + x * gBuildCfg.tileSize * gBuildCfg.cellSize;
				//	auto endVert = glm::vec3(stVert.x,stVert.y,maxVert.z  );

				//	verts.push_back(stVert);
				//	verts.push_back(endVert);

				//}
				//for (int z = 0; z < gMeshBuilder._height; z++) {
				//	stVert.z = minVert.z + z * gBuildCfg.tileSize * gBuildCfg.cellSize;
				//	auto endVert = glm::vec3(maxVert.x, stVert.y, stVert.z);

				//	verts.push_back(stVert);
				//	verts.push_back(endVert);

				//}

				//scene->addLines(verts);

			}
		}
		gMeshBuilder.build(gBuildCfg);
		//gMeshBuilder.debug();
		//for (auto& v : gMeshBuilder._debug) {
		//	scene->addMesh(v);
		//}
	}
	if (ImGui::Button("Tri")) {
		gMeshBuilder.build_Tri(gBuildCfg);
	}

	if (ImGui::Button("add outline")) {
		gMeshBuilder.debug_addOutline(gBuildCfg);
	}

	if (ImGui::Button("rm holes")) {
		gMeshBuilder.debug_removeHole(gBuildCfg);
	}


	if (ImGui::Button("Save")) {
		gMeshBuilder.seralize();
	}

	static bool showRegion;
	ImGui::Checkbox("show region", &showRegion);

	if (showRegion) {
		showTileRegion(scene);
	}
	
	static bool showRasterize;
	ImGui::Checkbox("show raster", &showRasterize);
	if (showRasterize) {
		showTileRasterize(scene);
	}


	static bool showContour;
	ImGui::Checkbox("show Contour", &showContour);

	if (showContour) {
		showTileContour(scene);
	}

	static bool bshowTileTris;
	ImGui::Checkbox("show tile tris", &bshowTileTris);

	if (bshowTileTris) {
		showTileTris(scene);
	}



	static bool bshowTris;
	ImGui::Checkbox("show tris", &bshowTris);

	if (bshowTris) {
		showTris(scene);
	}

	static bool bshowLinks;
	ImGui::Checkbox("show links", &bshowLinks);

	if (bshowLinks) {
		showLinks(scene);
	}

	ImGui::Checkbox("path find", &bPathFind);

	ImGui::Checkbox("path point", &bPathPoint);



	
	showTileBorder(scene);

}
bool vertCmp1(vec3 a, vec3 b) {
	if (a.x < b.x) {
		return true;
	}
	else if (a.x > b.x) {
		return false;
	}
	else if (a.z < b.z) {
		return true;
	}
	else {
		return false;
	}
}
void showGeometri(Scene* scene) {
	static int vertNum=0;
	ImGui::InputInt("vertex num", &vertNum);

	static int areaSize = 20;
	ImGui::InputInt("area size", &areaSize);

	static vector<vec3> rverts;
	static Delaunay2d_t del(rverts);
	static vector<int> tris;

	if (ImGui::Button("random vertex")) {
		time_t t;
		//srand((unsigned)time(&t));

		tris.clear();
		rverts.clear();
		for (int idx = 0; idx < vertNum; idx++) {
			float random = ((float)rand()) / (float)RAND_MAX;
			float x = -areaSize + areaSize * 2 * random;

			random = ((float)rand()) / (float)RAND_MAX;
			float z = -areaSize + areaSize * 2 * random;
			
			rverts.push_back( vec3(x, 0, z));

		}
		//sort(rverts.begin(), rverts.end(), vertCmp1);
		//vector<vec3> tmp;
		//for (int idx = 1438; idx <= 1453; idx++) {
		//	tmp.push_back(rverts[idx]);
		//}
		//rverts.clear();
		//for (int idx = 0; idx < tmp.size(); idx++) {
		//	rverts.push_back(tmp[idx]);
		//}
		
	}
	
	static vec3 point;
	ImGui::InputFloat3("vert pos", (float*)&point);
	if (ImGui::Button("init vertex")) {
		rverts.clear();
		rverts.push_back(vec3(-5,0,5));
		rverts.push_back(vec3(-5,0,0));
		rverts.push_back(vec3(-5,0,-5));
		rverts.push_back(vec3(5,0,5));
		rverts.push_back(vec3(5,0,0));


	}

	if (ImGui::Button("add vertex")) {
		rverts.push_back(point);
	}

	if (rverts.size() > 0) {
		scene->renderPoints(rverts, 4, yellow);
	}



	if (ImGui::Button("triangulation")) {
		del.clear();
		tris.clear();



		del.verts.reserve(rverts.size());
		for (int idx = 0; idx < rverts.size(); idx++) {
			del.verts.push_back(DCEL::Vertex(idx, -1,rverts[idx]));
		}
		delaunay2d(rverts,del);
		tris = traveral_delaunay(rverts, del.edges, del.faces);

		for (auto& vert : del.verts) {
			assert(vert.inc != -1);
			assert(del.edges[vert.inc].id != -1);

		}
	}
	static int edge[2];
	ImGui::InputInt2("constrain", edge);

	if (ImGui::Button("add constrain")) {
		addContrainedEdge(edge[0], edge[1], del);
		for (auto& e : del.edges) {
			e.face = -1;
		}
		tris = traveral_delaunay(rverts, del.edges, del.faces);

	}

	if (tris.size() > 0) {
		//scene->renderTris(rverts, tris);
		scene->renderTrisWire(rverts, tris, GL_LINES, yellow);
	}


}



