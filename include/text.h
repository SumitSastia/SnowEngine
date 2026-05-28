#include <ft2build.h>
#include FT_FREETYPE_H

#include <s_math.h>
#include <map>
#include <string>

class Shader;

struct TextCharacter {

    unsigned int textureID;
    glm::ivec2   size;
    glm::ivec2   bearing;
    unsigned int advance;
};

class Text {

    static FT_Library ft;
    static FT_Face font;

    static std::map <char, TextCharacter> Characters;
    
    static Shader*   shader;
    static glm::mat4 projection;

    static uint VAO, VBO;

public:

    static bool init(const char* path);
    static void render(const std::string text, const glm::vec2 pos, const float scale, const glm::vec3 color);
};