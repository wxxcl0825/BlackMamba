#include "runtime/resource/resourceManager.h"
#include "runtime/resource/geometry/geometry.h"
#include "runtime/resource/texture/texture.h"

ResourceManager *ResourceManager::_instance = nullptr;
std::unordered_map<std::string, Texture *> ResourceManager::_textureMap = std::unordered_map<std::string, Texture *>();
std::unordered_map<std::string, Model *> ResourceManager::_modelMap = std::unordered_map<std::string, Model *>();
std::unordered_map<std::string, Shader *> ResourceManager::_shaderMap = std::unordered_map<std::string, Shader *>();

ResourceManager::ResourceManager(){

}

ResourceManager::~ResourceManager(){
    for(auto it = _textureMap.begin(); it != _textureMap.end(); it++){
        delete it->second;
    }
    for(auto it = _modelMap.begin(); it != _modelMap.end(); it++){
        delete it->second;
    }
    for(auto it = _shaderMap.begin(); it != _shaderMap.end(); it++){
        delete it->second;
    }
}

ResourceManager *ResourceManager::getResourceManager(){
    if(_instance == nullptr){
        _instance = new ResourceManager();
    }
    return _instance;
}

Texture *ResourceManager::loadTexture(const std::string& filePath) const{
    if(_textureMap.find(filePath) == _textureMap.end()){
        Log("Loading texture: %s", filePath.c_str());
        Log("Binding texture to unit: %d", _textureMap.size());
        Texture *_texture = new Texture(filePath, _textureMap.size());
        _textureMap[filePath] = _texture;
    }
    return _textureMap[filePath];
}

Texture *ResourceManager::loadTexture(const std::vector<std::string>& filePaths) const{
    if(_textureMap.find(filePaths[0]) == _textureMap.end()){
        Log("Loading texture: %s", filePaths[0].c_str());
        Log("Binding texture to unit: %d", _textureMap.size());
        Texture *_texture = new Texture(filePaths, _textureMap.size());
        _textureMap[filePaths[0]] = _texture;
    }
    return _textureMap[filePaths[0]];
}

Shader *ResourceManager::loadShader(const std::string& vertexPath, const std::string& fragmentPath) const{
    if(_shaderMap.find(vertexPath + fragmentPath) == _shaderMap.end()){
        Log("Loading shader: %s and %s", vertexPath.c_str(), fragmentPath.c_str());
        Shader *_shader = new Shader(vertexPath, fragmentPath);
        _shaderMap[vertexPath + fragmentPath] = _shader;
    }
    return _shaderMap[vertexPath + fragmentPath];
}

Geometry *ResourceManager::createBoxGeometry(float size) const{
    std::vector<glm::vec3> vertices = {
        glm::vec3(-size, -size, size),
        glm::vec3(size, -size, size),
        glm::vec3(size, size, size),
        glm::vec3(-size, size, size),
        glm::vec3(-size, -size, -size),
        glm::vec3(size, -size, -size),
        glm::vec3(size, size, -size),
        glm::vec3(-size, size, -size)
    };

    std::vector<glm::vec2> uvs = {
        glm::vec2(0.0f, 0.0f),
        glm::vec2(1.0f, 0.0f),
        glm::vec2(1.0f, 1.0f),
        glm::vec2(0.0f, 1.0f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(1.0f, 0.0f),
        glm::vec2(1.0f, 1.0f),
        glm::vec2(0.0f, 1.0f)
    };

    std::vector<glm::vec3> normals = {
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f)
    };

    std::vector<unsigned int> indices = {
        0, 1, 2, 2, 3, 0,
        1, 5, 6, 6, 2, 1,
        7, 6, 5, 5, 4, 7,
        4, 0, 3, 3, 7, 4,
        3, 2, 6, 6, 7, 3,
        4, 5, 1, 1, 0, 4
    };

    return new Geometry(vertices, uvs, normals, indices);
}

Model *ResourceManager::loadModel(const std::string& filePath) const{
    if(_modelMap.find(filePath) == _modelMap.end()){
        Log("Loading model: %s,not complement yet", filePath.c_str());
        // Model *_model = new Model(filePath);
        // _modelMap[filePath] = _model;
    }
    return _modelMap[filePath];
}
