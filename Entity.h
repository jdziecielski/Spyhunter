#pragma once
#include "Functionalities.h"

class Entity {
protected:
	int dirX = 0, dirY = 0, health;
	Point position;
	SDL_Texture* texture;
	int indexInEntityArray;
	int entityType;
	int lastHitedBy;
	
	virtual void setPosition(int x, int y) {
		position.x = x;
		position.y = y;
	}

	int* DetectCollision(Entity** entities, int indexOfThisEntity) {
		int x1, x2, y1, y2;
		int collisions[4] = { 0, 0, 0, 0 };
		Entity* collider = entities[indexOfThisEntity];
		if (entities != nullptr && entities[indexOfThisEntity] != nullptr) {
			int colliderX1 = collider->getPosition().x - (CAR_WIDTH / 2);
			int colliderX2 = collider->getPosition().x + (CAR_WIDTH / 2);
			int colliderY1 = collider->getPosition().y - (CAR_HEIGHT / 2);
			int colliderY2 = collider->getPosition().y + (CAR_HEIGHT / 2);
			for (int i = 0; i < ENTITIES_ARRAY_SIZE; i++) {
				if (i != indexOfThisEntity && entities[i] != nullptr) {
					x1 = entities[i]->getPosition().x - (CAR_WIDTH / 2);
					x2 = entities[i]->getPosition().x + (CAR_WIDTH / 2);
					y1 = entities[i]->getPosition().y - (CAR_HEIGHT / 2);
					y2 = entities[i]->getPosition().y + (CAR_HEIGHT / 2);

					//collisions up/down
					if (colliderX1 + COLLISION_OFFSET <= x2 && colliderX2 >= x1 + COLLISION_OFFSET) {		//if on adequate width to collide
						if (colliderY1 <= y2 && colliderY2 >= y1 + COLLISION_OFFSET && dirY == -1) collisions[UP] = 1;
						
						if (colliderY1 + COLLISION_OFFSET <= y2 && colliderY2 >= y1 && dirY == 1) collisions[DOWN] = 1;
					}

					if (colliderY1 + COLLISION_OFFSET <= y2 && colliderY2 >= y1 + COLLISION_OFFSET) {
						if (colliderX1 <= x2 && colliderX2 >= x1 + COLLISION_OFFSET && dirX == -1) collisions[LEFT] = 1;

						if (colliderX1 + COLLISION_OFFSET <= x2 && colliderX2 >= x1 && dirX == 1) collisions[RIGHT] = 1;
					}
				}
			}
		}
		return collisions;
	}

public:	
	Point getPosition() {
		return this->position; 
	}

	void setX(int x) {
		this->position.x = x;
	}

	void setY(int y) {
		this->position.y = y;
	}

	void setDir(int dirX, int dirY) {
		this->dirX = dirX;
		this->dirY = dirY;
	}

	void changeDirection() {
		this->dirX = GetRandomDir();
		this->dirY = GetRandomDir();
	}

	virtual void action() = 0;

	virtual void draw(SDL_Renderer* renderer, int width, int height) {
		RenderTexture(renderer, this->texture, 0, this->position, width, height);
	}

	SDL_Texture* getTexture() {
		return this->texture;
	}

	void damage(int hp, int hitedBy) {
		this->health = this->health - hp; 
		this->lastHitedBy = hitedBy;
	}

	int getHP() {
		if (this == nullptr)
			return 0;
		else
			return this->health; 
	}

	int getEntityType() {
		if (this != nullptr)
			return this->entityType;
	}
};