#ifndef GAMMA_PRECOMPILED_HEADER_H
#define GAMMA_PRECOMPILED_HEADER_H

#include <sys/inotify.h>
#include <unistd.h>
#include <fcntl.h>

#include <cassert>

#include <algorithm> // @Temporary:
#include <iostream>  // cout
#include <chrono>
#include <cstring>   // strncmp

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <cstdint>
typedef int64_t  s64;
typedef int32_t  s32;
typedef int16_t  s16;
typedef int8_t   s8;
typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;
typedef float    f32;
typedef double   f64;

#include "fwd.h"
#include "utility.h"
#include "globals.h"
#include "array.h"
#include "gap_buffer.h"

#endif
