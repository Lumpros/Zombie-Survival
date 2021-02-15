#pragma once

#include "Entity.h"
#include "Resources.h"
#include "Animation.h"

namespace PZS
{
    struct Vector2D { int x, y; };

    class Bullet : public Entity
    {
    protected:
        double horizontal_velocity,
            vertical_velocity;

        double pos_x, pos_y;

        Texture* bullet_explosion = nullptr;
        Animation explosion_animation;

    public:
        /* bool destroyed is used to indicate that the bullet has collided, meaning the bullet */
        /* explosion animation is playing. bool active is used to indicate that the animation */
        /* has finished and the bullet ought to be removed from the player_bullets vector */
        bool active = true;
        bool destroyed = false;

        int damage;

        void Update(void) noexcept override;
        void Render(void) noexcept override;

        Bullet(SDL_Point player_position, double vel_x, double vel_y, int dmg) noexcept
            : horizontal_velocity{ vel_x }, vertical_velocity{ vel_y } 
        {
            /* Get textures */
            bullet_explosion = Resources::GetInstance()->gTextures.Get("bullet_explosion_sheet");
            mTexture = Resources::GetInstance()->gTextures.Get("bullet");

            /* Set in world coordinates */
            pos_x = player_position.x;
            pos_y = player_position.y;

            /* Set hitbox */
            hitbox = { player_position.x, player_position.y, BULLET_SIZE, BULLET_SIZE };

            damage = dmg;

            /* Initialize animation */
            for (int i = 0; i < 13; ++i)
                explosion_animation.AddFrame({ { i * 39, 0, 40, 42 }, 20 });
        }
    };
}

