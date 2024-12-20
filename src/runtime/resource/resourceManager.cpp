#include "runtime/resource/resourceManager.h"
#include "runtime/resource/geometry/geometry.h"
#include "runtime/resource/texture/texture.h"

ResourceManager *ResourceManager::_instance = nullptr;

std::unordered_map<std::string, Texture *> ResourceManager::_textureMap = std::unordered_map<std::string, Texture *>();
std::unordered_map<std::string, Shader *> ResourceManager::_shaderMap = std::unordered_map<std::string, Shader *>();

std::vector<Geometry *> ResourceManager::_geometryList;
// std::vector<Material *> ResourceManager::_materialList;

// std::unordered_map<std::string, Model *> ResourceManager::_modelMap = std::unordered_map<std::string, Model *>();

ResourceManager::ResourceManager(){

}

ResourceManager::~ResourceManager(){
    for(auto it = _textureMap.begin(); it != _textureMap.end(); it++){
        delete it->second;
    }
    _textureMap.clear();

    for(auto it = _shaderMap.begin(); it != _shaderMap.end(); it++){
        delete it->second;
    }
    _shaderMap.clear();

    for(auto it = _geometryList.begin(); it != _geometryList.end(); it++){
        delete *it;
    }
    _geometryList.clear();

    // for(auto it = _materialList.begin(); it != _materialList.end(); it++){
    //     delete *it;
    // }
    // _materialList.clear();

    // for(auto it = _modelMap.begin(); it != _modelMap.end(); it++){
    //     delete it->second;
    // }
    // _modelMap.clear();

    if(_instance != nullptr){
        delete _instance;
        _instance = nullptr;

        Log("ResourceManager deleted");
    }else{
        Log("ResourceManager already deleted");
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

// front back top bottom left right 
Geometry *ResourceManager::createBoxGeometry(float size) const{
    std::vector<glm::vec3> vertices = {
        glm::vec3(size/2, size/2, size/2),
        glm::vec3(-size/2, size/2, size/2),
        glm::vec3(-size/2, size/2, -size/2),
        glm::vec3(size/2, size/2, -size/2),

        glm::vec3(size/2, -size/2, size/2),
        glm::vec3(-size/2, -size/2, size/2),
        glm::vec3(-size/2, -size/2, -size/2),
        glm::vec3(size/2, -size/2, -size/2),

        glm::vec3(size/2, size/2, size/2),
        glm::vec3(-size/2, size/2, size/2),
        glm::vec3(-size/2, -size/2, size/2),
        glm::vec3(size/2, -size/2, size/2),

        glm::vec3(size/2, size/2, -size/2),
        glm::vec3(-size/2, size/2, -size/2),
        glm::vec3(-size/2, -size/2, -size/2),
        glm::vec3(size/2, -size/2, -size/2),

        glm::vec3(size/2, size/2, size/2),
        glm::vec3(size/2, size/2, -size/2),
        glm::vec3(size/2, -size/2, -size/2),
        glm::vec3(size/2, -size/2, size/2),

        glm::vec3(-size/2, size/2, size/2),
        glm::vec3(-size/2, size/2, -size/2),
        glm::vec3(-size/2, -size/2, -size/2),
        glm::vec3(-size/2, -size/2, size/2)
    };

    //six faces, each face has 4 vertices
    std::vector<glm::vec2> uvs = {
        glm::vec2(0.0f, 1.0f),
        glm::vec2(1.0f, 1.0f),
        glm::vec2(0.0f, 1.0f),
        glm::vec2(0.0f, 0.0f),

        glm::vec2(0.0f, 1.0f),
        glm::vec2(1.0f, 1.0f),
        glm::vec2(0.0f, 1.0f),
        glm::vec2(0.0f, 0.0f),

        glm::vec2(0.0f, 1.0f),
        glm::vec2(1.0f, 1.0f),
        glm::vec2(0.0f, 1.0f),
        glm::vec2(0.0f, 0.0f),

        glm::vec2(0.0f, 1.0f),
        glm::vec2(1.0f, 1.0f),
        glm::vec2(0.0f, 1.0f),
        glm::vec2(0.0f, 0.0f),

        glm::vec2(0.0f, 1.0f),
        glm::vec2(1.0f, 1.0f),
        glm::vec2(0.0f, 1.0f),
        glm::vec2(0.0f, 0.0f),

        glm::vec2(0.0f, 1.0f),
        glm::vec2(1.0f, 1.0f),
        glm::vec2(0.0f, 1.0f),
        glm::vec2(0.0f, 0.0f)
    };

    std::vector<glm::vec3> normals = {
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),

        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),

        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),

        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),

        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),

        glm::vec3(-1.0f, 0.0f, 0.0f),
        glm::vec3(-1.0f, 0.0f, 0.0f),
        glm::vec3(-1.0f, 0.0f, 0.0f),
        glm::vec3(-1.0f, 0.0f, 0.0f)
    };

    std::vector<unsigned int> indices = {
        0, 1, 2, 0, 2, 3,
        4, 6, 5, 4, 7, 6,
        8, 9, 10, 8, 10, 11,
        12, 14, 13, 12, 15, 14,
        16, 18, 17, 16, 19, 18,
        20, 21, 22, 20, 22, 23
    };

    Geometry *_geometry = new Geometry(vertices, uvs, normals, indices);
    
    _geometryList.push_back(_geometry);

    return _geometry;
}

Geometry *ResourceManager::createPlaneGeometry(float width, float height) const{
    std::vector<glm::vec3> vertices = {
        glm::vec3(width/2, 0.0f, height/2),
        glm::vec3(-width/2, 0.0f, height/2),
        glm::vec3(-width/2, 0.0f, -height/2),
        glm::vec3(width/2, 0.0f, -height/2)
    };

    std::vector<glm::vec2> uvs = {
        glm::vec2(0.0f, 1.0f),
        glm::vec2(1.0f, 1.0f),
        glm::vec2(1.0f, 0.0f),
        glm::vec2(0.0f, 0.0f)
    };

    std::vector<glm::vec3> normals = {
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    };

    std::vector<unsigned int> indices = {
        0, 1, 2, 0, 2, 3
    };

    Geometry *_geometry = new Geometry(vertices, uvs, normals, indices);
    
    _geometryList.push_back(_geometry);

    return _geometry;
}

// Material *ResourceManager::createWhiteMaterial(glm::vec3 color) const{
//     Material *_material = new WhiteMaterial(color);
//     _materialList.push_back(_material);
//     return _material;
// }
