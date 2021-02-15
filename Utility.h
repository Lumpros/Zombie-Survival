#pragma once

#include <SDL.h>
#include <cstdio>

using namespace std;

bool operator!=(const SDL_Rect rect1, const SDL_Rect rect2)
{
	return
		rect1.x != rect2.x ||
		rect1.y != rect2.y ||
		rect1.w != rect2.w ||
		rect1.h != rect2.h;
}