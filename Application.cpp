#include "Application.h"
#include "Resources.h"
#include "Sound.h"
#include "Texture.h"
#include "RandomNumberGenerator.h"
#include "Globals.h"

#include <thread>

PZS::GameState PZS::g_game_state = PZS::GameState::GS_MENU;

void ThreadLoadGameResources(PZS::Resources* resources, bool* has_loaded);

void PZS::Application::Loop(void) noexcept
{
	bool should_quit = false;
	SDL_Event sdl_event;
	
	clock_t clock_before_frame, ticksElapsed;

	bool space_pressed = false;

	while (!should_quit)
	{
		clock_before_frame = clock();

		while (SDL_PollEvent(&sdl_event))
			if (sdl_event.type == SDL_QUIT)
				should_quit = true;

		Update();
		Render();

		ticksElapsed = clock() - clock_before_frame;

		/* Cap Framerate */
		if (ticksElapsed < SCREEN_TICKS_PER_FRAME)
			SDL_Delay(SCREEN_TICKS_PER_FRAME - ticksElapsed);
	}
} 

void PZS::Application::Update(void) noexcept
{
	switch (g_game_state) 
	{
	case GameState::GS_PLAYING:
		Stage::Get()->Update();
		break;
		
	case GameState::GS_MENU:
		game_menu.Update();
		break;

	case GameState::GS_START:
		if (has_loaded_game_resources) {
			Stage::Get()->RegisterAudioSlider(game_menu.slider);

#ifndef DEBUG
			Stage::Get()->Reset();
#endif
		}
		break;

	/* Load game graphics after initializing the window, so that */
	/* It doesnt take 2 hours to open the first time lol */
	case GameState::GS_LOAD_GAME_GRAPHICS:
		std::thread load_graphics_thr(ThreadLoadGameResources, Resources::GetInstance(), &has_loaded_game_resources);
		load_graphics_thr.detach();

		g_game_state = GameState::GS_START;
		break;
	}
}

#ifdef DIFF
#undef DIFF
#endif

#define DIFF 5

void PZS::Application::DoBlackScreen(PZS::GameState new_state) noexcept
{
	static SDL_Renderer* renderer = PZS::Renderer::GetInstance()->mRenderer;
	static const SDL_Rect screen_rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };

	static Uint8 opacity = 0x00;
	static signed int diff = DIFF;
	static clock_t timePassedMilliseconds = 0;
	
	static clock_t begin = clock();

	timePassedMilliseconds += clock() - begin;

	if (timePassedMilliseconds > 5) {
		timePassedMilliseconds = 0;

		if ((diff > 0 && opacity != 0xFF) || (diff < 0 && has_loaded_game_resources))
			opacity += diff;

		if (opacity == 0xFF && has_loaded_game_resources) {
			g_game_state = new_state;
			diff = -DIFF;
		}

		if (diff < 0 && opacity == 0x00) {
			show_black_screen = false;
			diff = DIFF;
		}
	}

	SDL_SetRenderDrawBlendMode(renderer, SDL_BlendMode::SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, opacity);

	SDL_RenderFillRect(renderer, &screen_rect);

	SDL_SetRenderDrawBlendMode(renderer, SDL_BlendMode::SDL_BLENDMODE_NONE);
	
	begin = clock();
}

#undef DIFF

void PZS::Application::Render(void) noexcept
{
	SDL_Renderer* gRenderer = Renderer::GetInstance()->mRenderer;

	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderClear(gRenderer);

	if (g_game_state == GameState::GS_PLAYING)
		Stage::Get()->Render();

	else
	{
		game_menu.Render();

		if (GameState::GS_START == g_game_state) {
			SDL_ShowCursor(0);
			show_black_screen = true;
		}
	}

	if (show_black_screen)
		DoBlackScreen(GameState::GS_PLAYING);

	SDL_RenderPresent(gRenderer);
}

void PZS::Application::LoadResources(void) const noexcept
{
	Resources* resources = Resources::GetInstance();

	resources->gFonts.Add("font", new Font("Media/Fonts/font.ttf", 20));
	resources->gFonts.Add("zombie_font", new Font("Media/Fonts/zombie_font.ttf", 30));
	resources->gFonts.Add("zombie_font_2", new Font("Media/Fonts/zombie_font_2.ttf", 69));

	resources->gMusic.Add("menu_loop", new Music("Media/Audio/Music/haunted.ogg"));

	resources->gSFX.Add("button_click", new SFX("Media/Audio/SFX/button_click.wav"));

	resources->gTextures.Add("slider", new Texture("Media/Menu/slider.png"));
	resources->gTextures.Add("menu_background", new Texture("Media/Menu/menu_background.png"));
	resources->gTextures.Add("button_sheet", new Texture("Media/Menu/button_sheet.png"));
	resources->gTextures.Add("controls", new Texture("Media/Menu/controls.png"));
}

PZS::Application::~Application(void) noexcept
{
	SDL_DestroyWindow(mWindow);

	Resources::GetInstance()->Destroy();

	SDL_Quit();
	IMG_Quit();
	TTF_Quit();
	Mix_Quit();

	Mix_CloseAudio();
}

bool NODISCARD PZS::Application::Initialize(void) noexcept
{
	Logger("Initializing...");

	if ((SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0))
	{
		PRINT_SDL_ERROR("Failed to Initialize SDL");
		return false;
	}

	if ((Mix_Init(MIX_INIT_OGG) & MIX_INIT_OGG) != MIX_INIT_OGG)
	{
		PRINT_MIX_ERROR("Failed to Init OGG support");
		return false;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
	{
		PRINT_MIX_ERROR("Failed to open audio");
		return false;
	}

	Mix_AllocateChannels(30);
	Mix_Volume(-1, MIX_MAX_VOLUME);

	Logger("SDL Initialized!");

	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"))
		puts("Warning: Linear texture filtering not enabled!");

	if (!(mWindow = SDL_CreateWindow("Zombie Survival", SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_HIDDEN)))
	{
		PRINT_SDL_ERROR("Failed to create SDL window");
		return false;
	}

	Logger("Window Created!");

	if (!(Renderer::GetInstance()->mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)))
	{
		PRINT_SDL_ERROR("Failed to create renderer");
		return false;
	}

	Logger("Renderer Created!");

	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
	{
		PRINT_IMG_ERROR("SDL_image could not initialize!");
		return false;
	}

	Logger("SDL_Image Initialized!");

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		PRINT_MIX_ERROR("SDL_mixer could not initialize!");
		return false;
	}

	Logger("SDL Audio Initialized!");

	if (TTF_Init() == -1)
	{
		PRINT_TTF_ERROR("SDL_ttf could not initialize!");
		return false;
	}

	Logger("SDL_TTF Initialized!");
	Logger("Initialization done!");

	Logger("Loading media...");
	LoadResources();
	Logger("Loading done!");

	/* To avoid showing a white screen while loading */
	SDL_ShowWindow(mWindow);
	//SDL_ShowCursor(0);

	Logger("Window Shown!");
	Logger("Done");

	game_menu.Init();
	
	return true;
}

void ThreadLoadGameResources(PZS::Resources* resources, bool* has_loaded)
{
	using namespace PZS;

	if (*has_loaded)
		return;

	resources->gTextures.Add("debug", new Texture("Media/Sprites/Other/debug.png"));

	resources->gTextures.Add("zombie_walking_sheet", new Texture("Media/Sprites/Zombie/zombie_walking.png"));
	resources->gTextures.Add("zombie_attack_sheet", new Texture("Media/Sprites/Zombie/zombie_attack.png"));

	resources->gTextures.Add("player_pistol_walking_sheet", new Texture("Media/Sprites/Player/player_pistol_walking.png"));
	resources->gTextures.Add("player_pistol_standing_sheet", new Texture("Media/Sprites/Player/player_pistol_standing.png"));
	resources->gTextures.Add("player_pistol_shooting_sheet", new Texture("Media/Sprites/Player/player_pistol_shooting.png"));
	resources->gTextures.Add("player_pistol_reloading_sheet", new Texture("Media/Sprites/Player/player_pistol_reloading.png"));
	resources->gTextures.Add("player_pistol_meelee_sheet", new Texture("Media/Sprites/Player/player_pistol_meelee.png"));

	resources->gTextures.Add("player_shotgun_walking_sheet", new Texture("Media/Sprites/Player/player_shotgun_walking.png"));
	resources->gTextures.Add("player_shotgun_standing_sheet", new Texture("Media/Sprites/Player/player_shotgun_standing.png"));
	resources->gTextures.Add("player_shotgun_shooting_sheet", new Texture("Media/Sprites/Player/player_shotgun_shooting.png"));
	resources->gTextures.Add("player_shotgun_reloading_sheet", new Texture("Media/Sprites/Player/player_shotgun_reloading.png"));

	resources->gTextures.Add("player_rifle_walking_sheet", new Texture("Media/Sprites/Player/player_rifle_walking.png"));
	resources->gTextures.Add("player_rifle_standing_sheet", new Texture("Media/Sprites/Player/player_rifle_standing.png"));
	resources->gTextures.Add("player_rifle_shooting_sheet", new Texture("Media/Sprites/Player/player_rifle_shooting.png"));
	resources->gTextures.Add("player_rifle_reloading_sheet", new Texture("Media/Sprites/Player/player_rifle_reloading.png"));

	resources->gTextures.Add("heal_sheet", new Texture("Media/Sprites/Other/heal_effect_sheet.png"));

	resources->gTextures.Add("shotgun", new Texture("Media/Sprites/Other/shotgun.png"));
	resources->gTextures.Add("shotgun_ammo", new Texture("Media/Sprites/Drops/shotgun_ammo.png"));
	resources->gTextures.Add("rifle", new Texture("Media/Sprites/Other/rifle.png"));
	resources->gTextures.Add("rifle_ammo", new Texture("Media/Sprites/Drops/rifle_ammo.png"));
	resources->gTextures.Add("pistol", new Texture("Media/Sprites/Other/pistol.png"));
	resources->gTextures.Add("pistol_ammo", new Texture("Media/Sprites/Drops/pistol_ammo.png"));
	resources->gTextures.Add("instakill", new Texture("Media/Sprites/Drops/instakill.png"));
	resources->gTextures.Add("rapid_fire", new Texture("Media/Sprites/Drops/rapid_fire.png"));
	resources->gTextures.Add("infinite", new Texture("Media/Sprites/Drops/infinite.png"));

	resources->gTextures.Add("medkit", new Texture("Media/Sprites/Drops/medkit.png"));
	resources->gTextures.Add("target", new Texture("Media/Sprites/Other/target.png"));
	resources->gTextures.Add("arrow", new Texture("Media/Sprites/Other/arrow.png"));
	resources->gTextures.Add("bullet", new Texture("Media/Sprites/Other/bullet.png"));
	resources->gTextures.Add("nuke", new Texture("Media/Sprites/Drops/nuke.png"));
	resources->gTextures.Add("bullet_explosion_sheet", new Texture("Media/Sprites/Other/bullet_explosion.png"));
	resources->gTextures.Add("blood_splatter", new Texture("Media/Sprites/Other/blood_splatter.png"));

	resources->gSFX.Add("shotgun_shot", new SFX("Media/Audio/SFX/shotgun_shot.wav"));
	resources->gSFX.Add("pistol_shot", new SFX("Media/Audio/SFX/pistol_shot.wav"));
	resources->gSFX.Add("shotgun_reload", new SFX("Media/Audio/SFX/shotgun_reload.wav"));
	resources->gSFX.Add("rifle_reload", new SFX("Media/Audio/SFX/rifle_reload.wav"));
	resources->gSFX.Add("explosion", new SFX("Media/Audio/SFX/explosion.wav"));
	resources->gSFX.Add("heal_pickup", new SFX("Media/Audio/SFX/heal_pickup.wav"));
	resources->gSFX.Add("gun_pickup", new SFX("Media/Audio/SFX/gun_pickup.wav"));
	resources->gSFX.Add("ammo_pickup", new SFX("Media/Audio/SFX/ammo_pickup.wav"));
	resources->gSFX.Add("blood_splatter", new SFX("Media/Audio/SFX/blood_splatter.wav"));

	*has_loaded = true;
}
