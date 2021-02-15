#include "Bullet.h"
#include "Globals.h"

void PZS::Bullet::Render(void) noexcept
{
	/* Just a random value to render the bullet explosion */
	static const int size = 32;

	/* If it has been destroyed stop rendering the bullet */
	if (active && !destroyed)
		mTexture->Render(hitbox);

	else if (active && destroyed)
		bullet_explosion->Render({ hitbox.x - size / 2, hitbox.y - size / 2, hitbox.w + size, hitbox.h + size},
			explosion_animation.GetCurrentFrame());
}

void PZS::Bullet::Update(void) noexcept
{
	if (!destroyed) {
		/* Calculate coordinates with game coordinates */
		pos_x += horizontal_velocity;
		pos_y += vertical_velocity;

		/* Cast it to ints later. This is done to keep the accuracy */
		hitbox.x = static_cast<int>(pos_x);
		hitbox.y = static_cast<int>(pos_y);

		/* Check if the bullet has hit any barriers. If it has, destroy it (animation) */
		for (size_t i = 0; i < g_barriers.size() && active; ++i)
			if (SDL_HasIntersection(&hitbox, &g_barriers[i]->hitbox))
				destroyed = true;

		/* Check if it has hit any zombie. If it has, destroy the bullet and damage the zombie */
		for (size_t i = 0; i < g_zombies.size() && active; ++i)
		{
			/* Blood animation may be playing, so even with no hp the hitbox may be there*/
			if (g_zombies[i]->GetHP() <= 0)
				continue;

			/* Check collision, and stop checking if it has hit a zombie. Impossible to have hit any others*/
			if (SDL_HasIntersection(&hitbox, &g_zombies[i]->hitbox)) {
				g_zombies[i]->Damage(damage);
				destroyed = true;
				break;
			}
		}
	}

	/* Constrict bullets in the bounds of the screen*/
	/* If it gets out, deactivate it (not destroy, we don't want the animation) */
	if (hitbox.x + hitbox.w < 0 || hitbox.x > WINDOW_WIDTH || hitbox.y + hitbox.h < 0 || hitbox.y > WINDOW_HEIGHT)
		active = false;

	if (active && destroyed)
		explosion_animation.Update(true);

	/* If the explosion animation has finished, deactivate the bullet */
	if (!explosion_animation.IsRunning())
		active = false;
}