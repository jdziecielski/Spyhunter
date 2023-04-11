#pragma once
#include "Constants.h"



int GetRandomDir() {
	int randInt = (std::rand() % 3);

	if (randInt == 2)
		randInt = -1;

	return randInt;
}


void RemoveSpaces(char* s) {
	char* d = s;
	do {
		while (*d == ' ' || *d == ':') ++d;
	} while (*s++ = *d++);
}


std::string* GetNameFiles(int &numOfFiles) {
	//get names of files by using command prompt
	char buffer[128];
	int ifSave = 0;
	numOfFiles = -2;
	std::string result = "", bufferStr, startSavingCondition = "saves", endSavingCondition = "bytes";
	FILE* file = _popen(SAVES_PATH_COMMAND, "r");
	if (!file) throw std::runtime_error("_popen() failed!");
	try {
		while (fgets(buffer, sizeof buffer, file) != NULL) {
			bufferStr = buffer;

			if (bufferStr.find(startSavingCondition) != std::string::npos)
				ifSave = 1;

			if (bufferStr.find(endSavingCondition) != std::string::npos)
				ifSave = 0;

			if (ifSave) {
				numOfFiles++;
				result += buffer;
			}
		}
	}
	catch (...) {
		_pclose(file);
		throw;
	}
	_pclose(file);

	//edit to get only names of files
	result = result.erase(0, 68);
	std::istringstream fromStr(result);
	std::string line, toReturn, * toReturnArray = new std::string[numOfFiles];
	int length, i = 0;
	while (std::getline(fromStr, line)) {
		line.erase(0, 36);
		line += '\n';
		toReturn += line;
		toReturnArray[i] = line;
		i++;
	}

	return toReturnArray;
}


// draw a text txt on surface screen, starting from the point (x, y)
// charset is a 128x128 bitmap containing character images
void DrawString(SDL_Renderer* renderer, int x, int y, const char* text, SDL_Texture* charsettex) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while (*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_RenderCopy(renderer, charsettex, &s, &d);
		x += 8;
		text++;
	}
}


void RenderTexture(SDL_Renderer* renderer, SDL_Texture* tex, double angle, Point dstxy, int w, int h) {
	SDL_Rect dest;
	dest.x = dstxy.x - w / 2;
	dest.y = dstxy.y - h / 2;
	dest.w = w;
	dest.h = h;
	SDL_RenderCopyEx(renderer, tex, NULL, &dest, angle, NULL, SDL_FLIP_NONE);
}


void RenderTextureWithSrc(SDL_Renderer* renderer, SDL_Texture* tex, double angle, Point srcxy, Point dstxy, int w, int h) {
	SDL_Rect src, dest;
	src.x = srcxy.x;
	src.y = srcxy.y;
	src.w = w;
	src.h = h;
	dest.x = dstxy.x;
	dest.y = dstxy.y;
	dest.w = w;
	dest.h = h;

	SDL_RenderCopyEx(renderer, tex, &src, &dest, angle, NULL, SDL_FLIP_NONE);
}


bool LoadBitMap(const char filepath[], SDL_Surface** bitmap) {
	bool success = true;
	*bitmap = SDL_LoadBMP(filepath);
	if (bitmap == NULL) {
		printf("Unable to load image %s! SDL Error: %s\n", filepath, SDL_GetError());
		success = false;
	}
	return success;
}


SDL_Texture* LoadTexture(char* file, SDL_Renderer* renderer) {
	SDL_Surface* surface = SDL_LoadBMP(file);
	if (surface == NULL) {
		printf("SDL_LoadBMP error: %s\n", SDL_GetError());
		SDL_Quit();
		return NULL;
	}
	return SDL_CreateTextureFromSurface(renderer, surface);
}


SDL_Texture* LoadCharset(char* file, SDL_Renderer* renderer) {
	SDL_Surface* charset = SDL_LoadBMP(file);
	SDL_SetColorKey(charset, true, 0x000000);
	if (charset == NULL) {
		printf("SDL_LoadBMP error: %s\n", SDL_GetError());
		SDL_Quit();
		return NULL;
	}
	return SDL_CreateTextureFromSurface(renderer, charset);
}


void DrawRoad(SDL_Renderer* renderer, SDL_Texture* roadlefttex, SDL_Texture* roadrighttex, double time, int gameSpeed, int isGamePaused, int isLoadingMenu) {
	double drawingSpeed = -time * MOVESPEEDMAP;

	if (isGamePaused || isLoadingMenu)
		drawingSpeed = -MOVESPEEDMAP;
	else
		drawingSpeed = -time * MOVESPEEDMAP;

	int startDrawing = SCREEN_HEIGHT * drawingSpeed;
	int endDrawing = SCREEN_HEIGHT + ROAD_HEIGHT * 2;

	for (int y = startDrawing; y < endDrawing; y += ROAD_HEIGHT) {
		Point pointRoadLeft(SCREEN_WIDTH / 2 - ROAD_WIDTH, (-y + SCREEN_HEIGHT));
		Point pointRoadRight(SCREEN_WIDTH / 2, (-y + SCREEN_HEIGHT));
		RenderTextureWithSrc(renderer, roadlefttex, 0, Point(0, 0), pointRoadLeft, ROAD_WIDTH, ROAD_HEIGHT);
		RenderTextureWithSrc(renderer, roadrighttex, 0, Point(0, 0), pointRoadRight, ROAD_WIDTH, ROAD_HEIGHT);
	}
}


void DrawTerrain(SDL_Renderer* renderer, SDL_Texture* sidewalktex, SDL_Texture* objectstex, double time, int gameSpeed, int timeOfPause, int isGamePaused, int isLoadingMenu) {
	double drawingSpeed = -time * MOVESPEEDMAP;

	if (isGamePaused || isLoadingMenu)
		drawingSpeed = -MOVESPEEDMAP;
	else
		drawingSpeed = -time * MOVESPEEDMAP;

	int startY = SCREEN_HEIGHT * drawingSpeed;
	int endY = SCREEN_HEIGHT + SIDEWALK_HEIGHT;

	for (int y = startY; y < endY; y += SIDEWALK_HEIGHT) {
		for (int x = 0; x < SCREEN_WIDTH; x += SIDEWALK_WIDTH) {
			Point pointToRender(x, (-y + SCREEN_HEIGHT));
			RenderTextureWithSrc(renderer, sidewalktex, 0, Point(96, 0), pointToRender, SIDEWALK_WIDTH, SIDEWALK_HEIGHT);

			if ((x >= SCREEN_WIDTH / 4 && x <= SCREEN_WIDTH / 4 + LAMP_WIDTH) || (x >= SCREEN_WIDTH / 2 + ROAD_WIDTH + LAMP_WIDTH && x <= SCREEN_WIDTH / 2 + ROAD_WIDTH + 48))
				RenderTextureWithSrc(renderer, objectstex, 0, Point(0, 0), pointToRender, LAMP_WIDTH, LAMP_HEIGHT);

			if (x < SCREEN_WIDTH / 4 || x > SCREEN_WIDTH / 2 + ROAD_WIDTH + 48)
				RenderTextureWithSrc(renderer, objectstex, 0, Point(160, 0), pointToRender, BARREL_WIDTH, BARREL_HEIGHT);
		}
	}
}


void DrawInfo(SDL_Renderer* renderer, SDL_Texture* charset, double worldTime, double fps, char* text, int score, int isGameStarted, int isGamePaused, int isLoadingMenu, double timeOfPause, int playerHP) {
	sprintf(text, "Jan Dziecielski s191702");
	DrawString(renderer, INFO_X, INFO_Y, text, charset);

	if (isGameStarted && !(isGamePaused || isLoadingMenu))
		sprintf(text, "Game Time: %.1lf s", worldTime);
	else if (isGamePaused || isLoadingMenu)
		sprintf(text, "Game Time: %.1lf s", timeOfPause);
	else
		sprintf(text, "Game Time: 0.0 s");

	DrawString(renderer, INFO_X, INFO_Y + TEXT_OFFSET, text, charset);

	sprintf(text, "%.0lf frames / sec", fps);
	DrawString(renderer, INFO_X, INFO_Y + TEXT_OFFSET * 2, text, charset);

	//controls
	sprintf(text, "n - new game");
	DrawString(renderer, INFO_X, INFO_Y + TEXT_OFFSET * 4, text, charset);

	sprintf(text, "arrows - movement");
	DrawString(renderer, INFO_X, INFO_Y + TEXT_OFFSET * 5, text, charset);

	sprintf(text, "space - shooting");
	DrawString(renderer, INFO_X, INFO_Y + TEXT_OFFSET * 6, text, charset);

	sprintf(text, "s - save the game");
	DrawString(renderer, INFO_X, INFO_Y + TEXT_OFFSET * 7, text, charset);

	sprintf(text, "l - load the game");
	DrawString(renderer, INFO_X, INFO_Y + TEXT_OFFSET * 8, text, charset);

	sprintf(text, "p - pause the game");
	DrawString(renderer, INFO_X, INFO_Y + TEXT_OFFSET * 9, text, charset);

	sprintf(text, "f - finish the game");
	DrawString(renderer, INFO_X, INFO_Y + TEXT_OFFSET * 10, text, charset);

	sprintf(text, "esc - exit");
	DrawString(renderer, INFO_X, INFO_Y + TEXT_OFFSET * 12, text, charset);

	sprintf(text, "Score: %d", score);
	DrawString(renderer, SCREEN_WIDTH / 2 - 30, 10, text, charset);

	sprintf(text, "Health: %d/%d", playerHP, PLAYER_HEALTH);
	DrawString(renderer, SCREEN_WIDTH / 2 - 30, 10 + TEXT_OFFSET, text, charset);
}


void DrawMenu(SDL_Renderer* renderer, SDL_Texture* charset, int isGameStarted, int isGamePaused, int isLoadingMenu, int cursorPos, std::string* arrayOfFiles, int& numOfFiles) {
	char text[128];
	char cursor[2] = ">";
	const char* toDisplay;
	if (!isGameStarted) {
		sprintf(text, "New Game");
		DrawString(renderer, SCREEN_WIDTH / 2 - 30, SCREEN_HEIGHT / 4, text, charset);

		sprintf(text, "Press 'n' to play...");
		DrawString(renderer, SCREEN_WIDTH / 2 - 75, SCREEN_HEIGHT / 4 + TEXT_OFFSET * 2, text, charset);
	}
	else if (isGamePaused) {
		sprintf(text, "Pause");
		DrawString(renderer, SCREEN_WIDTH / 2 - 15, SCREEN_HEIGHT / 4, text, charset);

		sprintf(text, "Press 'p' to continue...");
		DrawString(renderer, SCREEN_WIDTH / 2 - 85, SCREEN_HEIGHT / 4 + TEXT_OFFSET * 2, text, charset);
	}
	else if (isLoadingMenu) {
		int i = 2;	//iterator to use for text offset
		std::string* filenamesArray = GetNameFiles(numOfFiles), line;
		sprintf(text, "Choose the file to load the game from...");
		DrawString(renderer, LOAD_X - 20, LOAD_Y, text, charset);

		for (int j = 0; j < numOfFiles; j++) {
			filenamesArray[j].erase(filenamesArray[j].length() - 1);
			toDisplay = filenamesArray[j].c_str();
			sprintf(text, toDisplay);
			DrawString(renderer, LOAD_X, LOAD_Y + TEXT_OFFSET * i, text, charset);
			i += 2;
		}

		DrawString(renderer, LOAD_X - 20, LOAD_Y + TEXT_OFFSET * cursorPos, cursor, charset);
	}
}