#pragma once

#include "Entity.h"

namespace PZS
{
    class GameActor : public Entity
    {
    protected:
        bool HandleCollisionWithBarrier(SDL_Rect rect) noexcept;

        virtual bool HandleCollisionWithZombie(SDL_Rect rect) noexcept = 0;
        virtual void HandleMovement(void) noexcept = 0;

    public:
        static double CalculateRotationAngle(SDL_Point point1, SDL_Point point2) noexcept;
    };
}

