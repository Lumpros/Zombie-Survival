#include "GameActor.h"

bool PZS::GameActor::HandleCollisionWithBarrier(SDL_Rect rect) noexcept
{
	if (abs(hitbox.x - rect.x) > 200) return false;

	SDL_Rect intersection;

	if (SDL_IntersectRect(&hitbox, &rect, &intersection))
	{
		if (intersection.w <= intersection.h)
		{
			if (hitbox.x < rect.x)
				hitbox.x -= intersection.w;
			else
				hitbox.x += intersection.w;
		}

		else
		{
			if (hitbox.y < rect.y)
				hitbox.y -= intersection.h;
			else
				hitbox.y += intersection.h;
		}

		return true;
	}

	return false;
}

double PZS::GameActor::CalculateRotationAngle(SDL_Point point1, SDL_Point point2) noexcept
{
	double angle = 0.0;

	if (point1.x == point2.x)
	{
		if (point1.y > point2.y)
			angle = 90.0;
		else
			angle = 270.0;
	}

	else
	{
		double dir_factor = (static_cast<double>(point2.y) - point1.y) / (static_cast<double>(point2.x) - point1.x);
		angle = atan(dir_factor) * 180 / M_PI;

		if (point2.x > point1.x)
			angle += 180;
	}

	return angle;
}