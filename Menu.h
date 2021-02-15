#pragma once

#include "Texture.h"
#include "Sound.h"

#include <ctime>

namespace PZS
{
	/* Use button sheet called "button_sheet" */
	class Button
	{
	private:
		Texture* mTexture;

		SDL_Rect button_rectangle, 
				 original_position,
				 clip;

		float scale = 1.0f;

		bool has_been_clicked = false;
		bool is_clicked = false;
		
		bool should_shake = false;

		float angle = 0.0f;
		float angle_dir = 0.25f;

		/* I should prob implement a class for this or something */
		clock_t begin = 0;
		clock_t timePassedMilliseconds = 0;

		void DoShake(void);
		void AdjustButtonSize(bool mouse_on_button) noexcept;

	public:
		void Update(void) noexcept;
		void Render(void) noexcept;

		bool IsClicked(void) noexcept;

		void Init(void) noexcept;

		Button(SDL_Rect clip, SDL_Rect button_rect, float scale, bool shake) noexcept;
	};

	class Menu
	{
	private:
		Texture* menu_background = nullptr;
		Texture* controls_tab = nullptr;

		SFX* button_click_sfx;

		SDL_Rect controls_rect = { WINDOW_WIDTH / 2 - 485 / 2, -740, 485, 739 };

		bool show_controls = false;
		bool going_up = false;

		Button play_button;
		Button controls_button;

	public:
		Music* menu_music;

		void Init(void) noexcept;

		Menu(void) noexcept;

		void Update(void) noexcept;
		void Render(void) noexcept;
	};
}