#include "Player.h"
#include "Resources.h"
#include "Preprocessors.h"

#define DIFF 38
#define HALF (DIFF / 2)

PZS::Player::Player(void) noexcept
{
	/* Default state is standing with a pistol */
	mTexture = still_sheet[PISTOL];
	current_animation = &still_animation[PISTOL];

	/* Initialize hitboxes accoording to size (guns differ) */
	hitboxes[PISTOL] = { 300, 300, 64, 64 };
	hitboxes[SHOTGUN] = { 300, 300, 80, 64 };
	hitboxes[RIFLE] = { 300, 300, 80, 64 };

	/* Default hitbox is the pistol */
	hitbox = hitboxes[PISTOL];

	/* Initialize animations and textures */
	Resources* resources = Resources::GetInstance();

	reload_sfx[PISTOL] = resources->gSFX.Get("rifle_reload");
	reload_sfx[SHOTGUN] = resources->gSFX.Get("shotgun_reload");
	reload_sfx[RIFLE] = reload_sfx[PISTOL];

	damage_sfx = resources->gSFX.Get("blood_splatter");

	InitializePistolGraphics(resources);
	InitializeShotgunGraphics(resources);
	InitializeBloodGraphics(resources);
	InitializeRifleGraphics(resources);
	InitializeHealGraphics(resources);


	/* disable all guns, they will be picked up later */
	for (size_t i = 0; i < GUN_COUNT; ++i)
		available_guns[i] = false;

	/* pistol is enabled by default */
	available_guns[PISTOL] = true;

#ifdef DEBUG
	available_guns[SHOTGUN] = true;
	available_guns[RIFLE] = true;
#endif
}

void PZS::Player::InitializeHealGraphics(Resources* resources) noexcept
{
	for (int y = 0; y < 4; ++y)
		for (int x = 0; x < 4; ++x)
			heal_animation.AddFrame({ { x * 192, y * 192, 192, 192 }, 20 });
	heal_animation.AddFrame({ { 0, 4 * 192, 192, 192 }, 20 });

	heal_sheet = resources->gTextures.Get("heal_sheet");

	heal_animation.Pause(true);
}

void PZS::Player::InitializeRifleGraphics(Resources* resources) noexcept
{
	guns[RIFLE] = make_unique<Rifle>();

	walk_sheet[RIFLE] = resources->gTextures.Get("player_rifle_walking_sheet");
	still_sheet[RIFLE] = resources->gTextures.Get("player_rifle_standing_sheet");
	shoot_sheet[RIFLE] = resources->gTextures.Get("player_rifle_shooting_sheet");
	reload_sheet[RIFLE] = resources->gTextures.Get("player_rifle_reloading_sheet");

	//  Walking
	// 313 x 206
	// 4 x 5
	for (int y = 0; y < 5; ++y)
		for (int x = 0; x < 4; ++x)
			walk_animation[RIFLE].AddFrame({ { x * 313, y * 206, 313, 206 }, 50 });

	// Standing
	// 313 x 207
	// 4 x 5
	for (int y = 0; y < 5; ++y)
		for (int x = 0; x < 4; ++x)
			still_animation[RIFLE].AddFrame({ { x * 313, y * 207, 313, 207 }, 50 });

	// Shooting
	// 312 x 206
	// 3 x 1
	for (int x = 0; x < 3; ++x)
		shooting_animation[RIFLE].AddFrame({ { x * 312, 0, 312, 206 }, guns[RIFLE]->GetShotRate() / 3 });

	constexpr int c = 10;

	// Reload
	// 322 x 217
	// 4 x 5
	for (int y = 0; y < 5; ++y)
		for (int x = 0; x < 4; ++x)
			reload_animation[RIFLE].AddFrame({ { x * 322 + c / 2, y * 217, 322 - c, 217 - c }, 50 });

}

void PZS::Player::InitializeBloodGraphics(Resources* resources) noexcept
{
	blood_sheet = resources->gTextures.Get("blood_splatter");

	blood_animation.AddFrame({ { 215 * 2, 0, 215, 222 }, 25 });
	blood_animation.AddFrame({ { 215 * 3, 0, 215, 222 }, 25 });

	for (int i = 1; i < 4; ++i)
		for (int j = 0; j < 4; ++j) // 215 222
			blood_animation.AddFrame({ { j * 215, i * 222, 215, 222 }, 25 });

	blood_animation.Pause(true);
}

void PZS::Player::InitializeShotgunGraphics(Resources* resources) noexcept
{
	guns[SHOTGUN] = make_unique<Shotgun>();

	walk_sheet[SHOTGUN] = resources->gTextures.Get("player_shotgun_walking_sheet");
	still_sheet[SHOTGUN] = resources->gTextures.Get("player_shotgun_standing_sheet");
	shoot_sheet[SHOTGUN] = resources->gTextures.Get("player_shotgun_shooting_sheet");
	reload_sheet[SHOTGUN] = resources->gTextures.Get("player_shotgun_reloading_sheet");

	// Walking
	// 313 x 206
	// 4 * 5
	for (int y = 0; y < 5; ++y)
		for (int x = 0; x < 4; ++x)
			walk_animation[SHOTGUN].AddFrame({ { x * 313, y * 206, 313, 206 }, 50 });

	// Standing
	// 313 x 207 
	// 4 x 5
	for (int y = 0; y < 5; ++y)
		for (int x = 0; x < 4; ++x)
			still_animation[SHOTGUN].AddFrame({ { x * 313, y * 207, 313, 207 }, 50 });

	// Shooting
	// 312 x 206
	// 3 x 1
	for (int x = 0; x < 3; ++x)
		shooting_animation[SHOTGUN].AddFrame({ { x * 312, 0, 312, 206 }, 25 });

	constexpr int c = 10;

	// Reloading
	// 322 x 217
	// 4 x 5
	for (int y = 0; y < 5; ++y)
		for (int x = 0; x < 4; ++x)
			reload_animation[SHOTGUN].AddFrame({ { x * 322 + c / 2, y * 217, 322 - c, 217 - c }, 35 });
}

#ifdef DIFF
#undef DIFF
#endif

#define DIFF 38

void PZS::Player::InitializePistolGraphics(Resources* resources) noexcept
{
	guns[PISTOL] = std::make_unique<Pistol>();

	walk_sheet[PISTOL] = resources->gTextures.Get("player_pistol_walking_sheet");
	still_sheet[PISTOL] = resources->gTextures.Get("player_pistol_standing_sheet");
	shoot_sheet[PISTOL] = resources->gTextures.Get("player_pistol_shooting_sheet");
	reload_sheet[PISTOL] = resources->gTextures.Get("player_pistol_reloading_sheet");
	meelee_sheet[PISTOL] = resources->gTextures.Get("player_pistol_meelee_sheet");

	/* God help me*/

	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 7; ++j)
		{
			if (i == 2 && j == 6)
				break;

			walk_animation[PISTOL].AddFrame({ { j * 258, i * 220, 258, 220 }, 50 });
		}
	}

	// Standing
	// 253 x 216
	// 10 x 2
	for (int i = 0; i < 2; ++i)
		for (int j = 0; j < 10; ++j)
			still_animation[PISTOL].AddFrame({ { j * 253, i * 216, 253, 216 }, 50 });

	// Shooting
	// 255 x 215
	// 3 x 1
	for (int i = 0; i < 3; ++i)
		shooting_animation[PISTOL].AddFrame({ { i * 255, 0, 255, 215 }, 25 });

	/*  Sprites are shit */
	constexpr int c = 11;

	// Reload
	// 260 x 230
	// 7 x 2
	for (int i = 0; i < 2; ++i)
		for (int j = 0; j < 7; ++j)
			reload_animation[PISTOL].AddFrame({ { j * 260 + c / 2, i * 230, 260 - c, 230 - 16}, 50 });

	reload_animation[PISTOL].AddFrame({ { c / 2, 2 * 230, 260 - c, 230 - 16}, 50 });

	// Meelee
	// 291 x 256
	// 5 x 3
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 5; ++j)
			meelee_animation[PISTOL].AddFrame({ { j * 291 + 3, i * 256 + 3, 291 - DIFF, 256 - DIFF}, 30 });
}

/* Look for a gun either to the left or to the right */
/* If a gun is available, use it */
/* If no gun is found, dont change anything*/
void PZS::Player::HandleGunChange(const Uint8* keyboard_state) noexcept
{
	/* Dont swap until an animation is finished */
	if (play_once) return;

	signed int index = (signed int)gun_index;

	/* Use these to avoid changing weapons more than once on accident */
	static bool q_pressed = false;
	static bool e_pressed = false;

	if (keyboard_state[SDL_SCANCODE_Q] && !q_pressed)
	{
		q_pressed = true;

		while (--index >= 0) {
			if (available_guns[index])
				break;
		}

		if (index >= 0)
			gun_index = static_cast<GunIndex>(index);
	}

	else if (!keyboard_state[SDL_SCANCODE_Q])
		q_pressed = false;

	if (keyboard_state[SDL_SCANCODE_E] && !e_pressed)
	{
		e_pressed = true;

		while (++index < GUN_COUNT) {
			if (available_guns[index])
				break;
		}

		if (index < GUN_COUNT)
			gun_index = static_cast<GunIndex>(index);
	}

	else if (!keyboard_state[SDL_SCANCODE_E])
		e_pressed = false;
}

PZS::Player* PZS::Player::Get(void) noexcept
{
	static Player instance;
	return &instance;
}

void PZS::Player::HandleMovement(void) noexcept
{
	const Uint8* keyboard_state = SDL_GetKeyboardState(NULL);

	/* Handle movement input */
	if (keyboard_state[SDL_SCANCODE_A])
		hitbox.x -= movement_speed;

	else if (keyboard_state[SDL_SCANCODE_D])
		hitbox.x += movement_speed;

	if (keyboard_state[SDL_SCANCODE_W])
		hitbox.y -= movement_speed;

	else if (keyboard_state[SDL_SCANCODE_S])
		hitbox.y += movement_speed;

	if (keyboard_state[SDL_SCANCODE_R] && !guns[gun_index]->MagIsFull())
		DoReloadAnimation();

	if (keyboard_state[SDL_SCANCODE_F] && gun_index == PISTOL)
		DoMeeleeAnimation();

	/* Dont let player exit the screen */
	if (hitbox.x > WINDOW_WIDTH - hitboxes[gun_index].w)
		hitbox.x = WINDOW_WIDTH - hitboxes[gun_index].w;

	if (hitbox.x < 0)
		hitbox.x = 0;

	if (hitbox.y > WINDOW_HEIGHT - hitboxes[gun_index].h)
		hitbox.y = WINDOW_HEIGHT - hitboxes[gun_index].h;

	if (hitbox.y < 0)
		hitbox.y = 0;

	HandleGunChange(keyboard_state);
}

void PZS::Player::HandleAnimation(SDL_Rect old_pos) noexcept
{
	/* dont change animation until one is finished */
	if (current_animation->IsRunning() && play_once)
			return;

	play_once = false;

	/* If the player didnt move, use the standing still animation*/
	if (hitbox.x == old_pos.x && hitbox.y == old_pos.y) {
		current_animation = &still_animation[gun_index];
		mTexture = still_sheet[gun_index];
	}

	/* If has moved */
	else {
		current_animation = &walk_animation[gun_index];
		mTexture = walk_sheet[gun_index];
	}
}

#define SQUARE(x) ((x) * (x))

void PZS::Player::HandleMeelee(void) noexcept 
{
	Vector2D mouse_position;
	SDL_GetMouseState(&mouse_position.x, &mouse_position.y);

	static bool stabbed = false;

	/* Cast it to int because it may be reduced to -1 */
	for (int i = 0; i < (int)g_zombies.size(); ++i)
	{
		static constexpr int dif = 4;

		SDL_Rect zombie_rect = g_zombies[i]->hitbox;
		SDL_Rect bigger_rect = {
			zombie_rect.x - dif / 2,
			zombie_rect.y - dif / 2,
			zombie_rect.h + dif,
			zombie_rect.w + dif
		};

		if (current_animation == &meelee_animation[gun_index] && current_animation->GetFrameIndex() == STAB_FRAME && !stabbed)
		{
			/* Calculate a point a certain distance away from the player to the direction he is facing */
			/* If it lands in the zombie's hitbox, it has been stabbed, therefore it should be damaged*/

			/* Calculate the distance */
			std::pair<double, double> vel = Gun::CalculateObjectVelocity({
				hitbox.x + hitbox.w / 2,
				hitbox.y + hitbox.h / 2 },
				mouse_position,
				GetWidth() / 2
				);

			/* Apply the distance */
			SDL_Point hit = { hitbox.x + hitboxes[gun_index].w / 2 + static_cast<int>(round(vel.first)),
							  hitbox.y + hitboxes[gun_index].h / 2 + static_cast<int>(round(vel.second)) };

			/* Apply to a slightly bigger rectangle */
			SDL_Rect big_rect = { zombie_rect.x - 10, zombie_rect.y - 10, zombie_rect.w + 20, zombie_rect.h + 20 };

			/* zombie may be removed here */
			if (SDL_PointInRect(&hit, &big_rect)) {
				g_zombies[i]->Damage(meelee_damage);
				stabbed = true;
			}
		}

		/* If the final zombie is removed, break */
		if (i < 0) break;

		if (g_zombies[i]->GetHP() > 0)
			HandleCollisionWithZombie(zombie_rect);

		if (SDL_HasIntersection(&bigger_rect, &hitboxes[gun_index]))
			g_zombies[i]->DoAttackAnimation();
	}

	if (current_animation == &meelee_animation[gun_index] && current_animation->GetFrameIndex() >= STAB_FRAME + 1)
		stabbed = false;
}

void PZS::Player::Update(void) noexcept
{
	SDL_Rect old_pos = hitboxes[gun_index];

	HandleMovement();

	/* Synchronize all hitboxe positions */
	for (int i = 0; i < GUN_COUNT; ++i) {
		hitboxes[i].x = hitbox.x;
		hitboxes[i].y = hitbox.y;
	}

	hitbox = hitboxes[gun_index];

	/* This must be done after changing the position */
	HandleAnimation(old_pos);

	/* Update gun and check for reload */
	guns[gun_index]->Update(hitbox.x, hitbox.y);

	if (current_animation == &reload_animation[gun_index])
	{
		if (current_animation->GetFrameIndex() == RELOAD_FRAME && !has_reloaded) {
			guns[gun_index]->Reload();
			has_reloaded = true;
		}
	}

	/* Check if player has stabbed a zombie */
	HandleMeelee();

	SDL_Point mouse;
	SDL_GetMouseState(&mouse.x, &mouse.y);

	rotation_angle = CalculateRotationAngle(mouse, { hitbox.x + hitboxes[gun_index].w / 2, hitbox.y + hitboxes[gun_index].h / 2 });

	current_animation->Update(play_once);

	/* Yes, it's supposed to be = not == */
	if (play_blood = blood_animation.IsRunning())
			blood_animation.Update(true);

	/* Same here */
	if (play_heal = heal_animation.IsRunning())
		heal_animation.Update(true);
}

void PZS::Player::DoBloodAnimation(void) noexcept
{
	blood_animation.Pause(false);
	play_blood = true;
}

void PZS::Player::Damage(int dmg) noexcept
{
	if (dmg > 0) {
		DoBloodAnimation();
		damage_sfx->Play();
	}
	else if (dmg < 0)
		DoHealAnimation();

	health -= dmg;

	/* Player may be healed. Cap health */
	if (health > 100)
		health = 100;

	if (health <= 0) {
		SDL_ShowCursor(1);
		alive = false;
		health = 0;
	}
}

void PZS::Player::Reset(void) noexcept
{
	health = 100;
	gun_index = GunIndex::PISTOL;

	mTexture = still_sheet[PISTOL];
	current_animation = &still_animation[PISTOL];

	/* Initialize hitboxes accoording to size (guns differ) */
	hitboxes[PISTOL] = { 300, 300, 64, 64 };
	hitboxes[SHOTGUN] = { 300, 300, 80, 64 };
	hitboxes[RIFLE] = { 300, 300, 80, 64 };

	/* Default hitbox is the pistol */
	hitbox = hitboxes[PISTOL];

	for (int i = 0; i < (int)GunIndex::GUN_COUNT; ++i) {
		guns[i]->Reset();
		available_guns[i] = false;
	}

	available_guns[PISTOL] = true;

	play_once = false;
	play_blood = false;
	play_heal = false;
	has_hit_zombie = false;
	has_reloaded = false;
	alive = true;
	health = 100;
}

bool PZS::Player::HandleCollisionWithZombie(SDL_Rect rect) noexcept
{
	return HandleCollisionWithBarrier(rect);
}

void PZS::Player::Render(void) noexcept
{
	guns[gun_index]->Render();

	static constexpr int d = 96;

	if (play_blood)
		blood_sheet->Render({ hitbox.x - d / 2, hitbox.y - d / 2, hitboxes[gun_index].w + d,
			hitboxes[gun_index].h + d }, blood_animation.GetCurrentFrame());

	if (alive)
		mTexture->Render(hitboxes[gun_index], current_animation->GetCurrentFrame(), rotation_angle);

	if (play_heal)
		heal_sheet->Render(hitbox, heal_animation.GetCurrentFrame());
}

void PZS::Player::DoShootingAnimation(void) noexcept
{
	if (current_animation != &shooting_animation[gun_index] && !play_once)
	{
		play_once = true;

		shooting_animation[gun_index].Pause(false);

		current_animation->Reset();
		current_animation = &shooting_animation[gun_index];
		mTexture = shoot_sheet[gun_index];
	}
}

void PZS::Player::DoReloadAnimation(void) noexcept
{
	if (guns[gun_index]->GetTotalAmmo() == 0)
		return;

	if (current_animation != &reload_animation[gun_index] && !play_once)
	{
		reload_sfx[gun_index]->Play();

		has_reloaded = false;
		play_once = true;

		reload_animation[gun_index].Pause(false);

		current_animation->Reset();
		current_animation = &reload_animation[gun_index];
		mTexture = reload_sheet[gun_index];
	}
}

void PZS::Player::DoMeeleeAnimation(void) noexcept
{
	if (current_animation != &meelee_animation[gun_index] && !play_once)
	{
		play_once = true;

		meelee_animation[gun_index].Pause(false);

		current_animation->Reset();
		current_animation = &meelee_animation[gun_index];
		mTexture = meelee_sheet[gun_index];
	}
}

void PZS::Player::DoHealAnimation(void) noexcept
{
	if (!heal_animation.IsRunning() && health < 100) 
	{
		play_heal = true;
		heal_animation.Pause(false);
	}
}