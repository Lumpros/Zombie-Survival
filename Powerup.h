#pragma once

#include "Drop.h"
#include "Player.h"

namespace PZS
{
	class Medkit : public Drop
	{
	protected:
		void ExecuteDropEffect(void) noexcept {
			if (Player::Get()->GetHP() < 100)
				pickup_sfx->Play();

			Player::Get()->Damage(-50);
		}

	public:
		Medkit(SDL_Rect rect) : Drop({ rect.x, rect.y, 32, 32 }) {
			mTexture = Resources::GetInstance()->gTextures.Get("medkit");
			pickup_sfx = Resources::GetInstance()->gSFX.Get("heal_pickup");
		}
	};

	class PistolAmmo : public Drop
	{
	protected:
		void ExecuteDropEffect(void) noexcept {
			Player::Get()->GetGun(PISTOL)->AddAmmoFromBox(5);
			pickup_sfx->Play();
		}

	public:
		PistolAmmo(SDL_Rect rect) : Drop({ rect.x, rect.y, 32, 24 }) {
			mTexture = Resources::GetInstance()->gTextures.Get("pistol_ammo");
			pickup_sfx = Resources::GetInstance()->gSFX.Get("ammo_pickup");
		}
	};

	class ShotgunAmmo : public Drop
	{
	protected:
		void ExecuteDropEffect(void) noexcept {
			Player::Get()->GetGun(SHOTGUN)->AddAmmoFromBox(10);
			pickup_sfx->Play();
		}

	public:
		ShotgunAmmo(SDL_Rect rect) : Drop({ rect.x, rect.y, 32, 24 }) {
			mTexture = Resources::GetInstance()->gTextures.Get("shotgun_ammo");
			pickup_sfx = Resources::GetInstance()->gSFX.Get("ammo_pickup");
		}
	};


	class RifleAmmo : public Drop
	{
	protected:
		void ExecuteDropEffect(void) noexcept {
			Player::Get()->GetGun(RIFLE)->AddAmmoFromBox(2);
			pickup_sfx->Play();
		}

	public: 
		RifleAmmo(SDL_Rect rect) : Drop({ rect.x, rect.y, 32, 24 }) {
			mTexture = Resources::GetInstance()->gTextures.Get("rifle_ammo");
			pickup_sfx = Resources::GetInstance()->gSFX.Get("ammo_pickup");
		}
	};

	class Nuke : public Drop
	{
	protected:
		void ExecuteDropEffect(void) noexcept {
			for (size_t i = 0; i < g_zombies.size(); ++i)
				g_zombies[i]->Damage(200);
			pickup_sfx->Play();
		}

	public:
		Nuke(SDL_Rect rect) : Drop({ rect.x, rect.y, 64, 64 }) {
			mTexture = Resources::GetInstance()->gTextures.Get("nuke");
			pickup_sfx = Resources::GetInstance()->gSFX.Get("explosion");
		}
	};

	class Instakill : public Drop
	{
	protected:
		void ExecuteDropEffect(void) noexcept {
			for (int i = 0; i < (int)GunIndex::GUN_COUNT; ++i)
				Player::Get()->GetGun((GunIndex)i)->SetInstaKill();
			Player::Get()->DoInstaKill();
		}

	public:
		Instakill(SDL_Rect rect) : Drop({ rect.x, rect.y, 64, 64 }) {
			mTexture = Resources::GetInstance()->gTextures.Get("instakill");
		}
	};

	class RapidFire : public Drop
	{
	protected:
		void ExecuteDropEffect(void) noexcept {
			for (int i = 0; i < (int)GunIndex::GUN_COUNT; ++i)
				Player::Get()->GetGun((GunIndex)i)->SetRapidFire();
		}

	public:
		RapidFire(SDL_Rect rect) : Drop({ rect.x, rect.y, 32, 32 }) {
			mTexture = Resources::GetInstance()->gTextures.Get("rapid_fire");
			pickup_sfx = Resources::GetInstance()->gSFX.Get("ammo_pickup");
		}
	};

	class InfiniteAmmo : public Drop
	{
	protected:
		void ExecuteDropEffect(void) noexcept {
			for (int i = 0; i < (int)GunIndex::GUN_COUNT; ++i)
				Player::Get()->GetGun((GunIndex)i)->DisableAmmoReduction();
		}

	public:
		InfiniteAmmo(SDL_Rect rect) : Drop({ rect.x, rect.y, 64, 32 }) {
			mTexture = Resources::GetInstance()->gTextures.Get("infinite");
		}
	};
}

