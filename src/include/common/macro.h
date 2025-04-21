#pragma once

#include <cstdio>

#define Log(format, ...) \
    printf("\33[1;35m[%s,%d,%s] " format "\33[0m\n", \
        __FILE__, __LINE__, __func__, ## __VA_ARGS__)

#define Err(format, ...) {                              \
    printf("\33[1;31m[%s,%d,%s] " format "\33[0m\n",    \
        __FILE__, __LINE__, __func__, ## __VA_ARGS__);  \
    while(1);                                           \
}

void checkGLError(const char *file, int line, const char *func);
void checkALError(const char *file, int line, const char *func);

#ifdef DEBUG
#define GL_CALL(func)                                                          \
  func;                                                                        \
  checkGLError(__FILE__, __LINE__, __func__);
#define AL_CALL(func)                                                          \
  func;                                                                        \
  checkALError(__FILE__, __LINE__, __func__);
#else
#define GL_CALL(func) func;
#define AL_CALL(func) func;
#endif