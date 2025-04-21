#include "runtime/framework/component/audio/audio.h"
#include <AL/al.h>
#include "common/macro.h"
#include "runtime/resource/audio/audio.h"

void AudioComponent::append(Audio *audio) {
  for (const auto &item : _playlist) {
    if (item == audio) {
      Log("Duplicated audio added. Abort.");
      return;
    }
  }
  _playlist.push_back(audio);
}

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