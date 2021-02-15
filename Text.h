#pragma once

#include <SDL_ttf.h>

#include <string>

namespace PZS
{
	/* Used to make allocation easier */
	class Font
	{
	private:
		TTF_Font* mFont = nullptr;

	public:
		Font(const Font&) = delete;

		Font(const char* path, int font_size) noexcept;
		~Font(void) noexcept;

		TTF_Font* GetFont(void) const noexcept;
	};


	class Text
	{
	private:
		Font* mFont;
		SDL_Texture* mTexture = nullptr;

		std::string mText;
		SDL_Color text_color;

		int height, width;

	public:
		Text(Font* font, const char*, SDL_Color text_color) noexcept;
		~Text(void) noexcept;

		void Free(void) noexcept;

		void SetText(const char* text) noexcept;
		void SetColor(SDL_Color color) noexcept;

		int GetHeight(void) const noexcept { return height; }
		int GetWidth(void) const noexcept { return width; }

		SDL_Texture* GetGeneratedTexture(void) const noexcept { return mTexture; }

		/* Creates a texture using a font, text, and color 
		   texture is used in Render() */
		bool LoadFromRenderedText(const char* path, SDL_Color text_color) noexcept;

		void Render(int x, int y, SDL_Rect* clip = nullptr) noexcept;
	};
}
