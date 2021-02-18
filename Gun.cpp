#include "Gun.h"
#include "Player.h"

#include <cmath>

std::vector<std::unique_ptr<PZS::Bullet>> PZS::g_player_bullets;

const std::pair<double, double> PZS::Gun::CalculateObjectVelocity(Vector2D entity_pos, Vector2D mouse_pos, const double bullet_velocity)
{
	if (entity_pos.x == mouse_pos.x)
		return std::make_pair(0.0, mouse_pos.y > entity_pos.y ? bullet_velocity : (-bullet_velocity));

	double dir_factor = (static_cast<double>(entity_pos.y) - mouse_pos.y) / (static_cast<double>(entity_pos.x) - mouse_pos.x);
	double mult = sqrt(
		(bullet_velocity * bullet_velocity) / (dir_factor * dir_factor + 1)
	);

	if (mouse_pos.x > entity_pos.x)
		mult = -mult;

	return std::make_pair(-mult, -mult * dir_factor);
}

void PZS::Gun::Shoot(Vector2D player_pos, Vector2D mouse_pos) noexcept
{
	/* If an animation is playing (meelee, shooting, reloading...), don't shoot */
	if (Player::Get()->IsPlayOnce())
		return;

	if (total_ammo == 0 && ammo == 0) return;
	
	std::pair<double, double> velocities = CalculateObjectVelocity(player_pos, mouse_pos, 10);

	/* Guns like the shotgun shoot more than one bullet */
	for (int i = 0; i < bullets_shot; ++i)
	{
		g_player_bullets.emplace_back(
			std::make_unique<Bullet>(
				SDL_Point({ player_pos.x, player_pos.y}),
				velocities.first, velocities.second,
				damage
				)
		);
	}

	shoot_sfx->Play();

	ammo -= bullets_shot;

	Player::Get()->DoShootingAnimation();
}

void PZS::Gun::UpdateIndividualBullet(void) noexcept
{
	for (size_t i = 0; i < g_player_bullets.size(); ++i)
	{
		/* Decrement i after erasing because the vector size is decreased */
		if (!g_player_bullets[i]->active)
			g_player_bullets.erase(g_player_bullets.begin() + (i--));
		else
			g_player_bullets[i]->Update();
	}
}

void PZS::Gun::Update(int x, int y) noexcept
{
	Player* player = Player::Get();
	Vector2D mouse_position;
	Uint32 mouse_state = SDL_GetMouseState(&mouse_position.x, &mouse_position.y);

	time_passed_milliseconds += clock() - fire_start;

	/* If left mouse button is clicked and etc */
	if ((mouse_state & SDL_BUTTON_LMASK) && time_passed_milliseconds > fire_rate_milliseconds && ammo > 0)
	{
		if ((!mouse_is_clicked && !hold_to_shoot) || hold_to_shoot)
		{
			mouse_is_clicked = true;

			/* Shoot bullet starting from the players center */
			/* Using GetWidth() and GetHeight() because the hitbox changes when changing weapons */
			Vector2D player_point = { x + player->GetWidth() / 2 - BULLET_SIZE / 2, y + player->GetHeight() / 2 - BULLET_SIZE / 2 };
			Vector2D mouse_point = { mouse_position.x - BULLET_SIZE / 2, mouse_position.y - BULLET_SIZE / 2 };

			Shoot(player_point, mouse_point);

			/* Reset timer */
			time_passed_milliseconds = 0;
		}
	}

	else if (!(mouse_state & SDL_BUTTON_LMASK))
		mouse_is_clicked = false;

	UpdateIndividualBullet();

	/* Checks if it should reload and the shooting animation is done */
	if (ammo <= 0 && !player->IsPlayOnce())
		Player::Get()->DoReloadAnimation();

	fire_start = clock();
}

void PZS::Gun::AddAmmoFromBox(int times) noexcept 
{
	total_ammo += ammo_lim * times;
	
	if (total_ammo > AMMO_LIM)
		total_ammo = AMMO_LIM;
}

void PZS::Gun::Render(void) noexcept
{
	for (size_t i = 0; i < g_player_bullets.size(); ++i)
		g_player_bullets[i]->Render();
}

void PZS::Gun::Reload(void) noexcept
{
	total_ammo -= ammo_lim - ammo;
	ammo += ammo_lim - ammo;

	if (total_ammo < 0) {
		ammo += total_ammo;
		total_ammo = 0;
	}
}