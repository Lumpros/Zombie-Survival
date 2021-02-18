#pragma once

#define NODISCARD [[nodiscard]]

#define PRINT_SDL_ERROR(x) fprintf(stderr, x##" SDL Error: %s", SDL_GetError());
#define PRINT_IMG_ERROR(x) fprintf(stderr, x##" IMG Error: %s", IMG_GetError());
#define PRINT_TTF_ERROR(x) fprintf(stderr, x##" TTF Error: %s", TTF_GetError());
#define PRINT_MIX_ERROR(x) fprintf(stderr, x##" Mix Error: %s", Mix_GetError());

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 800

#define SCREEN_FPS 60
#define SCREEN_TICKS_PER_FRAME 1000 / SCREEN_FPS

#define ZOMBIE_WIDTH 64
#define ZOMBIE_HEIGHT 64

#define BULLET_SIZE 8

#define DEBUG
#undef DEBUG

#ifdef DEBUG
#define Logger puts
#else
#define Logger(x)
#endif