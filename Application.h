#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include <cstdio>
#include <ctime>

#include "Preprocessors.h"
#include "Stage.h"
#include "Menu.h"

namespace PZS
{
	class Application
	{
	private:
		SDL_Window* mWindow;
		Menu game_menu;

		bool has_loaded_game_resources = false;
		bool show_black_screen = false;
		Uint8 bs_opacity = 0x00;

	private:
		void DoBlackScreen(PZS::GameState new_state) noexcept;

		void Update(void) noexcept;
		void Render(void) noexcept;

		void LoadResources(void) const noexcept;

	public:
		bool NODISCARD Initialize(void) noexcept;

		void Loop(void) noexcept;

		~Application(void) noexcept;
	};
}

