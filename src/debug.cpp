#include <debug.h>

void Debug_menu::init(GLFWwindow* window) {

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450");
}

void Debug_menu::update() {

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("It works!");
    ImGui::Text("OPENGLRENDERRER ENANGIEN GAME");
    ImGui::End();
}

void Debug_menu::render() const {

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Debug_menu::destroy() {
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}