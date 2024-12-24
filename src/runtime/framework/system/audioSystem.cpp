#include "runtime/framework/system/audioSystem.h"

#include <AL/al.h>
#include <AL/alc.h>
#include "runtime/framework/component/audio/audio.h"
#include "runtime/framework/component/camera/camera.h"
#include "runtime/framework/component/physics/rigidBody.h"
#include "runtime/framework/component/transform/transform.h"
#include <memory>

bool AudioSystem::init() {
  ALCdevice *device = alcOpenDevice(NULL);
  ALCcontext *context = alcCreateContext(device, NULL);
  alcMakeContextCurrent(context);
  return context != nullptr;
}

void AudioSystem::dispatch(GameObject *object) {
  if (object->getComponent<AudioComponent>())
    _audios.push_back(object);
  if (object->getComponent<CameraComponent>() &&
      object->getComponent<CameraComponent>()->isMain())
    _listener = object;
}

void AudioSystem::tick() {
  for (auto audio : _audios) {
    std::shared_ptr<AudioComponent> audioComp =
        audio->getComponent<AudioComponent>();
    audioComp->tick();
    glm::vec3 pos =
        audio->getComponent<TransformComponent>()->getPositionWorld();
    alSource3f(audioComp->getAudio()->getAudioID(), AL_POSITION, pos.x, pos.y,
               pos.z);
    std::shared_ptr<RigidBodyComponent> rigidbodyComp =
        audio->getComponent<RigidBodyComponent>();
    if (rigidbodyComp) {
      glm::vec3 v = rigidbodyComp->getLinearVelocity();
      alSource3f(audioComp->getAudio()->getAudioID(), AL_VELOCITY, v.x, v.y,
                 v.z);
    } else {
      alSource3f(audioComp->getAudio()->getAudioID(), AL_VELOCITY, 0, 0, 0);
    }
  }
  glm::vec3 pos =
      _listener->getComponent<TransformComponent>()->getPositionWorld();
  alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
  std::shared_ptr<RigidBodyComponent> rigidbodyComp =
      _listener->getComponent<RigidBodyComponent>();
  if (rigidbodyComp) {
    glm::vec3 v = rigidbodyComp->getLinearVelocity();
    alListener3f(AL_VELOCITY, v.x, v.y, v.z);
  } else {
    alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
  }
  clear();
}

void AudioSystem::clear() { _audios.clear(); }