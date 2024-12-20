#include "runtime/resource/material/whiteMaterial.h"
#include "runtime/resource/resourceManager.h"


WhiteMaterial::WhiteMaterial(){
    _shader = ResourceManager::getResourceManager()->loadShader("assets/shaders/white.vert", "assets/shaders/white.frag");
}

void WhiteMaterial::setColor(const glm::vec3& color){
    _color = color;
}

void WhiteMaterial::apply(const RenderInfo &info){
    _shader->begin();

    _shader->setUniform("model",info.modelInfo.model);
    _shader->setUniform("view",info.cameraInfo.view);
    _shader->setUniform("projection",info.cameraInfo.project);

    _shader->setUniform("color", _color);
}

void WhiteMaterial::finish(){
    _shader->end();
}