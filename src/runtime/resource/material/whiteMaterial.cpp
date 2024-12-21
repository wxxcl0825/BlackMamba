#include "runtime/resource/material/whiteMaterial.h"
#include "runtime/engine.h"
#include "runtime/resource/resourceManager.h"


WhiteMaterial::WhiteMaterial(){
    ResourceManager *resourceManager = Engine::getEngine()->getResourceManager();
    _shader = resourceManager->loadShader("assets/shaders/white.vert", "assets/shaders/white.frag");
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