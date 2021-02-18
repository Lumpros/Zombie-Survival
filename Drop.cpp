#include "Drop.h"
#include "Player.h"

PZS::Drop::Drop(SDL_Rect rect) noexcept
{
	hitbox = rect;

	end = clock();
}

void PZS::Drop::set_timer_for_pause(void)
{
	end = clock();
}

void PZS::Drop::Render(void) noexcept
{
	if (isVisible)
		mTexture->Render(hitbox);
}

void PZS::Drop::HandleBlink(void) noexcept 
{
	timePassedMilliseconds += clock() - end;

	if (timePassedMilliseconds >= 7000)
		should_blink = true;

	if (timePassedMilliseconds >= 10000)
		RemoveDrop();

	end = clock();
}

void PZS::Drop::RemoveDrop(void) noexcept
{
	if (isRemoved) 
		return;

	isRemoved = true;

	for (size_t i = 0; i < g_drops.size(); ++i) 
	{
		if (g_drops[i].get() == this) {
			g_drops.erase(g_drops.begin() + i);
			return;
		}
	}
}

void PZS::Drop::CalculateBlink(void) noexcept
{
	/* Dont want to initialize during construction */
	if (blinkEnd == 0)
		blinkEnd = clock();

	timePassedCalc += clock() - blinkEnd;

	if (timePassedCalc >= 300) {
		timePassedCalc = 0;
		isVisible = !isVisible;
	}

	blinkEnd = clock();
}

void PZS::Drop::Update(void) noexcept
{
	HandleBlink();

	if (!isRemoved)
	{
		if (should_blink)
			CalculateBlink();

		if (SDL_HasIntersection(&Player::Get()->hitbox, &hitbox)) {
			ExecuteDropEffect();
			RemoveDrop();
		}
	}
}