#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <string>

#include "Preprocessors.h"

namespace PZS
{
	/*
	*  Singleton Renderer class to avoid using a global variable
	*  Doesn't really do anything else
	*  
	*  mRenderer is initialized in Application::Initialize
	*/
	class Renderer
	{
	private:
		Renderer(void) noexcept { mRenderer = nullptr; }

	public:
		Renderer(const Renderer&) = delete;
		void operator=(const Renderer&) = delete;

		~Renderer(void) noexcept;

		static Renderer* GetInstance(void) noexcept;

		SDL_Renderer* mRenderer;
	};

	/* Basic wrapper class for SDL_Texture to handle Rendering and memory allocation */
	class Texture
	{
	private:
		SDL_Texture* mTexture = nullptr;
		int mWidth, mHeight;

		int _opacity = 0xFF;

	public:
		Texture(void) noexcept;
		explicit Texture(const char* path) noexcept;
		~Texture(void) noexcept;

		bool NODISCARD LoadFromFile(const char* path) noexcept;
		
		void SetAlpha(Uint8 alpha) { SDL_SetTextureAlphaMod(mTexture, alpha); }
		void Free(void) noexcept;
		
		void Render(SDL_Rect rect, SDL_Rect* clip = nullptr, double angle = 0.0, SDL_Point* center = nullptr) noexcept;
	};
}
