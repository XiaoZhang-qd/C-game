#ifndef PXPT_PLATFORM_H
#define PXPT_PLATFORM_H


#if defined(_WIN32)

#define PXPT_WINDOWS 1


#elif defined(__ANDROID__)

#define PXPT_ANDROID 1


#elif defined(__APPLE__)

#define PXPT_APPLE 1


#elif defined(__linux__)

#define PXPT_LINUX 1


#elif defined(__FreeBSD__)

#define PXPT_FREEBSD 1


#elif defined(__OpenBSD__)

#define PXPT_OPENBSD 1


#elif defined(__EMSCRIPTEN__)

#define PXPT_WEB 1


#endif


#endif