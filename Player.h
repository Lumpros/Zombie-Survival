#pragma once

#include "GameActor.h"
#include "Animation.h"
#include "Gun.h"

#define STAB_FRAME 9
#define RELOAD_FRAME 14

namespace PZS
{
    enum GunIndex {
        PISTOL, RIFLE, SHOTGUN, GUN_COUNT
    };

    class Player : public GameActor
    {
    private:
        Texture* still_sheet[GUN_COUNT],
            * walk_sheet[GUN_COUNT],
            * shoot_sheet[GUN_COUNT],
            * reload_sheet[GUN_COUNT],
            * meelee_sheet[GUN_COUNT],
            * blood_sheet, *heal_sheet;

        SFX* reload_sfx[GUN_COUNT];
        SFX* damage_sfx = nullptr;

        SDL_Rect hitboxes[GUN_COUNT];

        Player(void) noexcept;

        void InitializePistolGraphics(Resources* resources) noexcept;
        void InitializeShotgunGraphics(Resources* resources) noexcept;
        void InitializeRifleGraphics(Resources* resources) noexcept;
        void InitializeBloodGraphics(Resources* resources) noexcept;
        void InitializeHealGraphics(Resources* resources) noexcept;

        void HandleGunChange(const Uint8*) noexcept;
        void HandleMovement(void) noexcept override;
        void HandleAnimation(SDL_Rect) noexcept;
        bool HandleCollisionWithZombie(SDL_Rect rect) noexcept override;
        void HandleMeelee(void) noexcept;

        void DoMeeleeAnimation(void) noexcept;
        void DoBloodAnimation(void) noexcept;
        void DoHealAnimation(void) noexcept;

        // switch weapons with q and e
        std::unique_ptr<Gun> guns[GUN_COUNT];
        bool available_guns[GUN_COUNT];

        int health = 100;

        clock_t meelee_begin = 0;
        int meelee_damage = 50; 
        const int meelee_const = 50;

        int movement_speed = 3;

        double rotation_angle = 0.0f;

        GunIndex gun_index = PISTOL;

        bool has_reloaded = false;
        bool alive = true;
        bool has_hit_zombie = false;
        bool play_once = false; /* used when shooting etc */
        bool play_blood = false;
        bool play_heal = false;

        Animation* current_animation;
        Animation walk_animation[GUN_COUNT],
            still_animation[GUN_COUNT],
            shooting_animation[GUN_COUNT],
            reload_animation[GUN_COUNT],
            meelee_animation[GUN_COUNT],
            blood_animation, heal_animation;

    public:
        Player(const Player&) = delete;
        Player(Player&&) = delete;

        Player& operator=(const Player&) = delete;
        Player& operator=(Player&&) = delete;

        static Player* Get(void) noexcept;

        void Update(void) noexcept override;
        void Render(void) noexcept override;

        /* May be negative in order to heal */
        void Damage(int dmg) noexcept;

        int GetHeight(void) const noexcept { return hitboxes[gun_index].h; }
        int GetWidth(void) const noexcept { return hitboxes[gun_index].w; }

        void DoShootingAnimation(void) noexcept;
        void DoReloadAnimation(void) noexcept;

        void DoInstaKill(void) { 
            meelee_damage = 200; 
            meelee_begin = clock();
        }

        void SetProperPauseClock(void) {
            meelee_begin = clock();
        }

        int NODISCARD GetHP(void) const noexcept { return health; }

        void Reset(void) noexcept;

        /* Weapon related functions */
        Gun* GetGun(GunIndex index) const noexcept { return guns[index].get(); }

        bool WeaponIsActive(GunIndex index) const noexcept { return available_guns[index]; }
        void EnableWeapon(GunIndex index) noexcept { available_guns[index] = true; }
        
        GunIndex GetGunIndex(void) const noexcept { return gun_index; }

        bool IsPlayOnce(void) noexcept { return play_once; }
    };
}

