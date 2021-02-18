#include "Resources.h"

PZS::Resources* PZS::Resources::GetInstance(void) noexcept
{
	static Resources instance;
	return &instance;
}

PZS::Resources::Resources(void) noexcept
{
}

void PZS::Resources::SetMixChunkVolume(int volume) noexcept
{
	auto container = gSFX.GetContainerCopy();

	for (auto it = container->cbegin(); it != container->cend(); ++it)
		Mix_VolumeChunk(it->second->GetChunk(), volume);
}

void PZS::Resources::Destroy(void) noexcept
{
	gTextures.Destroy();
	gMusic.Destroy();
	gSFX.Destroy();
	gFonts.Destroy();
}
