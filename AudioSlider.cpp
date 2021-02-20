#include "AudioSlider.h"
#include "Resources.h"

#include <Windows.h>

#define SLIDER_HEIGHT 128
#define SLIDER_WIDTH  32

#define DIFF 4

PZS::AudioSlider::AudioSlider(const SDL_Point bot_left) noexcept
{
	slider_rect = {
		bot_left.x,
		bot_left.y - SLIDER_HEIGHT,
		SLIDER_WIDTH,
		SLIDER_HEIGHT
	};

#define WIDTH 26

	// x 14
	// w 36
	handle_rect = { slider_rect.x + SLIDER_WIDTH / 2 - WIDTH / 2, slider_rect.y + DIFF, WIDTH, 10 };
	
	slider_texture = Resources::GetInstance()->gTextures.Get("slider");
}

void PZS::AudioSlider::Render(void) noexcept
{
	SDL_Renderer* renderer = Renderer::GetInstance()->mRenderer;

	if (is_enabled)
	{
		slider_texture->Render(slider_rect);

		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderFillRect(renderer, &handle_rect);
	}
}

void PZS::AudioSlider::Update(void) noexcept
{
	if (!is_enabled)
		return;

	SDL_Point mouse_pos;

	bool is_clicked = SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y) & SDL_BUTTON_LMASK;

	if (!is_clicked)
		is_pressed = false;

	if (SDL_PointInRect(&mouse_pos, &slider_rect))
	{
		is_pressed = true;

		SetCursor(LoadCursor(NULL, IDC_HAND));

		if (is_clicked)
		{
			int temporary_rect = mouse_pos.y - handle_rect.h / 2;

			if (temporary_rect == handle_rect.y)
				return;

			handle_rect.y = temporary_rect;

			int new_volume = MIX_MAX_VOLUME - (handle_rect.y + handle_rect.h / 2 - slider_rect.y);

			if (new_volume > MIX_MAX_VOLUME) 
				new_volume = MIX_MAX_VOLUME;
			else if (new_volume < 0) 
				new_volume = 0;

			/* For the sfx that will be added later */
			Mix_Volume(-1, new_volume);

			Mix_VolumeMusic(new_volume);
			Resources::GetInstance()->SetMixChunkVolume(new_volume);

			if (handle_rect.y + handle_rect.h > slider_rect.y + slider_rect.h - DIFF)
				handle_rect.y = slider_rect.y + slider_rect.h - handle_rect.h - DIFF;

			else if (handle_rect.y < slider_rect.y + DIFF)
				handle_rect.y = slider_rect.y + DIFF;
		}
	}

	else if (is_clicked && !is_pressed)
		Enable(false);
}