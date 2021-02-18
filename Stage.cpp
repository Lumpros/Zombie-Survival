#include "Stage.h"
#include "Player.h"
#include "RandomNumberGenerator.h"

#include <thread>
#include <chrono>

void ThreadCallSpawnZombie(int wave, volatile bool* is_paused)
{
	std::this_thread::sleep_for(std::chrono::seconds(3));

	PZS::Stage* stage_ptr = PZS::Stage::Get();

	int count = stage_ptr->GetZombieLimit();

	int delay = 50 * wave;
	if (wave > 16)
		delay = 800;

	while (count-- > 0) {
		stage_ptr->SpawnZombie();

		while (*is_paused);

		std::this_thread::sleep_for(std::chrono::milliseconds(PZS::RNG::RandInt(1000 - delay, 1500 - delay)));
	}
}

void ThreadCallShowWaveText(SDL_Texture* text_texture, bool *is_visible, volatile bool* is_paused)
{	
	for (Uint8 alpha = 0; alpha < 255; alpha += 5) {
		SDL_SetTextureAlphaMod(text_texture, alpha);
		std::this_thread::sleep_for(std::chrono::milliseconds(20));

		while (*is_paused);
	}

	do {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	} while ((*is_paused));

	/* Careful with the unsigned */
	for (Uint8 alpha = 255; alpha > 0; alpha -= 5) {
		SDL_SetTextureAlphaMod(text_texture, alpha);
		std::this_thread::sleep_for(std::chrono::milliseconds(20));

		while (*is_paused);
	}

	*is_visible = false;
}

int PZS::Stage::GetZombieLimit(void) const noexcept
{
	return CalculateZombieSpawnNumber(wave);
}

void PZS::Stage::ShowMouseTarget(void) noexcept
{
	if (is_paused || Player::Get()->GetHP() <= 0)
		return;

	/* Render target on mouse */
	SDL_Point mouse;
	Vector2D target_dimension{ 20, 20 };
	SDL_GetMouseState(&mouse.x, &mouse.y);

	target_texture->Render({
		mouse.x - target_dimension.x / 2,
		mouse.y - target_dimension.y / 2 ,
		target_dimension.x,
		target_dimension.y
		});
}

void PZS::Stage::DisplayGameplayRelated(void) noexcept
{
	background_texture->Render({ 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT });

	/* Drops */
	for (size_t i = 0; i < g_drops.size(); ++i)
		g_drops[i]->Render();

	/* Player */
	Player::Get()->Render();

	/* Zombies */
	for (size_t i = 0; i < g_zombies.size(); ++i)
		g_zombies[i]->Render();
}

void PZS::Stage::ShowGameOverScreen(void) noexcept
{
	DrawAlphaScreen();

	game_over_text->Render(WINDOW_WIDTH / 2 - game_over_text->GetWidth() / 2, 250);
	main_menu_button->Render();
}

void PZS::Stage::DoNextStage(void) noexcept
{
	zombies_left = CalculateZombieSpawnNumber(++wave);
	HandleZombieCount();

	wave_is_visible = true;

	char buf[16];
	sprintf_s(buf, 16, "Wave %d", wave);

	Text* text_ptr = new Text(Resources::GetInstance()->gFonts.Get("zombie_font_2"), buf, { 0xFF, 0x00, 0x00, 0xFF });

	SDL_SetTextureAlphaMod(text_ptr->GetGeneratedTexture(), 0x00);

	wave_text.reset(text_ptr);

	std::thread show_wave_thread(ThreadCallShowWaveText, text_ptr->GetGeneratedTexture(), &wave_is_visible, &is_paused);
	std::thread spawn_zombie_thread(ThreadCallSpawnZombie, wave, &is_paused);

	show_wave_thread.detach();
	spawn_zombie_thread.detach();
}

void PZS::Stage::DrawAlphaScreen(void) noexcept
{
	SDL_Renderer* pRenderer = Renderer::GetInstance()->mRenderer;

	constexpr SDL_Rect screen_rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };

	SDL_SetRenderDrawBlendMode(pRenderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(pRenderer, 0x00, 0x00, 0x00, 128);

	SDL_RenderFillRect(pRenderer, &screen_rect);

	SDL_SetRenderDrawBlendMode(pRenderer, SDL_BLENDMODE_NONE);
}

void PZS::Stage::DrawPauseMenu(void) noexcept
{
	DrawAlphaScreen();
	pause_text->Render(
		WINDOW_WIDTH / 2 - pause_text->GetWidth() / 2,
		WINDOW_HEIGHT / 2 - pause_text->GetHeight() / 2
		);

	sound_button.Render();

	audio_slider->Render();
	
}

void PZS::Stage::UpdateGameplayRelated(void) noexcept
{
	if (Player::Get()->GetHP() <= 0)
		return;

	const Uint8* keyboard_state = SDL_GetKeyboardState(NULL);

	if (keyboard_state[SDL_SCANCODE_P] && !p_is_pressed) 
	{
		p_is_pressed = true;
		is_paused = !is_paused;
		audio_slider->Enable(false);
		SDL_ShowCursor(is_paused);

		for (size_t i = 0; i < g_drops.size(); ++i)
			g_drops[i]->set_timer_for_pause();
	}

	else if (!keyboard_state[SDL_SCANCODE_P])
		p_is_pressed = false;

	if (!is_paused)
	{
		if (zombies_left <= 0)
			DoNextStage();

		/* Zombie */
		for (size_t i = 0; i < g_zombies.size(); ++i)
		{
			g_zombies[i]->Update();
			if (g_zombies[i]->IsDead())
				g_zombies.erase(g_zombies.begin() + (i--));
		}

		/* Drops */
		for (size_t i = 0; i < g_drops.size(); ++i)
			g_drops[i]->Update();

		/* Player */
		Player::Get()->Update();
	}
}

void PZS::Stage::Render(void) noexcept
{
 	DisplayGameplayRelated();

	DisplayUI();

	ShowMouseTarget();
}

void PZS::Stage::UpdateUI(void) noexcept
{
	Player* player_ptr = Player::Get();

	if (!is_paused)
	{
		HandleHealthCount(player_ptr);
		HandleAmmoCount(player_ptr);
		HandleZombieCount();

		/* Gun boxes */
		rifle_box.Update();
		shotgun_box.Update();
		pistol_box.Update();

		/* Arrow */
		arrow.Update();
	}

	
	else {
		sound_button.Update();

		if (sound_button.IsClicked()) {
			slider_shown = !slider_shown;
			audio_slider->Enable(slider_shown);
		}

		audio_slider->Update();
	}

	if (Player::Get()->GetHP() <= 0)
	{
		main_menu_button->Update();

		if (main_menu_button->IsClicked()) {
			g_game_state = GameState::GS_MENU;
		}
	}
}

void PZS::Stage::DisplayUI(void) noexcept
{
	/* Gun Boxes */
	shotgun_box.Render();
	rifle_box.Render();
	pistol_box.Render();

	/* Arrow above boxes*/
	arrow.Render();

	health_text->Render(340, 690);
	ammo_text->Render(340, 720);
	zombies_left_text->Render(400 - zombies_left_text->GetWidth() / 2, 20);
	
	if (wave_is_visible) 
	{ 
		wave_text->Render(
			WINDOW_WIDTH  / 2 - wave_text->GetWidth()  / 2,
			WINDOW_HEIGHT / 2 - wave_text->GetHeight() / 2
		);
	}

	if (is_paused)
		DrawPauseMenu();

	if (Player::Get()->GetHP() <= 0)
		ShowGameOverScreen();
}

void PZS::Stage::HandleHealthCount(Player* player_ptr) noexcept
{
	int player_hp = player_ptr->GetHP();

	/* If player hp has changed, create new text texture */
	if (old_hp != player_hp) {
		old_hp = player_hp;

		char buf[20];
		sprintf_s(buf, 20, "Health: %d/100", player_hp);

		health_text->SetText(buf);
	}
}

void PZS::Stage::HandleAmmoCount(Player* player_ptr) noexcept
{
	/* next 3 used to avoid calling over and over */
	GunIndex index = player_ptr->GetGunIndex();

	int loaded_ammo = player_ptr->GetGun(index)->GetLoadedAmmo();
	int ammo_cap = player_ptr->GetGun(index)->GetAmmoCap();

	/* Check if the player has reloaded/shot */
	/* If he has, create new text texture */
	if (ammo_cap != old_cap || loaded_ammo != old_ammo) {
		old_cap = ammo_cap;
		old_ammo = loaded_ammo;

		char buf[20];
		sprintf_s(buf, 20, "Ammo: %d/%d", loaded_ammo, ammo_cap);

		ammo_text->SetText(buf);
	}
}

void PZS::Stage::HandleZombieCount(void) noexcept
{
	if (old_zombie_count != zombies_left) {
		old_zombie_count = zombies_left;

		/* Update text */
		char buf[25];
		sprintf_s(buf, 25, "%d ZOMBIE%s LEFT", zombies_left, (zombies_left == 1 ? "" : "S"));
		zombies_left_text->SetText(buf);
	}
}

void PZS::Stage::Update(void) noexcept
{
	int count_before_update = GetAliveZombieCount();

	UpdateGameplayRelated();

	int count_after_update = GetAliveZombieCount();

	if (count_after_update < count_before_update)
		zombies_left -= count_before_update - count_after_update;

	UpdateUI();
}

int PZS::Stage::CalculateZombieSpawnNumber(int x) const noexcept
{
	return x * x + 6;
}

/* Rewrite this maybe? this is ugly */
void PZS::Stage::SpawnZombie(void) noexcept
{
	constexpr int x_axis = 0;
	constexpr int y_axis = 1;

	SDL_Point zombie_position;

	int choice_x_or_y = RNG::RandInt(x_axis, y_axis);

	if (choice_x_or_y == x_axis)
	{
		zombie_position.y = RNG::RandInt(-ZOMBIE_HEIGHT, WINDOW_HEIGHT);

		constexpr int left = 0;
		constexpr int right = 1;

		int choose_left_right = RNG::RandInt(left, right);

		if (choose_left_right == left)
			zombie_position.x = -ZOMBIE_WIDTH;

		else
			zombie_position.x = WINDOW_WIDTH;
	}

	else
	{
		zombie_position.x = RNG::RandInt(-ZOMBIE_WIDTH, WINDOW_WIDTH);

		constexpr int top = 0;
		constexpr int bottom = 1;

		int choose_top_or_bot = RNG::RandInt(top, bottom);

		if (choose_top_or_bot == top)
			zombie_position.y = -ZOMBIE_HEIGHT;
		else
			zombie_position.y = WINDOW_HEIGHT;
	}

	g_zombies.push_back(std::make_unique<Zombie>(SDL_Rect({ zombie_position.x, zombie_position.y, ZOMBIE_WIDTH, ZOMBIE_HEIGHT })));
}

void PZS::Stage::InitializeTextPointers(Resources* resources) noexcept
{
	ammo_text = new Text(resources->gFonts.Get("font"), "Ammo: 6/6", { 0xFF, 0xFF, 0xFF, 0xFF });
	health_text = new Text(resources->gFonts.Get("font"), "Health: 100/100", { 0xFF, 0xFF, 0xFF, 0xFF });
	zombies_left_text = new Text(resources->gFonts.Get("zombie_font"), ".", { 0xFF, 0xFF, 0x00, 0xFF });
	pause_text = new Text(resources->gFonts.Get("zombie_font_2"), "PAUSED", { 0xFF, 0xFF, 0xFF, 0xFF });
	game_over_text = new Text(resources->gFonts.Get("zombie_font_2"), "GAME OVER", { 0xFF, 0x00, 0x00, 0xFF });

	char buf[25];
	sprintf_s(buf, 25, "%d ZOMBIES LEFT", zombies_left);
	zombies_left_text->SetText(buf);
}

PZS::Stage::Stage(void) noexcept :
	rifle_box(RIFLE, { 120, WINDOW_HEIGHT - 110, 100, 100 }),
	shotgun_box(SHOTGUN, { 230, WINDOW_HEIGHT - 110, 100, 100 }),
	pistol_box(PISTOL, { 10, WINDOW_HEIGHT - 110, 100, 100 }),
	sound_button({ 456, 0, 64, 64 }, { 758, 758, 32, 32 }, 1.0f, false)
{
	Resources* resources = Resources::GetInstance();

	background_texture = resources->gTextures.Get("debug");
	target_texture = resources->gTextures.Get("target");

	main_menu_button = std::make_unique<Button>(SDL_Rect { 0, 256, 456, 128 }, SDL_Rect { 400 - 152, 350, 304, 85 }, 1.05f, true);

	sound_button.Init();
	main_menu_button->Init();

	InitializeTextPointers(resources);
}

void PZS::Stage::Reset(void) noexcept
{
	if (ammo_text != nullptr) {
		DeleteText();
		InitializeTextPointers(Resources::GetInstance());
	}

	Player::Get()->Reset();

	is_paused = false;
	p_is_pressed = false;

	slider_shown = false;
	wave_is_visible = false;

	wave = 0;
	zombies_left = 0;

	old_hp = 100;
	old_cap = 6;
	old_ammo = 6;
	old_zombie_count = 0;

	g_drops.clear();
	g_player_bullets.clear();
	g_zombies.clear();

	audio_slider->Enable(false);
}


///////////////////////////// GunBox Section /////////////////////////////
SDL_Color PZS::GunBox::DecideBoxColor(void)
{
	SDL_Color color;

	if (Player::Get()->WeaponIsActive(index))
		color = { 180, 180, 180, 128 };
	else
		color = { 227, 0, 0, 128 };

	return color;
}

PZS::GunBox::GunBox(GunIndex index, SDL_Rect rect) noexcept 
	: bullet_count(Resources::GetInstance()->gFonts.Get("font"), "0", { 0xFF, 0xFF, 0xFF, 0xFF })
{
	this->index = index;
	hitbox = rect;

	ChooseGunTexture(index);
}

void PZS::GunBox::ChooseGunTexture(GunIndex gunIndex)
{
	switch (gunIndex) 
	{
	case PISTOL:
		mTexture = Resources::GetInstance()->gTextures.Get("pistol");
		break;

	case RIFLE:
		mTexture = Resources::GetInstance()->gTextures.Get("rifle");
		break;

	case SHOTGUN:
		mTexture = Resources::GetInstance()->gTextures.Get("shotgun");
		break;
	}
}

SDL_Color PZS::GunBox::DecideNumColor(void)
{
	return { 0xFF, 0xFF, 0xFF, 0xFF }; // white
}

void PZS::GunBox::Render(void) noexcept
{
	SDL_Color bc = DecideBoxColor(); // bc = box color
	SDL_Renderer* renderer = Renderer::GetInstance()->mRenderer;

	SDL_SetRenderDrawColor(renderer, bc.r, bc.g, bc.b, bc.a);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BlendMode::SDL_BLENDMODE_BLEND);

	SDL_RenderFillRect(renderer, &hitbox);

	mTexture->SetAlpha(bc.r == 227 ? 128 : 0xFF);
	mTexture->Render(hitbox);
	mTexture->SetAlpha(0xFF);

	SDL_Color tc = DecideNumColor(); // tc = text color
	SDL_SetRenderDrawColor(renderer, tc.r, tc.g, tc.b, tc.a);

	bullet_count.Render(
		hitbox.x + hitbox.w - bullet_count.GetWidth() - 10,
		hitbox.y + hitbox.h - bullet_count.GetHeight() - 10
	);

	/* Reset */
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BlendMode::SDL_BLENDMODE_NONE);
}

void PZS::GunBox::Update(void) noexcept
{
	int b_count = Player::Get()->GetGun(index)->GetTotalAmmo();

	if (b_count != old_bullet_count)
	{
		old_bullet_count = b_count;

		char buf[10]; /* Way more than enough lol */
		sprintf_s(buf, 10, "%d", b_count);

		bullet_count.SetText(buf);
	}
}


//////////////////////////// BoxArrow Section ////////////////////////////
PZS::BoxArrow::BoxArrow(void)
{
	hitbox = { 39, 800 - 150, arrow_sz, arrow_sz };
	mTexture = Resources::GetInstance()->gTextures.Get("arrow");

	end = clock();
}

void PZS::BoxArrow::Render(void) noexcept
{
	mTexture->Render(hitbox);
}

void PZS::BoxArrow::Update(void) noexcept
{
	hitbox.x = static_cast<int>(Player::Get()->GetGunIndex()) * 110 + 60 - arrow_sz / 2;

	timeElapsed += clock() - end;
	if (timeElapsed >= 100)
	{
		timeElapsed = 0;
		hitbox.y += direction;
		
		/* Magic numbers <3 */
		if (hitbox.y == WINDOW_HEIGHT - 150)
			direction = 1;
		else if (hitbox.y == WINDOW_HEIGHT - 145)
			direction = -1;
	}

	end = clock();
}