#include "runtime/resource/texture/texture.h"
#include "common/common.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "common/macro.h"

Texture::Texture(const std::string& path, unsigned int unit) : _unit(unit){

    int _nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(path.c_str(), &_width, &_height, &_nrChannels, STBI_rgb_alpha);

    if(!data){
        Log("Failed to load texture: %s", path.c_str());
        stbi_image_free(data);
    }

    GL_CALL(glGenTextures(1, &_textureID));
    // activate the texture unit first before binding texture
    GL_CALL(glActiveTexture(GL_TEXTURE0 + _unit));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, _textureID));

    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
    stbi_image_free(data);

    Log("Texture created: %s,and wrapping/filtering options set here", path.c_str());
    // set the texture wrapping/filtering options (on the currently bound texture object)
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

    // set the texture filtering parameters
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

}

// for muitiple pictures in skybox
// right left top bottom back front
Texture::Texture(const std::vector<std::string>& paths, unsigned int unit) : _unit(unit){
    _textureTarget = GL_TEXTURE_CUBE_MAP;

    stbi_set_flip_vertically_on_load(false);

    GL_CALL(glGenTextures(1, &_textureID));
    // activate the texture unit first before binding texture
    GL_CALL(glActiveTexture(GL_TEXTURE0 + _unit));
    GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, _textureID));

    int width, height, nrChannels;
    for (unsigned int i = 0; i < paths.size(); i++){
        unsigned char *data = stbi_load(paths[i].c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
        if (data){
            GL_CALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
            stbi_image_free(data);
        } else {
            Log("Failed to load texture: %s", paths[i].c_str());
            stbi_image_free(data);
        }
    }

    Log("Texture created: %s,and wrapping/filtering options set here", paths[0].c_str());
    // set the texture wrapping/filtering options (on the currently bound texture object)
    GL_CALL(glTexParameteri(_textureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GL_CALL(glTexParameteri(_textureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT));
    GL_CALL(glTexParameteri(_textureTarget, GL_TEXTURE_WRAP_R, GL_REPEAT));

    // set the texture filtering parameters
    GL_CALL(glTexParameteri(_textureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(_textureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
}

Texture::~Texture(){
    if(_textureID != 0){
        GL_CALL(glDeleteTextures(1, &_textureID));
    }
}