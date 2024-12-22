#pragma once

#include "runtime/framework/object/gameObject.h"
#include "runtime/framework/component/mesh/mesh.h"
#include "runtime/resource/texture/texture.h"
#include "runtime/resource/material/material.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"

namespace Utils {
glm::mat4 getMat4f(aiMatrix4x4 value);

void decompose(glm::mat4 matrix, glm::vec3& position, glm::vec3& eulerAngle, glm::vec3& scale);

Texture *parseTexture(aiMaterial *aiMat, aiTextureType type, const aiScene *scene, const std::string& rootPath);

std::shared_ptr<MeshComponent> parseMesh(aiMesh *aimesh, const aiScene *scene, const std::string& rootPath, Material &material);

void parse(aiNode *node, const aiScene *scene, GameObject *parent, const std::string& rootPath, Material &material);

GameObject *loadModel(const std::string& path, Material &material);
}