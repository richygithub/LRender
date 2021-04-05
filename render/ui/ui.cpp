#include "ui.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "scene/scene.h"

void UI::init(GLFWwindow*window,Scene*scene) {
	const char* glsl_version = "#version 130";
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	_win = window;
	_scene = scene;
}

void UI::updateCamera() {

	Camera& camera = _scene->getCamera();

	ImGui::Begin("Camera");                          // Create a window called "Hello, world!" and append into it.
	//ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
	
	ImGui::ColorEdit3("clear color", (float*)&camera._clearColor);
	ImGui::InputFloat3("camera position",(float*)&camera._position);
	ImGui::InputFloat3("camera lookAt",(float*)&camera._lookAt);



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