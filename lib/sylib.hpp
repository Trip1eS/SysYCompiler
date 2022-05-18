#pragma once

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

extern "C" DLLEXPORT int getint();
extern "C" DLLEXPORT void putint(int a);