#pragma once

#include <memory>
#include <vector>

#include <SDL.h>

#include "Bullet.h"
#include "Drop.h"
#include "Barrier.h"
#include "Zombie.h"

using namespace std;

namespace PZS
{
	enum class GameState
	{
		GS_MENU,
		GS_PLAYING,
		GS_START,
		GS_LOAD_GAME_GRAPHICS
	};

	extern GameState g_game_state;

	extern vector<unique_ptr<Bullet>>  g_player_bullets;;
	extern vector<unique_ptr<Zombie>>  g_zombies;
	extern vector<unique_ptr<Drop>>    g_drops;
}