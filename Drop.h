#pragma once

#include "Entity.h"
#include "Sound.h"

#include <time.h>

namespace PZS
{
    /* Render after zombies and player */
    class Drop : public Entity
    {
    private:
        void HandleBlink(void) noexcept;
        void RemoveDrop(void) noexcept;
        void CalculateBlink(void) noexcept;

        clock_t end = 0, blinkEnd = 0;
        clock_t timePassedMilliseconds = 0,
            timePassedCalc = 0;

        bool should_blink = false;
        bool isVisible = true;
        bool isRemoved = false;

    protected:
        virtual void ExecuteDropEffect(void) = 0;

        SFX* pickup_sfx = nullptr;

    public:
        explicit Drop(SDL_Rect rect) noexcept;

        void set_timer_for_pause(void);

        void Update(void) noexcept override;
        void Render(void) noexcept override;
    };
}

