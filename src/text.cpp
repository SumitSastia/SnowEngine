#include <text.h>
#include <renderer.h>
#include <shader.h>

#include <iostream>

FT_Library Text::ft   {};
FT_Face    Text::font {};

uint Text::VAO = 0;
uint Text::VBO = 0;

Shader* Text::shader = nullptr;
std::map <char, TextCharacter> Text::Characters {};

bool Text::init(const char* path) {

    if (FT_Init_FreeType(&ft)) {

        std::cout << "Failed to initialize FreeType Library!" << std::endl;
        return false;
    }

    if (FT_New_Face(ft, path, 0, &font)) {

        std::cout << "Failed to load font file: " << path << std::endl;
        return false;
    }

    FT_Set_Pixel_Sizes(font, 0, 48);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    for (unsigned char c = 0; c < 128; c++) {

        // load character glyph 
        if (FT_Load_Char(font, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }

        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            font->glyph->bitmap.width,
            font->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            font->glyph->bitmap.buffer
        );

        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // now store character for later use
        TextCharacter character = {
            texture, 
            glm::ivec2(font->glyph->bitmap.width, font->glyph->bitmap.rows),
            glm::ivec2(font->glyph->bitmap_left, font->glyph->bitmap_top),
            static_cast<unsigned int>(font->glyph->advance.x)
        };

        Characters.insert(std::pair<char, TextCharacter>(c, character));
    }

    FT_Done_Face(font);
    FT_Done_FreeType(ft);

    shader = new Shader(
        "../shaders/text/text.vert",
        "../shaders/text/text.frag"
    );

    glm::mat4 projection = glm::ortho(0.0f, (float)WIN_W, 0.0f, (float)WIN_H);

    shader->use();
    shader->setMat4("projection", projection);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return true;
}

void Text::render(const std::string text, glm::vec2 pos, const float scale, const glm::vec3 color) {

    // static glm::mat4 projection = glm::ortho(0.0f, (float)WIN_W, 0.0f, (float)WIN_H);

    glEnable(GL_BLEND);

    shader->use();
    shader->setVec3("textColor", color);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    std::string::const_iterator c;

    for (c = text.begin(); c != text.end(); c++) {

        TextCharacter ch = Characters[*c];

        float xpos = pos.x + ch.bearing.x * scale;
        float ypos = pos.y - (ch.size.y - ch.bearing.y) * scale;

        float w = ch.size.x * scale;
        float h = ch.size.y * scale;

        float vertices[6][4] = {

            { xpos,   ypos+h, 0.0f, 0.0f },
            { xpos,   ypos,   0.0f, 1.0f },
            { xpos+w, ypos,   1.0f, 1.0f },

            { xpos,   ypos+h, 0.0f, 0.0f },
            { xpos+w, ypos,   1.0f, 1.0f },
            { xpos+w, ypos+h, 1.0f, 0.0f }
        };

        glBindTexture(GL_TEXTURE_2D, ch.textureID);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);
        pos.x += (ch.advance >> 6) * scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDisable(GL_BLEND);
}