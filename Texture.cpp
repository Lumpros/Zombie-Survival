#include "Texture.h"

PZS::Renderer::~Renderer(void) noexcept
{
	SDL_DestroyRenderer(mRenderer);
}

PZS::Renderer* PZS::Renderer::GetInstance(void) noexcept
{
	static Renderer instance;
	return &instance;
}

PZS::Texture::Texture(void) noexcept
{
	mWidth = mHeight = 0;
}

PZS::Texture::Texture(const char* path) noexcept
{
	if (!LoadFromFile(path))
		fprintf(stderr, "Failed to load %s\n", path);
	else 
		SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_BLEND);
}

PZS::Texture::~Texture(void)
{
	Free();
}

bool NODISCARD PZS::Texture::LoadFromFile(const char* path) noexcept
{
	Free();

	SDL_Texture* new_texture = nullptr;
	SDL_Surface* loaded_surface = IMG_Load(path);

	if (!loaded_surface)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError());
		exit(-1);
	}

	else
	{
		new_texture = SDL_CreateTextureFromSurface(Renderer::GetInstance()->mRenderer, loaded_surface);
		if (!new_texture)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
			exit(-1);
		}

		else
		{
			mWidth = loaded_surface->w;
			mHeight = loaded_surface->h;
		}

		SDL_FreeSurface(loaded_surface);
	}

	mTexture = new_texture;
	return mTexture != NULL;
}

void PZS::Texture::Free(void) noexcept
{
	if (mTexture) {
		SDL_DestroyTexture(mTexture);
		mTexture = nullptr;
		mWidth = mHeight = 0;
	}
}

void PZS::Texture::Render(SDL_Rect rect, SDL_Rect* clip, double angle, SDL_Point* centre) noexcept
{
	SDL_Renderer* gRenderer = Renderer::GetInstance()->mRenderer;
	SDL_Rect renderQuad = { rect.x, rect.y, rect.w, rect.h };

	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, centre, SDL_FLIP_NONE);
}