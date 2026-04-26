#include <shader.h>

#include <iostream>
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// ------------------------------ Class Functions ------------------------------------ //

std::filesystem::path base = std::filesystem::current_path();

/*
Loads the image and allocate it into the Memory.
NOTE: "path" should start with '/' and rest should continue after the Main Directory (/SnowEngine).
*/
void texture2D::load(const char* path){

    std::string path_str(path);
    std::string base_str = base.string();

    base_str.erase(base_str.size() - 5);

    std::string finalPath = base_str + path_str;

    pixelData = stbi_load(finalPath.c_str(), &width, &height, nullptr, 4);

    if(!pixelData){
        std::cerr << "Failed to Load Image!\n" << finalPath << std::endl;
        return;
    }

    glGenTextures(1,&textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(pixelData);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void texture2D::destroy() {

}

// ------------------------------ Shader Uniform Setter ------------------------------ //

void setBool(const unsigned int& shaderProgram, const char* target, const bool& value) {

    glUniform1i(
        glGetUniformLocation(shaderProgram, target),
        value
    );
}

void setInt(const unsigned int &shaderProgram, const char* target, const int &value){

    glUniform1i(
        glGetUniformLocation(shaderProgram, target),
        value
    );
}

void setFloat(const unsigned int &shaderProgram, const char* target, const float &value){

    glUniform1f(
        glGetUniformLocation(shaderProgram, target),
        value
    );
}

void setVec3(const unsigned int &shaderProgram, const char* target, const glm::vec3 &vector){

    glUniform3fv(
        glGetUniformLocation(shaderProgram, target),
        1,
        glm::value_ptr(vector)
    );
}

void setMat3(const unsigned int &shaderProgram, const char* target, const glm::mat3 &matrix){

    glUniformMatrix3fv(
        glGetUniformLocation(shaderProgram, target),
        1,
        GL_FALSE,
        glm::value_ptr(matrix)
    );
}

void setMat4(const unsigned int &shaderProgram, const char* target, const glm::mat4 &matrix){

    glUniformMatrix4fv(
        glGetUniformLocation(shaderProgram, target),
        1,
        GL_FALSE,
        glm::value_ptr(matrix)
    );
}