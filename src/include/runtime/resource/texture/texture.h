#pragma once

#include "common/common.h"
#include <string>
#include <vector>

class Texture {
    friend class ResourceManager;

public:
    int getWidth() const { return _width; };
    int getHeight() const { return _height; };
    GLuint getTextureID() const { return _textureID; };
    unsigned int getUnit() const { return _unit; };

    void bind() const;

private:
    int _width{0};
    int _height{0};
    GLuint _textureID{0};
    unsigned int _unit{0};
    unsigned int _textureTarget{GL_TEXTURE_2D};

    Texture(const std::string& path, unsigned int unit);
    Texture(const std::vector<std::string>& paths, unsigned int unit);
    Texture(unsigned char* dataIn, uint32_t widthIn, uint32_t heightIn, unsigned int unit);
    ~Texture();

};
