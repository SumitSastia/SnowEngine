#include <debug.h>
#include <texture.h>

std::vector <DebugMenu::TextureDataPair> DebugMenu::textureInit_data {};
std::vector <DebugMenu::ColorDataPair>   DebugMenu::colorInit_data   {};

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

void DebugMenu::printAssetsInitTime() {

    const int max_padding = 18;

    for (const auto& data : textureInit_data) {
        
        std::cout << "Texture Loaded: "
                  << std::left    << std::setw(max_padding) << std::setfill(' ') << data.object
                  << ", Handle: " << data.handle
                  << '\n';
    }

    for (const auto& data : colorInit_data) {

        std::cout << "Color Loaded: "
                  << std::setw(8) << std::setfill('0') << std::hex << data.color
                  << ", Handle: " << std::dec          << data.handle
                  << '\n';
    }

    textureInit_data.clear();
    colorInit_data.clear();
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