#pragma once

#include "runtime/framework/component/component.h"
#include "runtime/resource/audio/audio.h"
#include <vector>

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