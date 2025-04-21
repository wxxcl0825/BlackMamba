#include "runtime/resource/audio/audio.h"

#include "AL/al.h"
#include "common/macro.h"
#include "stb_vorbis.c"

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

Audio::~Audio() {
  if (_audioID)
    AL_CALL(alDeleteSources(1, &_audioID));
  if (_buffer)
    AL_CALL(alDeleteBuffers(1, &_buffer));
}