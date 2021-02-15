#pragma once

#include "Drop.h"
#include "Player.h"
#include "Resources.h"

namespace PZS
{
	class ShotgunDrop : public Drop
	{
	protected:
		void ExecuteDropEffect(void) noexcept override {
			Player::Get()->EnableWeapon(SHOTGUN);
			Player::Get()->GetGun(SHOTGUN)->AddAmmoFromBox(10);

			pickup_sfx->Play();
		}

	public:
		ShotgunDrop(SDL_Rect rect) : Drop({ rect.x, rect.y, 64, 64 }) {
			mTexture = Resources::GetInstance()->gTextures.Get("shotgun");
			pickup_sfx = Resources::GetInstance()->gSFX.Get("gun_pickup");
		}
	};

	class RifleDrop : public Drop
	{
	protected:
		void ExecuteDropEffect(void) noexcept override {
			Player::Get()->EnableWeapon(RIFLE);
			Player::Get()->GetGun(RIFLE)->AddAmmoFromBox(2);

			pickup_sfx->Play();
		}

	public:
		RifleDrop(SDL_Rect rect) : Drop({ rect.x, rect.y, 64, 64 }) {
			mTexture = Resources::GetInstance()->gTextures.Get("rifle");
			pickup_sfx = Resources::GetInstance()->gSFX.Get("gun_pickup");
		}
	};
}