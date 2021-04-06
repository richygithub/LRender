#include "ui.h"
#include "imgui.h"
//#include "imgui_filebrowser.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui_filebrowser.h"
#include "scene/scene.h"
#include <iostream>
#include "utils\meshLoader.h"

void UI::init(GLFWwindow*window,Scene*scene) {
	const char* glsl_version = "#version 130";
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	_win = window;
	_scene = scene;
}
imgui_addons::ImGuiFileBrowser file_dialog;
void UI::updateCamera() {

	Camera& camera = _scene->getCamera();

	ImGui::Begin("Camera");                          // Create a window called "Hello, world!" and append into it.
	//ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
	
	ImGui::ColorEdit3("clear color", (float*)&camera._clearColor);
	ImGui::InputFloat3("camera position",(float*)&camera._position);
	ImGui::InputFloat3("camera lookAt",(float*)&camera._lookAt);

	//load
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
			_scene->addObject(obj);
		}
	}

	//obj
	if (ImGui::BeginTable("split", 2)) {

		auto objs = _scene->getObjs();
		for (auto& it: objs) {
			int id = it.first;
			Object& obj = *it.second;
			ImGui::PushID(id);
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::AlignTextToFramePadding();
			//bool node_open = ImGui::TreeNode("Object", "%d",  idx);
			ImGui::Text("%s",obj.getName().c_str() );

			ImGui::TableSetColumnIndex(1);
			//ImGui::Text("my sailor is rich");
			ImGui::Checkbox("visiable", &obj.visiable);
			ImGui::PopID();

		}

		//for (int idx = 0; idx < 5; idx++) {
		//	ImGui::PushID(idx);
		//	ImGui::TableNextRow();
		//	ImGui::TableSetColumnIndex(0);
		//	ImGui::AlignTextToFramePadding();
		//	//bool node_open = ImGui::TreeNode("Object", "%d",  idx);
		//	ImGui::Text("obj_%d", idx);

		//	ImGui::TableSetColumnIndex(1);
		//	ImGui::Text("my sailor is rich");
		//	ImGui::PopID();
		//}

		ImGui::EndTable();

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