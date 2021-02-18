#pragma once

#include <SDL.h>

#include "Texture.h"

namespace PZS
{
	class AudioSlider
	{
	private:
		SDL_Rect slider_rect, handle_rect;
		Texture* slider_texture;

		bool is_enabled = false;
		bool is_pressed = false;

	public:
		explicit AudioSlider(const SDL_Point bottom_left) noexcept;

		void Enable(bool enabled) noexcept { is_enabled = enabled; }

		void Update(void) noexcept;
		void Render(void) noexcept;
	};
}
