#include <renderer.h>

#include <fstream>
#include <sstream>

/*
Returns true if Renderer is successfully initialized.
*/
bool Renderer::init() {

    if (!glfwInit()) {
        std::cerr << "Unable to initialize GLFW!" << std::endl;
        return false;
    }

    // OpenGL Configurations //
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
    
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);


    // ---------- Monitor --------------------- //

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    
    if(!monitor){
        std::cerr << "Unable to fetch the Monitor!" << std::endl;
        return false;
    }
    
    const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);

    // ---------- Main Window ----------------- //

    window = glfwCreateWindow(WIN_W,WIN_H,"FirstWindow 1.0", nullptr, nullptr);

    if(!window){
        std::cerr << "Unable to initialize the window!" << std::endl;
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    
    if(!gladLoadGL()){
        std::cerr << "Unable to Load OpenGL!" << std::endl;
        return -1;
    }

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    
    const unsigned int win_Xpos = videoMode->width/2 - WIN_W/2;
    const unsigned int win_Ypos = videoMode->height/2 - WIN_H/2;

    glfwSetWindowPos(window,win_Xpos,win_Ypos);
    glfwSetWindowAttrib(window,GLFW_RESIZABLE,GLFW_FALSE);

    int frameWidth, frameHeight;
    glfwGetFramebufferSize(window, &frameWidth, &frameHeight);
    glViewport(0,0,frameWidth,frameHeight);
    
    glfwSwapInterval(1); // V-Sync

    glEnable(GL_DEPTH_TEST);

    return true;
}

void Renderer::render() const {

    // Background-Color
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::terminate() {

}

/*
Returns content of a Shader file in string format.
NOTE: This was created as a part of createShader, suggested to be not used externally.
*/
std::string Renderer::loadShaderFile(const char* path) {

    std::ifstream file(path);

    if (!file) {
        std::cerr << "Failed to open the File!" << std::endl;
        return "";
    }

    std::stringstream ss;
    ss << file.rdbuf();

    return ss.str();
}

/*
Returns a Complete Shader with Pipeline
-> Vertex -> Fragment ->
*/
const unsigned int Renderer::createShader(const char* vertPath, const char* fragPath) {

    // Vertex & Fragment Shader //

    std::string vertexStr = loadShaderFile(vertPath);
    std::string fragmentStr = loadShaderFile(fragPath);

    const char* vertexShaderSource = vertexStr.c_str();
    const char* fragmentShaderSource = fragmentStr.c_str();

    const unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    int success;
    char infoLog[512];

    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cout << "ERROR: VERTEX-SHADER COMPILATION FAILED!\n" << infoLog << std::endl;
        return 0;
    }

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cout << "ERROR: FRAGMENT-SHADER COMPILATION FAILED!\n" << infoLog << std::endl;
        return 0;
    }

    // Shader Program //
    const unsigned int shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "ERROR: SHADER-PROGRAM LINKING FAILED!\n" << infoLog << std::endl;
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

/*
Returns a Complete Shader with Pipeline
-> Vertex -> Geometry -> Fragment ->
*/
const unsigned int Renderer::createShader2(const char* vertPath, const char* geomPath, const char* fragPath) {

    // Vertex & Fragment Shader //

    std::string vertexStr = loadShaderFile(vertPath);
    std::string geometryStr = loadShaderFile(geomPath);
    std::string fragmentStr = loadShaderFile(fragPath);

    const char* vertexShaderSource = vertexStr.c_str();
    const char* geometryShaderSource = geometryStr.c_str();
    const char* fragmentShaderSource = fragmentStr.c_str();

    const unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const unsigned int geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
    const unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    int success;
    char infoLog[512];

    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    glShaderSource(geometryShader, 1, &geometryShaderSource, nullptr);
    glCompileShader(geometryShader);

    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cout << "ERROR: VERTEX-SHADER COMPILATION FAILED!\n" << infoLog << std::endl;
        return 0;
    }

    glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(geometryShader, 512, nullptr, infoLog);
        std::cout << "ERROR: GEOMETRY-SHADER COMPILATION FAILED!\n" << infoLog << std::endl;
        return 0;
    }

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cout << "ERROR: FRAGMENT-SHADER COMPILATION FAILED!\n" << infoLog << std::endl;
        return 0;
    }

    // Shader Program //
    const unsigned int shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, geometryShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "ERROR: SHADER-PROGRAM LINKING FAILED!\n" << infoLog << std::endl;
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(geometryShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

/*
Renders the Object (vertices & indices).
NOTE: This method is used in rendering the shadow of the object.
*/
void Renderer::renderShadow(const u_int& VAO, const u_int& indicesCount) {

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, (void*)(0 * sizeof(float)));
    glBindVertexArray(0);
}

/*
Renders the Object (vertices & indices),
instanceCounts = No. of instances of the object to render.
NOTE: This method is used in rendering the shadow of the object.
*/
void Renderer::renderShadowInstanced(const u_int& VAO, const u_int& indicesCount, const u_int& instanceCounts){

    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, nullptr, instanceCounts);
    glBindVertexArray(0);
}