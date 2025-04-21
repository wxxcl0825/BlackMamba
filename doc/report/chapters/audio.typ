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

= 音效系统

音效是营造沉浸式游戏体验不可或缺的重要成分。通过声音的大小、频率变化，可以从侧面反映出游戏对象的位置与运行状态。

OpenAL@openal_soft 是一款跨平台的3D音频API，其接口设计上借鉴了OpenGL的实现标准，具备缓冲区、绑定等概念，非常适合为游戏提供音效系统开发支持。

整体音效系统以组件——系统形式实现，通过在游戏对象上挂载音频组件，在系统更新时检测音频播放状态，并对状态做出更新。

由于OpenAL提供了OpenGL类似的接口，同样为其实现检错

#H(title: "include/common/macro.h")

```cpp
#ifdef DEBUG
#define AL_CALL(func)                                                         \
  func;                                                                       \
  checkALError(__FILE__, __LINE__, __func__);
#else
#define AL_CALL(func) func;
#endif
```

#H(title: "common/checkError.cpp")
```cpp
void checkALError(const char *file, int line, const char *func) {
  ALenum err = alGetError();
  if (err != AL_NO_ERROR) {
    std::string errorStr = "AL_UNKNOWN_ERROR";
    switch (err) {
    case AL_INVALID_NAME:
      errorStr = "AL_INVALID_NAME";
      break;
    case AL_INVALID_ENUM:
      errorStr = "AL_INVALID_ENUM";
      break;
    case AL_INVALID_VALUE:
      errorStr = "AL_INVALID_VALUE";
      break;
    case AL_INVALID_OPERATION:
      errorStr = "AL_INVALID_OPERATION";
      break;
    case AL_OUT_OF_MEMORY:
      errorStr = "AL_OUT_OF_MEMORY";
      break;
    }
    Err("OpenAL error: %s, file: %s, line: %d, func: %s", errorStr.c_str(),
        file, line, func);
  }
}
```

== 音频资源

由于OpenAL本身不具备音频载入功能，使用stb/vorbis@stb 实现音频数据的载入。

首先定义音频资源，声明如下


#H(title: "include/runtime/resource/audio/audio.h")
```cpp
class Audio {
  friend class ResourceManager;

public:
  ALuint getAudioID() const { return _audioID; }

private:
  ALuint _audioID{0};
  ALuint _buffer{0};

  Audio(const std::string &path);
  ~Audio();
};
```

在构造函数中实现音频资源的载入

#H(title: "runtime/resource/audio/audio.cpp")
```cpp
Audio::Audio(const std::string &path) {
  int channels = 0;
  int sampleRate = 0;
  short *data = nullptr;
  int samples =
      stb_vorbis_decode_filename(path.c_str(), &channels, &sampleRate, &data);
  if (samples < 0) {
    Log("Failed to load audio: %s", path.c_str());
  } else {
    AL_CALL(alGenBuffers(1, &_buffer));
    ALenum format = (channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
    AL_CALL(alBufferData(_buffer, format, data,
                         samples * channels * sizeof(short), sampleRate));
    AL_CALL(alGenSources(1, &_audioID));
    AL_CALL(alSourcei(_audioID, AL_BUFFER, _buffer));
    free(data);
  }
}
```
== 音频组件

音频组件声明如下

#H(title: "include/runtime/framework/component/audio/audio.h")

```cpp
class AudioComponent : public Component {
public:
  enum class Mode { SINGLE, REPEAT_SINGEL, REPEAT_LIST, INVALID };

  AudioComponent() = default;
  ~AudioComponent() override {}

  void start() { _isPlaying = true; }
  void stop() { _isPlaying = false; }
  void append(Audio *audio);

  void tick();

  Audio *getAudio() const { return _playlist[_current]; }

  void setMode(Mode mode) { _mode = mode; }

private:
  Mode _mode{Mode::INVALID};

  std::vector<Audio *> _playlist{};
  int _current{0};
  bool _isPlaying{false};

  int getNext();
};
```
- 在组件内部维护音频播放列表
- 提供3种音频播放模式：单曲，单曲循环，列表循环

其更新逻辑实现如下，根据音频播放模式决定下一首播放的曲目

#H(title: "runtime/framework/component/audio/audio.cpp")

```cpp
void AudioComponent::tick() {
  ALint state;
  Audio *audio = _playlist[_current];
  alGetSourcei(audio->getAudioID(), AL_SOURCE_STATE, &state);
  if ((state == AL_PLAYING && _isPlaying) ||
      (state != AL_PLAYING && !_isPlaying))
    return;
  if (!_isPlaying)
    alSourcePause(audio->getAudioID());
  else if (state == AL_PAUSED || state == AL_INITIAL)
    alSourcePlay(audio->getAudioID());
  else if (_mode == Mode::SINGLE)
    _isPlaying = false;
  else
    alSourcePlay(_playlist[getNext()]->getAudioID());
}

int AudioComponent::getNext() {
  if (_mode == Mode::REPEAT_SINGEL)
    return _current;
  if (_mode == Mode::REPEAT_LIST)
    return (_current + 1) % _playlist.size();
  Log("Unknown audio mode.");
  return 0;
}
```

- 无需每次都进行音频播放状态更新，通过维护期望播放状态`_isPlaying`, 通过与实际播放状态`state`进行比较，若一致则不进行更新，否则判断是否播放下一曲，决定下一曲是哪一曲

== 系统实现

音效系统声明如下

#H(title: "include/runtime/framework/system/audioSystem.h")

```cpp
class AudioSystem {
public:
  AudioSystem() = default;
  ~AudioSystem() = default;
 
  
  bool init();
  void dispatch(GameObject *object);
  void tick();

private:
  std::vector<GameObject *> _audios;
  GameObject* _listener;

  void clear();
};
```
- OpenAL提供Listener概念，用于设定用户位置；为保证音画一致，在分发时，直接将当前的主相机设为Listener

音效系统的初始化实现如下

#H(title: "runtime/framework/system/audioSystem.cpp")

```cpp
bool AudioSystem::init() {
  ALCdevice *device = alcOpenDevice(NULL);
  ALCcontext *context = alcCreateContext(device, NULL);
  alcMakeContextCurrent(context);
  return context != nullptr;
}
```
- 先对音频设备进行初始化，再对其进行绑定
- 处于简化，此处并未考虑音频播放设备的切换

更新逻辑实现如下

#H(title: "runtime/framework/system/audioSystem.cpp")

```cpp
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
```

- 分发阶段，音效系统捕获所有具有音频组件的游戏对象，同时寻找主相机，并设置为聆听者
- 更新阶段，对音频组件进行更新，并设置其位置、速度等参数