#pragma once
#include "Point.h"
#include <sstream>
#include <string>
#include <chrono>
#include <random>

extern "C" {
#include "SDL2-2.0.10/include/SDL.h"
#include "SDL2-2.0.10/include/SDL_main.h"
}

enum Directions {
	LEFT, UP, RIGHT, DOWN
};

enum Entities {
	PLAYER, ALLY, ENEMY
};

//window / fps / loading
#define SCREEN_FPS 60
#define SCREEN_TICKS_PER_FRAME 1000 / SCREEN_FPS
#define SCREEN_WIDTH	1200
#define SCREEN_HEIGHT	800
#define SAVES_PATH_COMMAND "dir /a-d C:\\Users\\jdzie\\source\\repos\\template-project-2\\saves"

//textures
#define ROAD_WIDTH 220
#define ROAD_HEIGHT 70
#define CAR_WIDTH 96
#define CAR_HEIGHT 128
#define SIDEWALK_WIDTH 32
#define SIDEWALK_HEIGHT 128
#define LAMP_WIDTH 32
#define LAMP_HEIGHT 96
#define BARREL_WIDTH 32
#define BARREL_HEIGHT 64
#define BULLET_SIZE 8


//game
#define PLAYER_X SCREEN_WIDTH/2
#define PLAYER_Y SCREEN_HEIGHT/2
#define MOVESPEEDPLAYER 3
#define MOVESPEEDENEMY 2
#define MOVESPEEDBULLET 10
#define MOVESPEEDMAP 0.25
#define POINTS_PER_SECOND 50
#define ENTITIES_ARRAY_SIZE 5
#define BULLET_ARRAY_SIZE 512
#define COLLISION_OFFSET 5
#define PLAYER_HEALTH 500
#define NPC_HEALTH 150
#define GUN_DMG 5
#define AWARD_FOR_KILLING_ENEMY 500

//drawing
#define INFO_X 10
#define INFO_Y 10
#define TEXT_OFFSET 15
#define IMPLEMENTATIONS_X SCREEN_WIDTH - 180
#define LOAD_X 30
#define LOAD_Y SCREEN_HEIGHT / 4 + 50
#define BULLET_X -40
#define BULLET_Y -48

enum Collisions { car, bullet };