#include "common/common.h"
#include "common/macro.h"

void checkError(const char *file, int line, const char *func) {
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
    Err("OpenGL error: %s, file: %s, line: %d, func: %s", error.c_str(), file, line, func);
  }
}