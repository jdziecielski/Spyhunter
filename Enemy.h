#pragma once
#include "Entity.h"


class Enemy : public Entity {
private:
	Entity** entities;
	Bullet** bulletArray;
	SDL_Texture* texture;
	SDL_Texture* bullettex;
	int score;

public:
	Enemy(int x, int y, Entity**& entities, Bullet**& bulletArray, SDL_Texture*& texture, SDL_Texture*& bullettex, int indexInEntityArray, int& score) {
		this->indexInEntityArray = indexInEntityArray;
		this->entities = new Entity *[ENTITIES_ARRAY_SIZE];
		this->entities = entities;
		this->bulletArray = bulletArray;
		this->texture = texture;
		this->bullettex = bullettex;
		this->setPosition(x, y);
		this->health = NPC_HEALTH;
		this->entityType = ENEMY;
		this->score = score;
	}

	~Enemy() {
		for (int i = 0; i < ENTITIES_ARRAY_SIZE; i++) {
			if (this->entities[i] != nullptr)
				delete this->entities[i];
		}
		delete[] this->entities;
	}

	void detectDeath() {
		if (this->health <= 0) {
      		if (lastHitedBy == PLAYER) {
				printf("%d ", score);
				this->score += AWARD_FOR_KILLING_ENEMY;
				printf("%d\n", score);
			}

			entities[indexInEntityArray] = nullptr;
		}
	}
	
	void action() override {
		detectDeath();
		int ifShoot = std::rand() % 10;
		if (ifShoot == 1) {
			int isBulletAppended = 0, i = 0;
			while (!isBulletAppended && i < BULLET_ARRAY_SIZE) {
				if (bulletArray[i] == nullptr) {
					bulletArray[i] = new Bullet(this->position.x + BULLET_X, this->position.y + BULLET_Y, this->entities, this->bulletArray, this->bullettex, i);
					bulletArray[i]->setDeployedBy(ENEMY);
					isBulletAppended = 1;
				}
				i++;
			}
		}
		int* collisions = DetectCollision(this->entities, this->indexInEntityArray);
		if (collisions[UP] == 1) dirY = 0;
		if (collisions[DOWN] == 1) dirY = 0;
		if (collisions[LEFT] == 1) dirX = 0;
		if (collisions[RIGHT] == 1) dirX = 0;
		this->setPosition(this->position.x + this->dirX * MOVESPEEDENEMY, this->position.y + this->dirY * MOVESPEEDENEMY);
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
		//if enemy spawned off the screen
		if (this->position.y - CAR_HEIGHT / 2 <= 0) {
			this->dirX = 0;
			this->dirY = 1;
			this->position.x = x;
			this->position.y = y;
		}
		else if (this->position.y + CAR_HEIGHT / 2 >= SCREEN_HEIGHT) {
			this->dirX = 0;
			this->dirY = -1;
			this->position.x = x;
			this->position.y = y;
		}
		else {
			if (x > SCREEN_WIDTH / 2 - ROAD_WIDTH + CAR_WIDTH / 2 && x < SCREEN_WIDTH / 2 + ROAD_WIDTH - CAR_WIDTH / 2)
				this->position.x = x;

			if (y > 0 + CAR_HEIGHT / 2 && y < SCREEN_HEIGHT - CAR_HEIGHT / 2)
				this->position.y = y;
		}	
	}
};