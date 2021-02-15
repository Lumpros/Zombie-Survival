#pragma once

#include "GameActor.h"
#include "Animation.h"
#include "Resources.h"

namespace PZS
{
    /* Can zombies even be considered alive? */
    int GetAliveZombieCount(void) noexcept;

    class Zombie : public GameActor
    {
    private:
        Animation walking_animation, attack_animation, blood_animation;
        Animation* current_animation = nullptr;

        Texture *attack_texture = nullptr, 
                *walk_texture = nullptr, 
                *blood_sheet = nullptr;

        SFX* damage_sfx = nullptr;

        bool play_once = false;
        bool can_move = true;
        bool hasnt_hit_player = true;
        bool play_blood = false;
        bool is_dead = false;

        int hp = 100;
        int damage = 10;

        void HandleAnimation(void) noexcept;
        void DoBloodAnimation(void) noexcept;

        void ChooseGunDrop(void) noexcept;
        void ChoosePowerup(void) noexcept;

        void InitializeWalkGraphics(Resources* resources) noexcept;
        void InitializeAttackGraphics(Resources* resources) noexcept;
        void InitializeBloodGraphics(Resources* resources) noexcept;

    public:
        explicit Zombie(SDL_Rect rect);

        void Update(void) noexcept override;
        void Render(void) noexcept override;

        bool IsDead(void) const noexcept { return is_dead; }
        void Damage(int dmg) noexcept;
        int GetHP(void) const { return hp; }

        void HandleMovement(void) noexcept override;
        bool HandleCollisionWithZombie(SDL_Rect rect) noexcept override;
        void DoAttackAnimation(void) noexcept;
    };
}

