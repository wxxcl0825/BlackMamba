#include "game/entity/terrain.h"
#include "game/game.h"
#include "runtime/framework/component/mesh/mesh.h"
#include "runtime/framework/component/terrain/terrain.h"
#include "runtime/framework/object/gameObject.h"
#include "runtime/resource/geometry/geometry.h"
#include "runtime/resource/resourceManager.h"
#include <memory>
#include <vector>

Terrain::Terrain(float width, float height, int rez, int repeat,
                 TerrainMaterial *material) {
  ResourceManager *resourceManager =
      Game::getGame()->getEngine()->getResourceManager();

  std::vector<glm::vec3> vertices;
  std::vector<glm::vec2> uvs;
  for (int i = 0; i <= rez - 1; i++) {
    for (int j = 0; j <= rez - 1; j++) {
      vertices.push_back(glm::vec3(-width / 2.0f + width * i / (float)rez, 0.0f,
                                   -height / 2.0f + height * j / (float)rez));
      uvs.push_back(glm::vec2(repeat * i / (float)rez, repeat * j / (float)rez));
      vertices.push_back(glm::vec3(-width / 2.0f + width * (i + 1) / (float)rez,
                                   0.0f,
                                   -height / 2.0f + height * j / (float)rez));
      uvs.push_back(glm::vec2(repeat * (i + 1) / (float)rez, repeat * j / (float)rez));
      vertices.push_back(
          glm::vec3(-width / 2.0f + width * i / (float)rez, 0.0f,
                    -height / 2.0f + height * (j + 1) / (float)rez));
      uvs.push_back(glm::vec2(repeat * i / (float)rez, repeat * (j + 1) / (float)rez));
      vertices.push_back(
          glm::vec3(-width / 2.0f + width * (i + 1) / (float)rez, 0.0f,
                    -height / 2.0f + height * (j + 1) / (float)rez));
      uvs.push_back(glm::vec2(repeat * (i + 1) / (float)rez, repeat * (j + 1) / (float)rez));
    }
  }

  _terrain = new GameObject();
  Geometry *terrainGeometry =
      resourceManager->loadGeometry(std::move(vertices), std::move(uvs));
  std::shared_ptr<MeshComponent> meshComp =
      std::make_shared<MeshComponent>(terrainGeometry, material);
  std::shared_ptr<TerrainComponent> terrainComp =
      std::make_shared<TerrainComponent>(rez);
  _terrain->addComponent(meshComp);
  _terrain->addComponent(terrainComp);
}

Terrain::~Terrain() {
  if (_terrain) {
    delete _terrain;
    _terrain = nullptr;
  }
}