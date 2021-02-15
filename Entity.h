#pragma once

#include <SDL.h>

#include "Texture.h"

namespace PZS
{
	class Entity
	{
	public:
		SDL_Rect hitbox = { 0, 0, 0, 0 };
		Texture* mTexture = nullptr;

	public:
		virtual void Update(void) noexcept = 0;
		virtual void Render(void) noexcept = 0;
	};
}