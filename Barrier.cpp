#include "Barrier.h"
#include "Resources.h"

PZS::Barrier::Barrier(SDL_Rect rect) noexcept
{
	hitbox = rect;
	mTexture = Resources::GetInstance()->gTextures.Get("barrier");
}

void PZS::Barrier::Render(void) noexcept
{
	mTexture->Render(hitbox);
}

void PZS::Barrier::Update(void) noexcept
{

}