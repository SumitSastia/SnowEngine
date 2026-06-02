#include <debug.h>

void DebugMenu::init(GLFWwindow* window) {

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450");

    ImGuiIO& io = ImGui::GetIO();

    // io.Fonts->AddFontFromFileTTF("../assets/fonts/FiraSans-Regular.ttf", 18);
    // io.Fonts->AddFontFromFileTTF("../assets/fonts/BlockBlueprint.ttf", 18);
    // io.Fonts->AddFontFromFileTTF("../assets/fonts/BrixelaDEMO-Regular.ttf", 18);
    // io.Fonts->AddFontFromFileTTF("../assets/fonts/NK57-Monospace-Cd-Rg.otf", 18);
}

void DebugMenu::update() {
}

void DebugMenu::beginUI() {

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void DebugMenu::renderTextbox(const std::string title, const std::string desc) {

    ImGui::Begin(title.c_str());
    ImGui::Text("%s", desc.c_str());
    ImGui::End();
}

void DebugMenu::endUI() {

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DebugMenu::destroy() {
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}