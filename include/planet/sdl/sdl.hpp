#pragma once


/// ## Single internal SDL include point
/**
 * Pulls in the SDL3 C API in one place so the SDL include only appears here,
 * keeping every header and source that touches SDL routed through it.
 */
#include <SDL3/SDL.h>
