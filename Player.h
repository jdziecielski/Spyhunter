#pragma once
#include "Entity.h"
#include "Bullet.h"


class Player: public Entity {
private:
	Entity** entities;
	Bullet** bulletArray;
	SDL_Texture* texture;
	SDL_Texture* bullettex;
	double worldTime;
public:

	Player(int x, int y, Entity **&entities, Bullet **&bulletArray, SDL_Texture* &texture, SDL_Texture* &bullettex, int indexInEntityArray) {
		this->indexInEntityArray = indexInEntityArray;
		this->entities = new Entity * [ENTITIES_ARRAY_SIZE];
		this->entities = entities;
		this->bulletArray = new Bullet * [BULLET_ARRAY_SIZE];
		this->bulletArray = bulletArray;
		this->texture = texture;
		this->bullettex = bullettex;
		this->setPosition(x, y);
		this->worldTime = worldTime;
		this->health = PLAYER_HEALTH;
		this->entityType = PLAYER;
	}

	~Player() {
		for (int i = 0; i < ENTITIES_ARRAY_SIZE; i++) {
			if (this->entities[i] != nullptr)
				delete this->entities[i];
		}

		for (int i = 0; i < BULLET_ARRAY_SIZE; i++) {
			if (this->bulletArray[i] != nullptr)
				delete this->bulletArray[i];
		}
		delete[] this->bulletArray;
	}

	void detectDeath() {
		if (this->health <= 0) {
			entities[indexInEntityArray] = nullptr;

		}
	}

	void action()override {
		detectDeath();
		SDL_Event event;
		dirX = 0;
		dirY = 0;
		const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
		if (keyboardState[SDL_SCANCODE_DOWN]) dirY = 1;
		if (keyboardState[SDL_SCANCODE_UP]) dirY = -1;
		if (keyboardState[SDL_SCANCODE_LEFT]) dirX = -1;
		if (keyboardState[SDL_SCANCODE_RIGHT]) dirX = 1;
		if (keyboardState[SDL_SCANCODE_SPACE]) {
			int isBulletAppended = 0, i = 0;
			while (!isBulletAppended && i < BULLET_ARRAY_SIZE) {
				if (bulletArray[i] == nullptr) { 
					bulletArray[i] = new Bullet(this->position.x + BULLET_X, this->position.y + BULLET_Y, this->entities, this->bulletArray, this->bullettex, i);
					bulletArray[i]->setDeployedBy(PLAYER);
					isBulletAppended = 1;
				}
				i++;
			}
		}

		int *collisions = DetectCollision(this->entities, this->indexInEntityArray);
		if (collisions[UP] == 1) dirY = 0;
		if (collisions[DOWN] == 1) dirY = 0;
		if (collisions[LEFT] == 1) dirX = 0;
		if (collisions[RIGHT] == 1) dirX = 0;

		this->setPosition(this->position.x + dirX * MOVESPEEDPLAYER, this->position.y + dirY * MOVESPEEDPLAYER);
	}

	void draw(SDL_Renderer* renderer, int width, int height) override {
		if (this->dirY == 1 && this->dirX == -1)							//down left
			RenderTexture(renderer, this->texture, 345, this->position, CAR_WIDTH, CAR_HEIGHT);
		else if (this->dirY == 1 && this->dirX == 1)						//down right
			RenderTexture(renderer, this->texture, 15, this->position, CAR_WIDTH, CAR_HEIGHT);
		else if (this->dirY == -1 && this->dirX == -1)						//up left
			RenderTexture(renderer, this->texture, 345, this->position, CAR_WIDTH, CAR_HEIGHT);
		else if (this->dirY == -1 && this->dirX == 1)						//up right
			RenderTexture(renderer, this->texture, 15, this->position, CAR_WIDTH, CAR_HEIGHT);
		else if (this->dirX == 0 || this->dirX == -1 || this->dirX == 1)	//every other
			RenderTexture(renderer, this->texture, 0, this->position, CAR_WIDTH, CAR_HEIGHT);
	}

	void setPosition(int x, int y) override {
		if (x >= SCREEN_WIDTH / 2 - ROAD_WIDTH + CAR_WIDTH / 2 && x <= SCREEN_WIDTH / 2 + ROAD_WIDTH - CAR_WIDTH / 2)
			this->position.x = x;

		if (y >= CAR_HEIGHT / 2 && y <= SCREEN_HEIGHT - CAR_HEIGHT / 2)
			this->position.y = y;
	}


};