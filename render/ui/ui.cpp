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

using namespace glm;
using namespace std;


void UI::init(GLFWwindow*window,Scene*scene) {
	const char* glsl_version = "#version 130";
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	_win = window;
	_scene = scene;
}
imgui_addons::ImGuiFileBrowser file_dialog;

int mainPlaneId = -1;
void showNavmesh(Scene* scene);
void showObjects(Scene* scene);
void showTileRasterize(Scene* scene);

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

	//load
	if (ImGui::CollapsingHeader("Objects")) {
		showObjects(_scene);
	}
	if (ImGui::CollapsingHeader("Navmesh")) {
		showNavmesh(_scene);
	}



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
void showTileRasterize(Scene* scene) {
	//gMeshBuilder.
	float cellSize = gMeshBuilder._cellSize;
	vector<vec3> verts;
	for (int x = 0; x < gMeshBuilder._width; x++) {
		for (int y = 0; y<gMeshBuilder._height; y++) {
			auto& tile = gMeshBuilder._tiles[x + y * gMeshBuilder._width];
			if (tile.cells != nullptr) {
				float offsetX = tile.x * gMeshBuilder._tileSize * cellSize + gMeshBuilder._min.x;
				float offsetY = tile.y * gMeshBuilder._tileSize * cellSize + gMeshBuilder._min.z;


				for (int cx = 0; cx < tile.size; cx++) {
					for (int cy = 0; cy < tile.size; cy++) {
						if (tile.cells[cx + cy * tile.size].value == 1) {
							//push
							float minx = cx * cellSize + offsetX;
							float miny = cy * cellSize + offsetY;
							vec3 v0(minx, 0, miny);
							vec3 v1(minx+cellSize, 0, miny);
							vec3 v2(minx+cellSize, 0, miny+cellSize);
							vec3 v3(minx, 0, miny+cellSize);

							verts.push_back(v0);
							verts.push_back(v1);
							verts.push_back(v2);

							verts.push_back(v0);
							verts.push_back(v2);
							verts.push_back(v3);


						}
					}
				}
			}
		}
	}
	scene->renderTris(verts);
}

void showNavmesh(Scene*scene) {

	if (ImGui::SliderInt("TileSize", &gBuildCfg.tileSize, 1, 256)) {

	}
	if (ImGui::SliderFloat("CellSize", &gBuildCfg.cellSize, 0.1, 2)) {

	}
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
		gMeshBuilder.debug();
		//for (auto& v : gMeshBuilder._debug) {
		//	scene->addMesh(v);
		//}
	}

	

	showTileBorder(scene);
	showTileRasterize(scene);
}
