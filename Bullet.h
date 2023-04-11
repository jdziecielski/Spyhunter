#pragma once
#pragma once
#include "Entity.h"


class Bullet : public Entity {
private:
	int indexInBulletArray;
	Entity** entities;
	Bullet** bulletArray;
	SDL_Texture* texture;
	int ifShotHited = 0;
	int deployedBy;
public:
	Bullet(int x, int y, Entity**& entities, Bullet**& bulletsArray, SDL_Texture*& texture, int indexInEntityArray) {
		this->indexInBulletArray = indexInEntityArray;
		this->entities = entities;
		this->bulletArray = new Bullet * [BULLET_ARRAY_SIZE];
		this->bulletArray = bulletsArray;
		this->texture = texture;
		this->setPosition(x, y);
		this->dirY = -1;
		this->health = 1;
	}

	~Bullet() {
		this->bulletArray[indexInBulletArray] = nullptr;
	}

	void DetectHit() {
		int bulletX = this->position.x; 
		int bulletY = this->position.y;
		int carX1, carX2, carY1, carY2;
		int collisions[4] = { 0, 0, 0, 0 };
			for (int i = 0; i < ENTITIES_ARRAY_SIZE; i++) {
				if (entities[i] != nullptr && entities[i]->getEntityType() != this->deployedBy) {
					carX1 = this->entities[i]->getPosition().x - (CAR_WIDTH / 2);
					carX2 = this->entities[i]->getPosition().x + (CAR_WIDTH / 2);
					carY1 = this->entities[i]->getPosition().y - (CAR_HEIGHT / 2);
					carY2 = this->entities[i]->getPosition().y + (CAR_HEIGHT / 2);

					if (bulletY >= carY1 && bulletY <= carY2 && bulletX >= carX1 && bulletX <= carX2) {
						entities[i]->damage(GUN_DMG, this->deployedBy);
						this->ifShotHited = 1;
					}
				}
			}
	}

	void action() override {
		DetectHit();
		if (this->position.y <= -BULLET_SIZE || ifShotHited)
			bulletArray[indexInBulletArray] = nullptr;

		this->setPosition(this->position.x + this->dirX * MOVESPEEDBULLET, this->position.y + this->dirY * MOVESPEEDBULLET);

	}

	void draw(SDL_Renderer* renderer, int width, int height) {
		RenderTexture(renderer, this->texture, 0, this->position, width, height);
	}

	void setPosition(int x, int y) override {
		if (y >= -CAR_HEIGHT && y < SCREEN_HEIGHT) {
			this->position.x = x;
			this->position.y = y;
		}

	}

	void setDeployedBy(int entityType) { this->deployedBy = entityType; }
};