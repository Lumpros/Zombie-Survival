#include "Text.h"
#include "Texture.h"

PZS::Font::Font(const char* path, int font_size) noexcept
{
	mFont = TTF_OpenFont(path, font_size);
}

PZS::Font::~Font(void) noexcept
{
    /* This call is cursed.
    *  Destructor is only called once,
    *  mFont is a valid pointer
    *  Yet, I get a segmentation fault EVERY time*/

	/* TTF_CloseFont(mFont); */
}

TTF_Font* PZS::Font::GetFont(void) const noexcept
{
	return mFont;
}

PZS::Text::Text(Font* font, const char* text, SDL_Color text_color) noexcept
	: mFont(font)
{
	this->text_color = text_color;

    SetText(text);
	LoadFromRenderedText(text, text_color);
}

PZS::Text::~Text(void) noexcept
{
    Free();
}

void PZS::Text::Free(void) noexcept
{
    if (mTexture)
    {
        SDL_DestroyTexture(mTexture);
        mTexture = nullptr;
        mText = "";
    }
}

void PZS::Text::SetText(const char* text) noexcept
{
    mText = text;
    LoadFromRenderedText(text, text_color);

    SDL_QueryTexture(mTexture, nullptr, nullptr, &width, &height);
}

void PZS::Text::SetColor(SDL_Color color) noexcept
{
    text_color = color;
    LoadFromRenderedText(mText.c_str(), color);
}

bool PZS::Text::LoadFromRenderedText(const char* text, SDL_Color text_color) noexcept
{
    Free();

    //Render text surface
    SDL_Surface* textSurface = TTF_RenderText_Solid(mFont->GetFont(), text, text_color);
    if (textSurface == NULL)
    {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
    }
    else
    {
        //Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface(Renderer::GetInstance()->mRenderer, textSurface);
        if (mTexture == NULL)
        {
            printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        }

        //Get rid of old surface
        SDL_FreeSurface(textSurface);
    }

    //Return success
    return mTexture != nullptr;
}

void PZS::Text::Render(int x, int y, SDL_Rect* clip) noexcept
{
    SDL_Renderer* gRenderer = Renderer::GetInstance()->mRenderer;
    SDL_Rect renderQuad = { x, y, width, height };

    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderCopy(gRenderer, mTexture, clip, &renderQuad);
}
