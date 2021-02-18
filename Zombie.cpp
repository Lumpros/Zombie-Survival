#include "Zombie.h"
#include "Resources.h"
#include "Globals.h"
#include "Player.h"
#include "RandomNumberGenerator.h"
#include "Utility.h"
#include "Powerup.h"
#include "GunDrops.h"

#define DIFF 38
#define HALF (DIFF / 2)

#define ATTACK_FRAME 5
#define BLOOD_FRAME 12

vector<unique_ptr<PZS::Zombie>> PZS::g_zombies;
vector<unique_ptr<PZS::Drop>> PZS::g_drops;

int PZS::GetAliveZombieCount(void) noexcept
{
	int count = 0;
	for (size_t i = 0; i < g_zombies.size(); ++i)
		if (g_zombies[i]->GetHP() > 0)
			++count;
	return count;
}

template <typename T> void add_drop(SDL_Rect pos) {
	PZS::g_drops.push_back(make_unique<T>(pos));
}

PZS::Zombie::Zombie(SDL_Rect rect)
{
	hitbox = { rect.x + HALF, rect.y + HALF, rect.w - DIFF, rect.h - DIFF };

	Resources* resources = Resources::GetInstance();

	damage_sfx = resources->gSFX.Get("blood_splatter");

	InitializeWalkGraphics(resources);
	InitializeAttackGraphics(resources);
	InitializeBloodGraphics(resources);

	current_animation = &walking_animation;
}

void PZS::Zombie::InitializeAttackGraphics(Resources* resources) noexcept
{
	attack_texture = resources->gTextures.Get("zombie_attack_sheet");

	for (int y = 0; y < 3; ++y)
		for (int x = 0; x < 3; ++x)
			attack_animation.AddFrame({ { x * 318 + HALF, y * 294 + HALF, 318 - DIFF, 294 - DIFF }, 100 });
}

void PZS::Zombie::InitializeWalkGraphics(Resources* resources) noexcept
{
	walk_texture = resources->gTextures.Get("zombie_walking_sheet");

	mTexture = walk_texture;

	for (int i = 0; i < 2; ++i)
		for (int j = 0; j < 7; ++j)
			walking_animation.AddFrame({ { j * 288 + HALF, i * 311 + HALF, 288 - DIFF, 311 - DIFF }, 50 });

	walking_animation.AddFrame({ { 0 + HALF, 622 + HALF, 288 - DIFF, 311 - DIFF }, 50 });
	walking_animation.AddFrame({ { 288 + HALF, 622 + HALF, 288 - DIFF, 311 - DIFF }, 50 });
	walking_animation.AddFrame({ { 288 * 2 + HALF, 622 + HALF, 288 - DIFF, 311 - DIFF }, 50 });
}

void PZS::Zombie::InitializeBloodGraphics(Resources* resources) noexcept
{
	blood_sheet = resources->gTextures.Get("blood_splatter");

	blood_animation.AddFrame({ { 215 * 2, 0, 215, 222 }, 25 });
	blood_animation.AddFrame({ { 215 * 3, 0, 215, 222 }, 25 });

	for (int i = 1; i < 4; ++i)
		for (int j = 0; j < 4; ++j) // 215 222
			blood_animation.AddFrame({ { j * 215, i * 222, 215, 222 }, 25 });
}

void PZS::Zombie::DoBloodAnimation(void) noexcept
{
	play_blood = true;
	blood_animation.Pause(false);
	blood_animation.Reset();
}

void PZS::Zombie::DoAttackAnimation(void) noexcept
{
	if (current_animation == &attack_animation)
		return;

	hasnt_hit_player = true;
	 
	current_animation->Reset();
	current_animation = &attack_animation;

	current_animation->Reset();
	current_animation->Pause(false);

	mTexture = attack_texture;

	play_once = true;
}

#define ZOMBIE_SPEED 2

void PZS::Zombie::HandleMovement(void) noexcept
{
	SDL_Rect player_rectangle = Player::Get()->hitbox;

	Vector2D zombie_middle = { hitbox.x + hitbox.w / 2, hitbox.y + hitbox.h / 2 };
	Vector2D player_middle = { player_rectangle.x + player_rectangle.w / 2, player_rectangle.y + player_rectangle.h / 2 };

	/* Calculate zombie velocity to follow player*/
	std::pair<double, double> vel = Gun::CalculateObjectVelocity(zombie_middle, player_middle, ZOMBIE_SPEED);

	hitbox.x += static_cast<int>(round(vel.first));
	hitbox.y += static_cast<int>(round(vel.second));
	
	/* Don't let zombie intersect player */
	HandleCollisionWithBarrier(player_rectangle);
}

void PZS::Zombie::ChooseGunDrop(void) noexcept
{
	GunIndex gunIndex = static_cast<GunIndex>(RNG::RandInt(PISTOL + 1, GUN_COUNT - 1));

	switch (gunIndex)
	{
	case SHOTGUN:
		if (!Player::Get()->WeaponIsActive(SHOTGUN))
			add_drop<ShotgunDrop>(hitbox);
		break;

	case RIFLE:
		if (!Player::Get()->WeaponIsActive(RIFLE))
			add_drop<RifleDrop>(hitbox);
		break;
	}
}

enum class Powerup {
	BEGIN, // for rng
	MEDKIT, PISTOL_AMMO,
	RIFLE_AMMO, SHOTGUN_AMMO,
	NUKE,
	TOTAL_COUNT // for rng
};

void PZS::Zombie::ChoosePowerup(void) noexcept
{
	Powerup choice = (Powerup)RNG::RandInt((int)Powerup::BEGIN + 1, (int)Powerup::TOTAL_COUNT - 1);

	switch (choice) {
		case Powerup::MEDKIT:
			add_drop<Medkit>(hitbox);
			break;

		case Powerup::PISTOL_AMMO:
			add_drop<PistolAmmo>(hitbox);
			break;

		case Powerup::RIFLE_AMMO:
			add_drop<RifleAmmo>(hitbox);
			break;

		case Powerup::SHOTGUN_AMMO:
			add_drop<ShotgunAmmo>(hitbox);
			break;

		case Powerup::NUKE:
			add_drop<Nuke>(hitbox);
			break;
	}
}

void PZS::Zombie::Damage(int dmg) noexcept
{
	if (dmg > 0 && hp > 0) {
		DoBloodAnimation();
		damage_sfx->Play();
	}

	hp -= dmg;

	// try Drop gun
	if (hp <= 0)
	{
		/* 1 in 20 the first time */
		/* 1 in 40 the second time */
		if (RNG::RandInt(1, 20) == RNG::RandInt(1, 20))
			ChooseGunDrop();

		else if (RNG::RandInt(1, 5) == RNG::RandInt(1, 5)) // 20%
			ChoosePowerup();
	}
}

bool PZS::Zombie::HandleCollisionWithZombie(SDL_Rect rect) noexcept
{
	if (rect != hitbox)
		return HandleCollisionWithBarrier(rect);
	return false;
}

int i_square(int x)
{
	return x * x;
}

void PZS::Zombie::HandleAnimation(void) noexcept
{
	/* If the attacking animation has finished */
	if (!can_move && !current_animation->IsRunning())
	{
		current_animation = &walking_animation;
		mTexture = walk_texture;
	}

	/* Check if zombie has hit the player */
	/* If so, damage him */
	if (current_animation == &attack_animation && current_animation->GetFrameIndex() == ATTACK_FRAME && hp > 0)
	{
		Player* player = Player::Get();

		if (hasnt_hit_player && sqrt(
			i_square((player->hitbox.x + player->hitbox.w / 2) - (hitbox.x + hitbox.w / 2)) + 
			i_square((player->hitbox.y + player->hitbox.h / 2) - (hitbox.y + hitbox.h / 2))) < 64)
		{	
			Player::Get()->Damage(RNG::RandInt(damage - 3, damage + 3));
			hasnt_hit_player = false;
		}
	}

	can_move = (current_animation != &attack_animation);

	if (can_move && play_once)
		play_once = false;

	current_animation->Update(play_once);

	if (play_blood)
	{
		if (blood_animation.GetFrameIndex() == BLOOD_FRAME)
		{
			play_blood = false;
			if (hp <= 0)
				is_dead = true;
		}
		else
			blood_animation.Update(true);
	}

	if (!play_blood && hp <= 0)
		DoBloodAnimation();
}

void PZS::Zombie::Update(void) noexcept
{
	HandleAnimation();

	if (hp > 0)
	{
		if (can_move)
			HandleMovement();

		for (size_t i = 0; i < g_zombies.size(); ++i)
		{
			if (g_zombies[i]->GetHP() > 0)
				HandleCollisionWithZombie(g_zombies[i]->hitbox);
		}
	}
}

void PZS::Zombie::Render(void) noexcept
{
	SDL_Rect player_rectangle = Player::Get()->hitbox;

	double angle = CalculateRotationAngle({ 
		player_rectangle.x + player_rectangle.w / 2,
		player_rectangle.y + player_rectangle.h / 2 }, { 
			hitbox.x + hitbox.w / 2, hitbox.y + hitbox.h / 2 
		});

	static constexpr int d = 128;

	if (play_blood)
		blood_sheet->Render({ hitbox.x - d / 2, hitbox.y - d / 2, hitbox.w + d, hitbox.h + d }, blood_animation.GetCurrentFrame());

	if (hp > 0)
		mTexture->Render({hitbox.x - DIFF / 2, hitbox.y - DIFF / 2, hitbox.w + DIFF, hitbox.h + DIFF}, current_animation->GetCurrentFrame(), angle);
}