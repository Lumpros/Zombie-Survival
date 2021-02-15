#pragma once

#include <vector>
#include <SDL.h>

#include "Texture.h"
#include "Resources.h"
#include "Entity.h"
#include "Player.h"

namespace PZS
{
	class GunBox : public Entity
	{
	private:
		GunIndex index;
		Text bullet_count;

		int old_bullet_count = 0;

		SDL_Color DecideBoxColor(void);
		SDL_Color DecideNumColor(void);

		void ChooseGunTexture(GunIndex gunIndex);

	public:
		GunBox(GunIndex index, SDL_Rect rectangle) noexcept;

		void Update(void) noexcept override;
		void Render(void) noexcept override;
	};

	class BoxArrow : public Entity
	{
	private:
		int direction = 1; // Down
		
		const int arrow_sz = 32;

		clock_t timeElapsed = 0;
		clock_t end;

	public:
		BoxArrow(void);

		void Update(void) noexcept override;
		void Render(void) noexcept override;
	};

	class Stage
	{
	private:
		Texture* background_texture = nullptr;
		Texture* target_texture = nullptr;

		std::unique_ptr<Text> wave_text;

		// ui related stuff
		GunBox shotgun_box;
		GunBox rifle_box;
		GunBox pistol_box;

		BoxArrow arrow;

		Text health_text, ammo_text;
		Text zombies_left_text;

		/* some optimizations */
		int old_hp = 100;
		int old_cap = 6;
		int old_ammo = 6;
		int old_zombie_count = 0;

		volatile bool is_paused = false;

		/* Gameplay related*/
		int wave = 0;
		int zombies_left = 0;

		bool wave_is_visible = false;

	private:
		void ShowMouseTarget(void) noexcept;

		void DisplayGameplayRelated(void) noexcept;
		void UpdateGameplayRelated(void) noexcept;

		void HandleZombieCount(void) noexcept;
		void HandleAmmoCount(Player*) noexcept;
		void HandleHealthCount(Player*) noexcept;
		void DoNextStage(void) noexcept;

		void DisplayUI(void) noexcept;
		void UpdateUI(void) noexcept;

		int CalculateZombieSpawnNumber(int wave) const noexcept;

	public:
		Stage(const Stage&) = delete;
		Stage(Stage&&) = delete;

		Stage& operator=(const Stage&) = delete;
		Stage& operator=(Stage&&) = delete;

		void Update(void) noexcept;
		void Render(void) noexcept;

		void SpawnZombie(void) noexcept;
		int GetZombieLimit(void) const noexcept;

		Stage(void) noexcept;

		static Stage* Get(void) noexcept {
			static Stage instance;
			return &instance;
		}
	};
}

