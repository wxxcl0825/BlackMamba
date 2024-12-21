#pragma once

#include "common/common.h"
#include "runtime/framework/object/gameObject.h"
#include "runtime/framework/component/mesh/mesh.h"
#include "runtime/resource/texture/texture.h"
#include "runtime/resource/material/material.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

static glm::mat4 getMat4f(aiMatrix4x4 value);

static void decompose(glm::mat4 matrix, glm::vec3& position, glm::vec3& eulerAngle, glm::vec3& scale);

static Texture *parseTexture(aiMaterial *aiMat, aiTextureType type, const aiScene *scene, const std::string& rootPath);

static std::shared_ptr<MeshComponent> parseMesh(aiMesh *aimesh, const aiScene *scene, const std::string& rootPath, Material &material);

static void parse(aiNode *node, const aiScene *scene, GameObject *parent, const std::string& rootPath, Material &material);

GameObject *loadModel(const std::string& path, Material &material);