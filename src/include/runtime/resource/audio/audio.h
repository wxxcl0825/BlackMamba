#pragma once

#include <AL/al.h>
#include <string>

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