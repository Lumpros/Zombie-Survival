#pragma once

#include <utility>
#include <ctime>

#include "Resources.h"
#include "Globals.h"

#define AMMO_LIM 990

namespace PZS
{
	class Gun
	{
	private:
		bool mouse_is_clicked : 1;

	protected:
		SFX* shoot_sfx;
		SFX* reload_sfx;

	protected:
		/* entity_pos is the center of the entity */
		void Shoot(Vector2D entity_pos, Vector2D mouse_pos) noexcept;

		/* Calls bullet.update() and deletes destroyed bullets */
		void UpdateIndividualBullet(void) noexcept;

		clock_t fire_start  = 0;

		int damage = 0;
		int fire_rate_milliseconds = 0;
		int bullets_shot = 1;

		int ammo_lim = 0;
		int ammo = 0;
		int total_ammo = 0;

		bool hold_to_shoot = false;

		clock_t time_passed_milliseconds = 0;

	public:
		Gun(void) { mouse_is_clicked = false; }

		virtual void Reset(void) {};

		void Update(int x, int y) noexcept;
		void Render(void) noexcept;

		void Reload(void) noexcept;
		void AddAmmoFromBox(int times) noexcept;

		bool MagIsFull(void) const noexcept { return ammo == ammo_lim; }

		int GetShotRate(void) const noexcept { return fire_rate_milliseconds; }
		int GetTotalAmmo(void) const noexcept { return total_ammo; }
		int GetLoadedAmmo(void) const noexcept { return ammo; }
		int GetAmmoCap(void) const noexcept { return ammo_lim; }

		static const std::pair<double, double> CalculateObjectVelocity(Vector2D entity, Vector2D mouse, const double speed);
	};

	class Pistol : public Gun
	{
	public:
		Pistol(void) : Gun() 
		{	
			Reset();

			shoot_sfx = Resources::GetInstance()->gSFX.Get("pistol_shot");
			reload_sfx = Resources::GetInstance()->gSFX.Get("rifle_reload");
		}

		void Reset(void) override 
		{
			ammo_lim = 6;
			ammo = 6;
			total_ammo = 240;

			bullets_shot = 1;
			fire_rate_milliseconds = 150;

			hold_to_shoot = false;

			damage = 25;
		}
	};

	class Shotgun : public Gun
	{
	public:
		Shotgun(void) : Gun() 
		{
			Reset();

			shoot_sfx = Resources::GetInstance()->gSFX.Get("shotgun_shot");
			reload_sfx = Resources::GetInstance()->gSFX.Get("shotgun_reload");
		}

		void Reset(void) override
		{
			ammo_lim = 2;
			ammo = 2;
			total_ammo = 0;

			fire_rate_milliseconds = 400;
			damage = 100;

			hold_to_shoot = false;

			bullets_shot = 2;
		}
	};

#ifdef DEBUG
#define DAMAGE 200
#define AMMO 99999
#else
#define DAMAGE 34
#define AMMO 30
#endif

	class Rifle : public Gun
	{
	public:
		Rifle(void) : Gun() 
		{
			Reset();

			shoot_sfx = Resources::GetInstance()->gSFX.Get("pistol_shot");
			reload_sfx = Resources::GetInstance()->gSFX.Get("rifle_reload");
		}

		void Reset(void) override
		{
			ammo_lim = 30;
			ammo = AMMO;
			total_ammo = 0;

			fire_rate_milliseconds = 100;
			damage = DAMAGE;

			hold_to_shoot = true;

			bullets_shot = 1;

		}
	};
}