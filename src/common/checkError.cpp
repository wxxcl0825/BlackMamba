#include "al.h"
#include "common/common.h"
#include "common/macro.h"

#include <string>

void checkGLError(const char *file, int line, const char *func) {
  GLenum errorCode = glGetError();
  std::string error = "";
  if (errorCode != GL_NO_ERROR) {
    switch (errorCode) {
    case GL_INVALID_ENUM:
      error = "INVALID_ENUM";
      break;
    case GL_INVALID_VALUE:
      error = "INVALID_VALUE";
      break;
    case GL_INVALID_OPERATION:
      error = "INVALID_OPERATION";
      break;
    case GL_OUT_OF_MEMORY:
      error = "OUT_OF_MEMORY";
      break;
    default:
      error = "UNKNOWN";
      break;
    }
    Err("OpenGL error: %s, file: %s, line: %d, func: %s", error.c_str(), file,
        line, func);
  }
}

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