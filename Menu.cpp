#include "Menu.h"
#include "Resources.h"
#include "Globals.h"

#include <Windows.h>
#include <thread>

void PZS::Button::Update(void) noexcept
{
	SDL_Point mouse_pos;
	bool is_mouse_clicked = SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y) & SDL_BUTTON_LMASK;

	if (SDL_PointInRect(&mouse_pos, &button_rectangle)) 
	{
		DoShake();

		SetCursor(LoadCursor(NULL, IDC_HAND));

		/* Mouse button is held down, the button changes size, but nothing else happens */
		if (is_mouse_clicked && !has_been_clicked) {
			has_been_clicked = true;
			AdjustButtonSize(true);
		}

		/* Mouse button is let go */
		else if (!is_mouse_clicked && has_been_clicked) {
			AdjustButtonSize(false);
			is_clicked = true;
			has_been_clicked = false;
		}

		/* This happens the iteration after the mouse is let go, in order to disable clicking too many times*/
		else
			is_clicked = false;

	}

	else
	{
		is_clicked = false;
		has_been_clicked = false;

		angle = 0.0f;
		angle_dir = abs(angle_dir);

		AdjustButtonSize(false);
	}
}

void PZS::Button::DoShake(void)
{
	timePassedMilliseconds += clock() - begin;

	if (timePassedMilliseconds > 5) {
		angle += angle_dir;
		if (angle == angle_dir * 3)
			angle_dir = -angle_dir;
	}

	begin = clock();
}

bool PZS::Button::IsClicked(void) noexcept
{
	return is_clicked;
}

void PZS::Button::Render(void) noexcept
{
	mTexture->Render(button_rectangle, &clip, angle);
}

void PZS::Button::AdjustButtonSize(bool mob) noexcept
{
	if (mob) {
		button_rectangle = {
			original_position.x - static_cast<int>((scale - 1) * original_position.w) / 2,
			original_position.y - static_cast<int>((scale - 1) * original_position.h) / 2,
			static_cast<int>(original_position.w * scale),
			static_cast<int>(original_position.h * scale)
		};
	}

	else
		button_rectangle = original_position;
}

void PZS::Button::Init(void) noexcept
{
	mTexture = Resources::GetInstance()->gTextures.Get("button_sheet");
}

PZS::Button::Button(SDL_Rect clip, SDL_Rect button_rect, float scale, bool shake) noexcept
{
	button_rectangle = (original_position = button_rect);

	this->clip = clip;
	this->scale = scale;

	should_shake = shake;

	begin = clock();
}

#ifdef DIFF
#undef DIFF
#endif

#define DIFF 40

void ThreadDoControlsTabMovement(SDL_Rect* rect, bool* show)
{
	int diff = 0, original_y = rect->y, lim;

	/* Choose whether tab goes up or down */
	if (rect->y > 0) {
		diff = -DIFF;
		lim = -740;
	}
	else {
		diff = DIFF;
		lim = 20;
	}

	/* If tab is going down, show it */
	if (diff > 0)
		*show = true;

	while (rect->y != lim) {
		rect->y += diff;
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	/* Once the movement has finished, if it was going up, hide it */
	if (diff < 0)
		*show = false;
}

#undef DIFF

PZS::Menu::Menu(void) noexcept
	: play_button({ 0, 0, 456, 128 }, { 400 - 152, 400, 304, 85 }, 1.05f, true)
	, controls_button({ 0, 128, 456, 128 }, { 400 - 152, 500, 304, 85 }, 1.05f, true)
{
	
} 

const SDL_Rect screen_rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };

void PZS::Menu::Render(void) noexcept
{
	SDL_Renderer* gRenderer = Renderer::GetInstance()->mRenderer;

	menu_background->Render(screen_rect);

	play_button.Render();
	controls_button.Render();

	if (show_controls) {

		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 128);
		SDL_SetRenderDrawBlendMode(gRenderer, SDL_BlendMode::SDL_BLENDMODE_BLEND);

		SDL_RenderFillRect(gRenderer, &screen_rect);

		SDL_SetRenderDrawBlendMode(gRenderer, SDL_BlendMode::SDL_BLENDMODE_NONE);

		controls_tab->Render(controls_rect);
	}
}

void PZS::Menu::Update(void) noexcept
{
	SDL_Point mouse_pos;

	bool clicked = SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y) & SDL_BUTTON_LMASK;
	const Uint8* keyboard_state = SDL_GetKeyboardState(NULL);

	if (!show_controls) {
		play_button.Update();
		controls_button.Update();
	}

	if (play_button.IsClicked()) {
		g_game_state = GameState::GS_LOAD_GAME_GRAPHICS;
		menu_music->Stop();
		button_click_sfx->Play();
	}

	/* Move tab down when button is pressed */
	if ((controls_button.IsClicked() && !show_controls)) {
		std::thread con_thr(ThreadDoControlsTabMovement, &controls_rect, &show_controls);
		con_thr.detach();
		button_click_sfx->Play();
	}

	/*  Magic number */
	if (controls_rect.y == 20)
		going_up = false;

	/* Move tab up when escape is pressed */
	if (show_controls && keyboard_state[SDL_SCANCODE_ESCAPE] && !going_up) {
		going_up = true;

		std::thread con_thr(ThreadDoControlsTabMovement, &controls_rect, &show_controls);
		con_thr.detach();
	}
}

void PZS::Menu::Init(void) noexcept
{
	Resources* resources = Resources::GetInstance();

	menu_background = resources->gTextures.Get("menu_background");
	controls_tab = resources->gTextures.Get("controls");
	menu_music = resources->gMusic.Get("menu_loop");
	button_click_sfx = resources->gSFX.Get("button_click");

	menu_music->Play(-1);

	play_button.Init();
	controls_button.Init();
}