#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#include "Player.h"
#include "Enemy.h"
#include "Ally.h"
#include <iostream>


// repair spawning range x

void DeallocateEntities(Entity**& entities) {
	for (int i = 0; i < ENTITIES_ARRAY_SIZE; i++) {
		if (entities[i] != nullptr) {
			delete entities[i];
			entities[i] = nullptr;
		}
	}
}


void DeallocateBulletArray(Bullet**& bulletArray) {
	for (int i = 0; i < BULLET_ARRAY_SIZE; i++) {
		if (bulletArray[i] != nullptr) {
			bulletArray[i] = nullptr;
		}
	}
}


void StartGame(Entity **&entities, Bullet**& bulletArray, SDL_Texture *&playercartex, SDL_Texture *&bullettex, double &worldTime, int &isGameStarted, int &gameSpeed, int &timer, int &score) {
	DeallocateEntities(entities);
	entities[0] = new Player(PLAYER_X, PLAYER_Y, entities, bulletArray, playercartex, bullettex, 0);
	worldTime = 0.0;
	isGameStarted = 1;
	gameSpeed = 1;
	timer = worldTime - 2;
	score = 0;
}


void PauseGame(int& isGamePaused, double& worldTime, double& timeOfPause) {
	if (isGamePaused) {
		isGamePaused = 0;
		worldTime = timeOfPause;
	}
	else {
		isGamePaused = 1;
		timeOfPause = worldTime;
	}
}


void SaveGame(double timeElapsed, int score, Entity** entities) {
	FILE* file;
	char text[64];
	auto time = std::chrono::system_clock::now();
	std::time_t time2 = std::chrono::system_clock::to_time_t(time);
	char* path = std::ctime(&time2);
	path[strlen(path) - 1] = '\0';
	RemoveSpaces(path);
	sprintf(text, "saves/%s.txt", path);
	file = fopen(text, "w");
	if (file != NULL) {
		sprintf(text, "%f\n", timeElapsed);
		fputs(text, file);
		sprintf(text, "%d\n", score);
		fputs(text, file);
		for (int i = 0; i < ENTITIES_ARRAY_SIZE; i++) {
			if (entities[i] != nullptr) {
				sprintf(text, "%d %d\n", entities[i]->getPosition().x, entities[i]->getPosition().y);
				fputs(text, file);
			}
		}
		fclose(file);
	}
}

void LoadGame(std::string filenameStr, double& worldTime, int& score, Entity** &entities, Bullet**& bulletArray, int &timerForScore, SDL_Texture* playerTex, SDL_Texture* enemyTex, SDL_Texture* &playerBulletTex, SDL_Texture* &enemyBulletTex) {
	char line[64], path[128], * linehelp;
	filenameStr.erase(filenameStr.length() - 1);
	const char *filenameCharArray = filenameStr.c_str();
	sprintf(path, "saves/%s", filenameCharArray);
	FILE* file = fopen(path, "r");
	int lineNum = 0, i = 0, j = 1, x, y;
	if (file != NULL) {
		DeallocateEntities(entities);
		while (fgets(line, 32, file) != NULL) {
			if (lineNum == 0) worldTime = atoi(line);
			else if (lineNum == 1) score = atoi(line);
			else if (lineNum == 2) {
				i = 0;
				linehelp = strtok(line, " ");
				while (linehelp != NULL) {
					if (i == 0) {
						x = atoi(linehelp);
					}
					else if (i == 1) {
						y = atoi(linehelp);
						entities[0] = new Player(x, y, entities, bulletArray, playerTex, playerBulletTex, 0);
					}

					linehelp = strtok(NULL, " ");
					i++;
				}
			}
			else {
				i = 0;
				linehelp = strtok(line, " ");
				while (linehelp != NULL) {
					if (i == 0) {
						x = atoi(linehelp);
					}
					else if (i == 1) {
						y = atoi(linehelp);
						entities[j] = new Enemy(x, y, entities, bulletArray, enemyTex, enemyBulletTex, j, score);
						j++;
					}

					linehelp = strtok(NULL, " ");
					i++;
				}
			}
			lineNum++;
		}
		fclose(file);
		timerForScore = worldTime - 1;
	}
}

void DrawEntities(Entity** entities, SDL_Renderer* renderer) {
	for (int i = 0; i < ENTITIES_ARRAY_SIZE; i++) {
		if (entities[i] != nullptr)
			entities[i]->draw(renderer, CAR_WIDTH, CAR_HEIGHT);
	}
}

void DrawBullets(Bullet** bulletArray, SDL_Renderer* renderer) {
	int i = 0;
	while (i < BULLET_ARRAY_SIZE) {
		if (bulletArray[i] != nullptr) {
			bulletArray[i]->draw(renderer, BULLET_SIZE, BULLET_SIZE);
		}
		i++;
	}
}


//adds npc to a game, spawns beyond the map, ally if allyOrEnemy = 1; enemy if allyOrEnemy = 0
void AddNPC(Entity**& entities, Bullet**& bulletArray, SDL_Texture*& enemyTex, SDL_Texture*& allyTex, SDL_Texture *&enemyBulletTex, int allyOrEnemy, int &score, int &ifHaltScore) {
	int randX = ((std::rand() % ROAD_WIDTH * 2 - 50) + SCREEN_WIDTH / 2 - ROAD_WIDTH);
	int randY = (std::rand() & 2);
	if (randY == 0) randY = -CAR_HEIGHT;
	else randY = SCREEN_HEIGHT + CAR_HEIGHT;

	int isNPCPlaced = 0, i = 1;
	while (i < ENTITIES_ARRAY_SIZE && !isNPCPlaced) {
		if (entities[i] == nullptr) {
			if (allyOrEnemy == 0)
				entities[i] = new Enemy(randX, randY, entities, bulletArray, enemyTex, enemyBulletTex, i, score);
			else
				entities[i] = new Ally(randX, randY, entities, allyTex, i, ifHaltScore);

			isNPCPlaced = 1;
		}
		i++;
	}
}


#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char** argv) {
	int t1, t2, quit, frames, gameSpeed = 2, score = 0, timer, frameTime, numOfEntities = 0, timerForEnemies = 0, timerForHaltScore = 0;
	int isGamePaused = 0, isGameStarted = 0, isLoadingMenu = 0, cursorPos = 2, numOfFiles, ifSecondPassed = 0, ifHaltScore = 0;
	double delta, worldTime = 0.0, fpsTimer, fps, distance, timeOfPause = 0.0;
	std::string fileToLoadFrom, * arrayOfFiles = nullptr;
	Uint32 frameStart;
	SDL_Window* window;
	SDL_Renderer* renderer;
	Entity** entities = new Entity*[ENTITIES_ARRAY_SIZE];
	for (int i = 0; i < ENTITIES_ARRAY_SIZE; i++) entities[i] = nullptr;
	Bullet** bulletArray = new Bullet * [512];
	for (int i = 0; i < BULLET_ARRAY_SIZE; i ++) bulletArray[i] = nullptr;

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		SDL_Quit();
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
	}

	if (SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer) != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
	};

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetWindowTitle(window, "Spyhunter");
	SDL_ShowCursor(SDL_DISABLE);	// wy³¹czenie widocznoœci kursora myszy


	//initializing textures
	SDL_Surface* screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	SDL_Texture* scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_Texture* charsettex = LoadCharset("./textures/cs8x8.bmp", renderer);
	SDL_Texture* roadlefttex = LoadTexture("./textures/street-left.bmp", renderer);
	SDL_Texture* roadrighttex = LoadTexture("./textures/street-right.bmp", renderer);
	SDL_Texture* sidewalktex = LoadTexture("./textures/terrain textures/Sidewalk_dark.bmp", renderer);
	SDL_Texture* terrainobjectstex = LoadTexture("./textures/terrain textures/objects.bmp", renderer);
	SDL_Texture* enemycartex = LoadTexture("./textures/carenemyarmed.bmp", renderer);
	SDL_Texture* playercartex = LoadTexture("./textures/carplayerarmed.bmp", renderer);
	SDL_Texture* allycartex = LoadTexture("./textures/carally.bmp", renderer);
	SDL_Texture* bulletplayer = LoadTexture("./textures/shotplayer.bmp", renderer);
	SDL_Texture* bulletenemy = LoadTexture("./textures/shotenemy.bmp", renderer);

	char text[128];		//for sprintf use
	int black = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	t1 = SDL_GetTicks();
	frames = 0;
	fpsTimer = 0;
	fps = 0;
	quit = 0;
	worldTime = 0;
	distance = 0;
	SDL_Event event;

	while (!quit) {
		frameStart = SDL_GetTicks();
		t2 = SDL_GetTicks();
		delta = (t2 - t1) * 0.001;
		t1 = t2;
		worldTime += delta;
		fpsTimer += delta;
		if (fpsTimer > 0.5) {
			fps = frames * 2;
			frames = 0;
			fpsTimer -= 0.5;
		};

		//handling game events
		if (isGameStarted && !isGamePaused && !isLoadingMenu) {
			//increment score every one second
			if (ifHaltScore) {
				timerForHaltScore = 5;
				ifHaltScore = 0;
			}

			if ((int)worldTime - timer == 2) {
				timer = worldTime - 1;


				if (timerForHaltScore <= 0)
					score += POINTS_PER_SECOND;

				timerForEnemies++;
				for (int i = 1; i < ENTITIES_ARRAY_SIZE; i++) {
					if (entities[i] != nullptr)
						entities[i]->changeDirection();
				}

				if (timerForEnemies == 5) {
					int allyOrEnemy, ifAddNPC = std::rand() % 2;
					if (ifAddNPC) {
						allyOrEnemy = std::rand() % 2;
						AddNPC(entities, bulletArray, enemycartex, allycartex, bulletenemy, allyOrEnemy, score, ifHaltScore);
					}
					timerForEnemies = 0;
				}

				if (timerForHaltScore >= 5)
					timerForHaltScore--;
			}

			for (int i = 0; i < ENTITIES_ARRAY_SIZE; i++) {
				if (entities[i] != nullptr)
					entities[i]->action();
			}
			
			for (int i = 0; i < BULLET_ARRAY_SIZE; i++) {
				if (bulletArray[i] != nullptr)
					bulletArray[i]->action();
			}
		}

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
				else if (event.key.keysym.sym == SDLK_n) StartGame(entities, bulletArray, playercartex, bulletplayer, worldTime, isGameStarted, gameSpeed, timer, score);
				else if (event.key.keysym.sym == SDLK_p) PauseGame(isGamePaused, worldTime, timeOfPause);
				else if (event.key.keysym.sym == SDLK_s) SaveGame(worldTime, score, entities);
				else if (event.key.keysym.sym == SDLK_l) {	//setting up loading from file
					if (isLoadingMenu) {		
						if (numOfFiles != -2) delete[] arrayOfFiles;
						worldTime = timeOfPause;
						isLoadingMenu = 0;
					}
					else {
						arrayOfFiles = GetNameFiles(numOfFiles);
						timeOfPause = worldTime;
						cursorPos = 2;
						isLoadingMenu = 1;
					}
				}
				else if (event.key.keysym.sym == SDLK_DOWN) {
					if (isLoadingMenu && cursorPos < numOfFiles * 2) 
						cursorPos += 2;
				}
				else if (event.key.keysym.sym == SDLK_UP) {
					if (isLoadingMenu && cursorPos > 2) 
						cursorPos -= 2;
				}
				else if (event.key.keysym.sym == SDLK_RETURN) {		//loading the game after pressing enter
					if (isLoadingMenu) {
						isLoadingMenu = 0;
						worldTime = timeOfPause;
						LoadGame(arrayOfFiles[cursorPos / 2 - 1], worldTime, score, entities, bulletArray, timer, playercartex, enemycartex, bulletplayer, bulletenemy);
						delete[] arrayOfFiles;
					}
				}
				break;
			case SDL_QUIT:
				quit = 1;
				break;
			};
		};


		//drawing
		SDL_RenderClear(renderer);
		SDL_FillRect(screen, NULL, black);
		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		
		DrawTerrain(renderer, sidewalktex, terrainobjectstex, worldTime, gameSpeed, timeOfPause, isGamePaused, isLoadingMenu);
		DrawRoad(renderer, roadlefttex, roadrighttex, worldTime, gameSpeed, isGamePaused, isLoadingMenu);
		DrawEntities(entities, renderer);
		DrawBullets(bulletArray, renderer);
		DrawInfo(renderer, charsettex, worldTime, fps, text, score, isGameStarted, isGamePaused, isLoadingMenu, timeOfPause, entities[0]->getHP());
		DrawMenu(renderer, charsettex, isGameStarted, isGamePaused, isLoadingMenu, cursorPos, arrayOfFiles, numOfFiles);

		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
		SDL_RenderPresent(renderer);
		
		frames++;
		frameTime = SDL_GetTicks() - frameStart;
		if (SCREEN_TICKS_PER_FRAME > frameTime)
			SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTime);
	};

	//zwolnienie powierzchni / freeing all surfaces
	SDL_DestroyTexture(roadlefttex);
	SDL_DestroyTexture(roadrighttex);
	SDL_DestroyTexture(sidewalktex);
	SDL_DestroyTexture(terrainobjectstex);
	SDL_DestroyTexture(charsettex);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyTexture(enemycartex);
	SDL_DestroyTexture(playercartex);
	SDL_DestroyTexture(allycartex);
	SDL_DestroyTexture(bulletplayer);
	SDL_DestroyTexture(bulletenemy);
	SDL_FreeSurface(screen);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	
	DeallocateEntities(entities);
	DeallocateBulletArray(bulletArray);
	delete[] bulletArray;
	delete[] entities;
	return 0;
};