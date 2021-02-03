#ifndef GAMMA_PRECOMPILED_HEADER_H
#define GAMMA_PRECOMPILED_HEADER_H

#include <sys/inotify.h>
#include <sys/stat.h>   // stat
#include <unistd.h>
#include <fcntl.h>

#include <cassert>
#include <cstdint>

#include <unordered_map> // @Temporary.
#include <algorithm>     // @Temporary.

#include <iostream> // print
#include <chrono>
#include <cstring> // strncmp.

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

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
#include "globals.h"
#include "array.h"
#include "our_string.h"
#include "gap_buffer.h"
#include "utility.h"

#endif
