#import "@preview/codly:1.1.1": *
#import "@preview/codly-languages:0.1.3": *

#let H(
  title: none
) = {
  codly(
    languages: codly-languages,
    header: [*#title*],
    header-cell-args: (align: center)
    )
}

= 游戏实现

== 相机控制

相机组件仅仅实现了获取ViewMatrix的功能，并没有实现相机对事件的响应。为了实现相机的控制，对相机进行封装，认为相机是一个具有相机组件的游戏对象，并通过向游戏注册回调的方式实现相机的控制。

为了实现多视角的观察，实现自由相机和第一人称相机两种不同的相机类型。

游戏实现层相机声明如下

#H(title: "include/game/entity/camera.h")

```cpp
class Camera {
public:
  enum class Type { Free, FirstPersion, Invalid };

  Camera(std::shared_ptr<CameraComponent> cameraComp, Type type);
  ~Camera();

  void enable();
  void disable();

  GameObject *getCamera() const { return _camera; }

  void setSpeed(float speed) { _speed = speed; }

private:
  GameObject *_camera{nullptr};

  std::shared_ptr<TransformComponent> _transformComp;
  std::shared_ptr<CameraComponent> _cameraComp;

  Type _type{Type::Invalid};

  std::map<int, bool> _keyMap;
  float _speed{1.0f};

  bool _leftMouseDown, _rightMouseDown, _middleMouseDown;
  float _currentX, _currentY;
  float _sensitivity{0.2f};

  float _pitch{0.0f};
  float _yaw{0.0f};

  void onKey(int key, int action, int mods);
  void onMouse(int button, int action, float xpos, float ypos);
  void onCursor(double xpos, double ypos);

  void tick();
  void updatePosition();
  void pitch(float angle);
  void yaw(float angle);
};
```

- 使用Key map方式处理按键，在事件回调中，仅维护当前按下的按键状态；在更新逻辑中，读取Key map，响应键盘事件

相机实现如下

#H(title: "game/entity/camera.cpp")

```cpp
Camera::Camera(std::shared_ptr<CameraComponent> cameraComp, Type type)
    : _type(type) {
  _camera = new GameObject();
  _transformComp = _camera->getComponent<TransformComponent>();
  _cameraComp = cameraComp;
  _camera->addComponent(_cameraComp);

  Game::getGame()->bind(
      Game::KeyBoard{},
      [this](int key, int action, int mods) { onKey(key, action, mods); });
  Game::getGame()->bind(Game::Mouse{}, [this](int button, int action,
                                              int mods) {
    double xpos = 0, ypos = 0;
    Game::getGame()->getEngine()->getWindowSystem()->getCursorPosition(xpos,
                                                                       ypos);
    onMouse(button, action, xpos, ypos);
  });
  Game::getGame()->bind(Game::Cursor{}, [this](double xpos, double ypos) {
    onCursor(xpos, ypos);
    _currentX = xpos, _currentY = ypos;
  });
  _camera->setTick([this]() { tick(); });
}

Camera::~Camera() {
  if (_camera) {
    delete _camera;
    _camera = nullptr;
  }
}

void Camera::enable() {
  if (_camera)
    _camera->getComponent<CameraComponent>()->pick();
}

void Camera::disable() {
  if (_camera)
    _camera->getComponent<CameraComponent>()->put();
}

void Camera::onKey(int key, int action, int mods) {
  _keyMap[key] = action == GLFW_PRESS  || action == GLFW_REPEAT;
}

void Camera::onCursor(double xpos, double ypos) {
  if (_type == Type::Free || _type == Type::FirstPersion) {
    float dx = (xpos - _currentX) * _sensitivity,
          dy = (ypos - _currentY) * _sensitivity;
    pitch(-dy);
    yaw(-dx);
  }
}

void Camera::onMouse(int button, int action, float xpos, float ypos) {
  bool pressed = action == GLFW_PRESS;
  if (pressed)
    _currentX = xpos, _currentY = ypos;
  switch (button) {
  case GLFW_MOUSE_BUTTON_LEFT:
    _leftMouseDown = pressed;
    break;
  case GLFW_MOUSE_BUTTON_RIGHT:
    _rightMouseDown = pressed;
    break;
  case GLFW_MOUSE_BUTTON_MIDDLE:
    _middleMouseDown = pressed;
    break;
  }
}

void Camera::updatePosition() {
  if (_type == Type::Free) {
    glm::vec3 direction{0.0f};
    glm::vec3 front =
        glm::cross(_cameraComp->getUpVec(), _cameraComp->getRightVec());
    glm::vec3 right = _cameraComp->getRightVec();
    if (_keyMap[GLFW_KEY_UP])
      direction += front;
    if (_keyMap[GLFW_KEY_DOWN])
      direction -= front;
    if (_keyMap[GLFW_KEY_LEFT])
      direction -= right;
    if (_keyMap[GLFW_KEY_RIGHT])
      direction += right;
    if (glm::length(direction)) {
      direction = glm::normalize(direction);
      _transformComp->setPositionLocal(_transformComp->getPositionLocal() +
                                  direction * _speed);
    }
  }
}

void Camera::tick() {
  updatePosition();
  if (_type == Type::FirstPersion) {
    if (_camera && _camera->getParent()) {
      glm::mat4 model = _camera->getParent()->getComponent<TransformComponent>()->getModel();
      glm::vec3 pos, euler, scl;
      Utils::decompose(model, pos, euler, scl);
      _camera->getComponent<TransformComponent>()->setAngle(euler);
    }
  }
}

void Camera::pitch(float angle) {
  if (_type == Type::Free) {
    if (_pitch + angle > 89.0f || _pitch + angle < -89.0f)
      return;
    _pitch += angle;
    glm::mat4 rotation =
        glm::rotate(glm::mat4(glm::one<float>()), glm::radians(angle),
                    _cameraComp->getRightVec());
    _cameraComp->setUpVec(glm::vec4(_cameraComp->getUpVec(), 0.0f) * rotation);
  }
  if(_type == Type::FirstPersion) {
    glm::vec3 position, eulerAngle, scale;
    Utils::decompose(_camera->getComponent<TransformComponent>()-> getParentModel(), position, eulerAngle, scale); 
    if (_pitch + angle > fmin(70.0f + eulerAngle.x, 89.0f) || _pitch + angle < fmax(-30.0f + eulerAngle.x, -89.0f))
      return;
    _pitch += angle;
    glm::mat4 rotation =
        glm::rotate(glm::mat4(glm::one<float>()), glm::radians(angle),
                    _cameraComp->getRightVec());
    _cameraComp->setUpVec(glm::vec4(_cameraComp->getUpVec(), 0.0f) * rotation);
  }
}

void Camera::yaw(float angle) {
  if (_type == Type::Free) {
    glm::mat4 rotation =
        glm::rotate(glm::mat4(glm::one<float>()), glm::radians(angle),
                    glm::vec3(0.0f, 1.0f, 0.0f));
    _cameraComp->setUpVec(glm::vec4(_cameraComp->getUpVec(), 0.0f) * rotation);
    _cameraComp->setRightVec(glm::vec4(_cameraComp->getRightVec(), 0.0f) *
                             rotation);
  }
  if (_type == Type::FirstPersion) {
    if(_yaw + angle > 70.0f || _yaw + angle < -70.0f)
      return;
    _yaw += angle;
    glm::mat4 rotation =
        glm::rotate(glm::mat4(glm::one<float>()), glm::radians(angle),
                    glm::vec3(0.0f, 1.0f, 0.0f));
    _cameraComp->setUpVec(glm::vec4(_cameraComp->getUpVec(), 0.0f) * rotation);
    _cameraComp->setRightVec(glm::vec4(_cameraComp->getRightVec(), 0.0f) *
                             rotation);
  }
}
```

- 由于相机组件分为透视和正交投影相机，为了降低设计复杂度，在构造相机对象时。要求从外部传入相机组件
- 根据不同相机的特点，对相机系的基(up, right)进行更新；为了防止视角倒转，对俯仰角范围进行限制
- 为了防止因视角限制，在飞机机头拉高时看不到玻璃，对于第一人称相机，动态获取父亲的ModelView, 并分解得到俯仰角，实时调整视角限制

== 模型导入

为了丰富游戏体验，需要对外部的3D模型导入提供支持。由于模型文件中
数据多以层次结构存在，正好与游戏对象的树状结构形成对应。

同时，为了实现模型文件中变换矩阵的表示与Transform组件中的变换表达之间的转化，需实现工具函数对矩阵进行分解。

由于实现了透明材质，利用材质的继承关系和C++的多态特性，可以根据模型每个部分含有的贴图情况，动态适配适合的材质。

#H(title: "game/utils/utils.cpp")

模型导入实现如下

```cpp
namespace Utils {
GameObject *loadModel(const std::string &path, Material &material) {
  GameObject *root = new GameObject();

  Assimp::Importer importer;
  const aiScene *scene =
      importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals);
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    Err("Assimp error: %s", importer.GetErrorString());
    return nullptr;
  }

  std::size_t lastIndex = path.find_last_of('/');
  auto rootPath = path.substr(0, lastIndex + 1);

  parse(scene->mRootNode, scene, root, rootPath, material);

  return root;
}

void parse(aiNode *ainode, const aiScene *scene, GameObject *parent,
           const std::string &rootPath, Material &material) {
  GameObject *gameObject = new GameObject();

  glm::mat4 localMatrix = getMat4f(ainode->mTransformation);
  glm::vec3 position, enlerAngle, scale;
  decompose(localMatrix, position, enlerAngle, scale);

  gameObject->getComponent<TransformComponent>()->setPositionLocal(position);
  gameObject->getComponent<TransformComponent>()->setAngle(enlerAngle);
  gameObject->getComponent<TransformComponent>()->setScale(scale);

  parent->addChild(gameObject);

  for (std::size_t i = 0; i < ainode->mNumMeshes; i++) {
    aiMesh *aimesh = scene->mMeshes[ainode->mMeshes[i]];
    Material *childMat = material.clone();
    auto mesh = parseMesh(aimesh, scene, rootPath, *childMat);
    auto _object = new GameObject();
    _object->addComponent(mesh);
    gameObject->addChild(_object);
  }

  for (std::size_t i = 0; i < ainode->mNumChildren; i++) {
    Material *childMat = material.clone();
    parse(ainode->mChildren[i], scene, gameObject, rootPath, *childMat);
  }
}

std::shared_ptr<MeshComponent> parseMesh(aiMesh *aimesh, const aiScene *scene,
                                         const std::string &rootPath,
                                         Material &material) {
  ResourceManager *resourceManager =
      Game::getGame()->getEngine()->getResourceManager();

  std::vector<glm::vec3> vertices;
  std::vector<glm::vec2> uvs;
  std::vector<glm::vec3> normals;
  std::vector<unsigned int> indices;

  for (std::size_t i = 0; i < aimesh->mNumVertices; i++) {
    vertices.push_back(glm::vec3(aimesh->mVertices[i].x, aimesh-> mVertices[i].y,
                                 aimesh->mVertices[i].z));
    normals.push_back(glm::vec3(aimesh->mNormals[i].x, aimesh->mNormals[i].y,
                                aimesh->mNormals[i].z));

    if (aimesh->mTextureCoords[0]) {
      uvs.push_back(glm::vec2(aimesh->mTextureCoords[0][i].x,
                              aimesh->mTextureCoords[0][i].y));
    } else {
      uvs.push_back(glm::vec2(0.0f, 0.0f));
    }
  }

  for (std::size_t i = 0; i < aimesh->mNumFaces; i++) {
    aiFace face = aimesh->mFaces[i];
    for (std::size_t j = 0; j < face.mNumIndices; j++) {
      indices.push_back(face.mIndices[j]);
    }
  }

  auto geometry =
      resourceManager->loadGeometry(vertices, uvs, normals, indices);

  Material *finalMaterial = &material;

  if (aimesh->mMaterialIndex >= 0) {
    aiMaterial *aiMat = scene->mMaterials[aimesh->mMaterialIndex];
    material.setDiffuse(parseTexture(aiMat, aiTextureType_DIFFUSE, scene, rootPath));
    aiString aipath;
    if (aiMat->Get(AI_MATKEY_TEXTURE(aiTextureType_OPACITY, 0), aipath) == AI_SUCCESS) {
      TransparentMaterial *transMaterial = nullptr;
      PhongMaterial *phongMaterial = dynamic_cast<PhongMaterial *> (&material);
      if (phongMaterial) {
        transMaterial = new TransparentMaterial(*phongMaterial);
      } else {
        transMaterial = new TransparentMaterial(material);
      }
      transMaterial->setAlphaMap(parseTexture(aiMat, aiTextureType_OPACITY, scene, rootPath));
      finalMaterial = transMaterial;
    }
  } else {
    material.setDiffuse(
        resourceManager->loadTexture("assets/textures/default.jpg"));
  }
  return std::make_shared<MeshComponent>(geometry, finalMaterial);
}

Texture *parseTexture(aiMaterial *aimat, aiTextureType type,
                      const aiScene *scene, const std::string &rootPath) {
  ResourceManager *resourceManager =
      Game::getGame()->getEngine()->getResourceManager();
  aiString aipath;
  aimat->Get(AI_MATKEY_TEXTURE(type, 0), aipath);
  if (aipath.length == 0) {
    return resourceManager->loadTexture("assets/textures/default.jpg");
  }
  const aiTexture *aitexture = scene->GetEmbeddedTexture(aipath.C_Str());
  if (aitexture) {
    unsigned char *dataIn =
        reinterpret_cast<unsigned char *>(aitexture->pcData);
    uint32_t widthIn = aitexture->mWidth;
    uint32_t heightIn = aitexture->mHeight;
    return resourceManager->loadTexture(aipath.C_Str(), dataIn, widthIn,
                                        heightIn);
  } else {
    std::string path = rootPath + aipath.C_Str();
    return resourceManager->loadTexture(path);
  }
}

glm::mat4 getMat4f(aiMatrix4x4 value) {
  return glm::mat4(
      value.a1, value.b1, value.c1, value.d1,
      value.a2, value.b2, value.c2, value.d2,
      value.a3, value.b3, value.c3, value.d3,
      value.a4, value.b4, value.c4, value.d4
  );
}

void decompose(glm::mat4 matrix, glm::vec3 &position, glm::vec3 &eulerAngle,
               glm::vec3 &scale) {
  glm::quat quaternion;
  glm::vec3 skew;
  glm::vec4 perspective;

  glm::decompose(matrix, scale, quaternion, position, skew, perspective);

  glm::mat4 rotation = glm::toMat4(quaternion);
  glm::extractEulerAngleXYZ(rotation, eulerAngle.x, eulerAngle.y, eulerAngle.z);

  eulerAngle.x = glm::degrees(eulerAngle.x);
  eulerAngle.y = glm::degrees(eulerAngle.y);
  eulerAngle.z = glm::degrees(eulerAngle.z);
}
} // namespace Utils
```
- 使用Assimp@assimp 实现模型文件的读取
- 将模型读取为三角面，以适配渲染过程时的DrawCall类型
- 对于漫反射贴图缺失的部分，为其贴上默认贴图

== 飞行控制

飞机在飞行时，需要考虑到重力、空气阻力、升力等因素。首先，飞机自身有一个引擎的动力，推动飞机向机头方向运动，其大小固定，对飞机产生固定大小的加速度；同时，飞机的机翼产生升力，该升力的方向朝向飞机模型的上方，其大小与飞机速度的平方成正比；此外，飞机的机身受到空气阻力的影响，空气阻力的方向与飞机的运动方向相反，大小也与速度成正比；最后，飞机可以通过控制升降舵和方向舵，给飞机自身施加转向力矩，控制飞机的滚转、俯仰和偏航。

玩家类（即飞机）的声明如下：

#H(title: "include/game/entity/player.h")

```cpp
class Player {
public:
  using ExplosionFunc = std::function<float(float)>;

  Player(GameObject *player, const glm::vec3 &position, const glm::vec3 &angle,
         const float &scale, const glm::vec3 &boxSize, const float &mass,
         const float &liftCoefficient, const float &maxAcceleration,
         const float &maxAngularAcceleration);
  ~Player();

  GameObject *getPlayer() const { return _player; }

  void setLiftCoefficient(const float &liftCoefficient) {
    _liftCoefficient = liftCoefficient;
  }
  void setMaxAcceleration(const float &maxAcceleration) {
    _maxAcceleration = maxAcceleration;
  }
  void setMaxAngularAcceleration(const float &maxAngularAcceleration) {
    _maxAngularAcceleration = maxAngularAcceleration;
  }

private:
  GameObject *_player{nullptr};

  glm::vec3 _position;
  glm::vec3 _angle;
  float _scale;

  glm::vec3 _boxSize;
  float _mass;
  float _liftCoefficient;
  float _maxAcceleration;
  float _maxAngularAcceleration;

  void tick();
};
```

其受力模型的实现如下：

#H(title: "game/entity/player.cpp")

```cpp
void Player::tick() {
  if (_onExplode) {
    if (glfwGetTime() >= _explodeEnd) {
      _player->getParent()->removeChild(_player);
      return;
    }
    _explode(_player, _explosionFunc(glfwGetTime() - _explodeStart));
    return;
  }

  if(_canExplode && _player->getComponent<RigidBodyComponent>()->getLinearVelocity().y < -0.6f && _player->getComponent<RigidBodyComponent>()->isCollide()){
    explode();
    _canExplode = false;
  }

  glm::vec3 forward = _player->getComponent<TransformComponent>()-> getForwardVec();
  glm::vec3 right = _player->getComponent<TransformComponent>()-> getRightVec();
  glm::vec3 up = _player->getComponent<TransformComponent>()->getUpVec();

  // set force
  glm::vec3 engineForce = glm::vec3{0.0f};
  if (_keyMap[GLFW_KEY_SPACE]) {
    engineForce += forward * _maxAcceleration * _mass;
  }

  glm::vec3 liftForce = glm::vec3{0.0f};
  glm::vec3 linearVelocity =
      _player->getComponent<RigidBodyComponent>()->getLinearVelocity();
  liftForce = static_cast<float>(std::pow(_liftCoefficient, 2)) *
              glm::length(linearVelocity) * up;

  if(glm::length(liftForce) > glm::length(engineForce) * 0.5f)
    liftForce = glm::normalize(liftForce) * glm::length(engineForce) * 0.5f;

  _player->getComponent<RigidBodyComponent>()->setForce(engineForce +
                                                        liftForce);

  // set torque
  glm::vec3 torque = glm::vec3{0.0f};
  if (_keyMap[GLFW_KEY_W]) {
    torque +=
        right * _maxAngularAcceleration * (1.0f / 12.0f) *
        static_cast<float>(std::pow(_boxSize.y, 2) + std::pow(_boxSize.z, 2)) * _mass;
  }
  if (_keyMap[GLFW_KEY_S]) {
    torque +=
        -right * _maxAngularAcceleration * (1.0f / 12.0f) *
        static_cast<float>(std::pow(_boxSize.y, 2) + std::pow(_boxSize.z, 2)) * _mass;
  }
  if (_keyMap[GLFW_KEY_A]) {
    torque +=
        -forward * _maxAngularAcceleration * (1.0f / 12.0f) *
        static_cast<float>(std::pow(_boxSize.x, 2) + std::pow(_boxSize.y, 2)) * _mass;
  }
  if (_keyMap[GLFW_KEY_D]) {
    torque +=
        forward * _maxAngularAcceleration * (1.0f / 12.0f) *
        static_cast<float>(std::pow(_boxSize.x, 2) + std::pow(_boxSize.y, 2)) * _mass;
  }

  _player->getComponent<RigidBodyComponent>()->setTorque(torque);
}
```

== 场景构建

游戏场景十分简单，具有6个游戏对象：场景、自由相机、第一人称相机、玩家、天空盒、地形。

由于天空盒实现的原理是在相机外围蒙上立方体，故让天空盒始终作为主相机的子节点，并设置本地位移为0，从而实现天空盒对相机的追随。

对于第一人称相机，为了沉浸式体验开飞机的感觉，需让主相机成为模型的子节点。

对于视角切换，将其注册为键盘回调即可。

实现如下

#H(title: "ame/game.cpp")

```cpp
void Game::setupScene() {
  _scene = new GameObject();

  Camera *camera = new Camera(
      std::make_shared<CameraComponent>(
          45.0f, _engine->getWindowSystem()->getAspect(), 0.1f, 10000.0f),
      Camera::Type::Free);

  camera->setSpeed(0.3f);

  _scene->addComponent(
      std::make_shared<LightComponent>(glm::vec3(1.0f, 1.0f, 1.0f)));

  Skybox *skybox = new Skybox(
      {"assets/textures/skybox/right.jpg", "assets/textures/skybox/left.jpg",
       "assets/textures/skybox/top.jpg", "assets/textures/skybox/bottom.jpg",
       "assets/textures/skybox/back.jpg", "assets/textures/skybox/front.jpg"});

  skybox->bind(camera->getCamera());

  _scene->addChild(camera->getCamera());

  ExplosionMaterial *explosionMat = new ExplosionMaterial();
  GameObject *model =
      Utils::loadModel("assets/models/fighter/fighter.obj", *explosionMat);

  Player *player = new Player(model, glm::vec3(0.0f, 3.0f, 0.0f),
                              glm::vec3(0.5f, 0.0f, 0.5f), 1.0f,
                              glm::vec3(1.0f), 1.0f, 0.7f, 10.0f, 0.02f);

  GameObject* light = new GameObject();
  light->addComponent(std::make_shared<LightComponent>(glm::vec3(0.9f, 0.54f, 0.0f), glm::normalize(glm::vec3(0.0f, -0.2f, -1.0f)), 0.3, 45, 60));
  light->getComponent<TransformComponent>()->setPositionLocal(glm::vec3(0.0f, -1.0f, 5.0f));
  player->getPlayer()->addChild(light);

  std::shared_ptr<AudioComponent> audioComp =
      std::make_shared<AudioComponent>();
  audioComp->append(
      _engine->getResourceManager()->loadAudio("assets/audios/engine.ogg"));
  audioComp->start();
  audioComp->setMode(AudioComponent::Mode::REPEAT_SINGEL);
  player->getPlayer()->addComponent(audioComp);

  Camera *fCamera = new Camera(
      std::make_shared<CameraComponent>(
          60.0f, _engine->getWindowSystem()->getAspect(), 0.1f, 10000.0f),
      Camera::Type::FirstPersion);
  fCamera->disable();
  fCamera->getCamera()->getComponent<TransformComponent>()->setPositionLocal(
      glm::vec3(0.0f, 1.75f, 4.15f));
  fCamera->getCamera()->getComponent<CameraComponent>()->setRightVec(
      glm::vec3(-1.0f, 0.0f, 0.0f));
  fCamera->getCamera()->getComponent<CameraComponent>()->setUpVec(
      glm::normalize(glm::vec3(0.0f, 1.0f, 0.5f)));

  player->getPlayer()->addChild(fCamera->getCamera());

  Game::getGame()->bind(KeyBoard{}, [player, camera, fCamera,
                                     skybox](int key, int action, int mods) {
    if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
      camera->disable();
      fCamera->enable();
      skybox->bind(fCamera->getCamera());
    }
    if ((key == GLFW_KEY_1 && action == GLFW_PRESS)) {
      fCamera->disable();
      camera->enable();
      skybox->bind(camera->getCamera());
      camera->getCamera()->getComponent<TransformComponent>()->setPositionLocal(
          glm::inverse(camera->getCamera()
                           ->getComponent<TransformComponent>()
                           ->getParentModel()) *
          glm::vec4((player->getPlayer()
                         ->getComponent<TransformComponent>()
                         ->getPositionWorld() +
                     glm::vec3(0.0f, 5.0f, 0.0f)),
                    1.0f));
    }
  });
  skybox->getSkybox()->setTick([camera, player, skybox]() {
    if (!player->getPlayer()->getParent())
      skybox->bind(camera->getCamera());
  });

  TerrainMaterial *terrainMat = new TerrainMaterial();
  terrainMat->setDiffuse(_engine->getResourceManager()->loadTexture(
      "assets/textures/terrain/diffuse.jpg"));
  terrainMat->setHeightMap(_engine->getResourceManager()->loadTexture(
      "assets/textures/terrain/heightMap.png"));
  Terrain *terrain = new Terrain(10000.0f, 10000.0f, 200, 100, terrainMat);
  terrain->getTerrain()->addComponent(std::make_shared<RigidBodyComponent>(
      JPH::EMotionType::Static, Layers::STATIC, 10000.0f, 1.f, 10000.0f, 1.0f));

  _scene->addChild(player->getPlayer());
  _scene->addChild(terrain->getTerrain());

  player->setExplosionFunc([](float x) -> float { return 10 * log(x + 1); });

  _engine->setMainLoop([] {});
}
```
- 值得注意的是，在飞机爆炸销毁后，飞机将从场景的子节点列表中移除，从而不再参与渲染流程；而与此同时，作为飞机子节点的相机、作为相机子节点的天空盒也会被从游戏对象树中移除，即此时会陷入主相机缺失的场景。渲染系统会感知到主相机的缺失，并自动启用副相机，但天空盒并不会随之切换，因为游戏对象是否为天空盒对渲染系统而言不可见。故需为天空盒注册回调，动态检测飞机是否被移出游戏对象树，一旦发现飞机已被销毁，自动绑定到自由相机上，防止在爆炸瞬间天空盒出现渲染异常