#include "Resources.h"

PZS::Resources* PZS::Resources::GetInstance(void) noexcept
{
	static Resources instance;
	return &instance;
}

PZS::Resources::Resources(void) noexcept
{
}

void PZS::Resources::Destroy(void) noexcept
{
	gTextures.Destroy();
	gMusic.Destroy();
	gSFX.Destroy();
	gFonts.Destroy();
}
