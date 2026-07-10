#pragma once


/// ## Single internal SDL include point
/**
 * Pulls in the SDL C API and undoes its `main` macro redefinition in one place.
 *
 * Centralising the include path here — `<SDL.h>` today, `<SDL3/SDL.h>` once the
 * `PLANET_SDL3` toggle lands — means the SDL2→SDL3 include switch is a single
 * edit in this file rather than a scatter-shot change across every header and
 * source that touches SDL.
 */
#if PLANET_SDL3
#include <SDL3/SDL.h>
#else
#include <SDL.h>
#endif
#undef main
