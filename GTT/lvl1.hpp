#ifndef LVL1_H
#define LVL1_H

#include <iostream>
#include "utility.hpp"
#include "Obstacle.hpp"
#include "VisualFX.hpp"
#include "Scoring.hpp"
#include "Checkpoint.hpp"
#include "Controller.hpp"

using namespace std;
extern int mat[720][1200];
extern int PlayerX_For_Parallax;
extern int laserImage;

// Custom Checkpoint Coordinates for Level 1 (Modify these X and Y values)
int lvl1CheckpointX = 200;
int lvl1CheckpointY = 620;

// necessary structures
struct Platform {
	int x, y, w, h;
	const int* imagePtr; // Pointer to global image variable (evaluated dynamically at render time)

	Platform(int posX = 0, int posY = 0, int width = 0, int height = 0, const int& customImg = platformImage) {
		this->x = posX;
		this->y = posY;
		this->w = width;
		this->h = height;
		this->imagePtr = &customImg;
	}

	void draw() {
		int defaultImg = (currentLevel == 2) ? platformLvl2Image : platformLvl1Image;
		if (defaultImg <= 0) defaultImg = platformImage;

		int imgToUse = (imagePtr != NULL && *imagePtr > 0 && imagePtr != &platformImage) ? *imagePtr : defaultImg;
		iShowImage(x, y, w, h, imgToUse);
	}
};
struct Stair {
	int x, y, w, h;
};

// level 1 platforms
Platform level1Platforms[] = {
	Platform(0, 0, 1200, 150, platformLvl1Image),      // Primary Ground
	Platform(0, 250, 450, 20, platformLvl1Image),
	Platform(500, 250, 900, 20, platformLvl1Image),    // Secondary Platform

	Platform(1050, 330, 150, 15, platformLvl1Image),
	Platform(1045, 455, 155, 15, platformLvl1Image),

	Platform(250, 370, 730, 15, platformLvl1Image),    // Upper Platform
	Platform(530, 490, 440, 15, platformLvl1Image),    // Exiting floor

	Platform(250, 530, 200, 15, platformLvl1Image),
	Platform(0, 600, 200, 15, platformLvl1Image)
};
const int platformCount = sizeof(level1Platforms) / sizeof(level1Platforms[0]);

// Stairs
Stair level1Stairs[] = {
	{ 450, 150, 50, 120 }
};
const int stairCount = sizeof(level1Stairs) / sizeof(level1Stairs[0]);

// Lasers
Laser level1Lasers[] = {
	Laser(530, 355, 120, 8, 270, 360, 1),
	Laser(720, 355, 70, 8, 270, 360, 1.5),
	Laser(845, 355, 30, 8, 270, 360, 2),

	Laser(280, 355, 120, 8, 270, 360, 1.2)
};
const int laserCount = sizeof(level1Lasers) / sizeof(level1Lasers[0]);

// Bombs
Bomb level1Bombs[] = {
	Bomb(600, 385, 20, 20, 3.0, 0.2),
	Bomb(820, 385, 20, 20, 3.0, 0.2),

	Bomb(190, 270, 20, 20, 3.0, 0.2)
};
const int bombCount = sizeof(level1Bombs) / sizeof(level1Bombs[0]);

// Cutters
Cutter level1Cutters[] = {
	Cutter(600, 505, 30, 30, 570, 950, 1.6),
	Cutter(950, 505, 30, 30, 570, 950, 1.6)
};
const int cutterCount = sizeof(level1Cutters) / sizeof(level1Cutters[0]);

// Security Cameras array
static SecurityCamera level1Cameras[] = {
	SecurityCamera(100, 720, -50.0, 40.0, 0.7, 130.0),
	SecurityCamera(900, 250, -85.0, 85.0, 0.7, 130.0)
};
const int cameraCount = sizeof(level1Cameras) / sizeof(level1Cameras[0]);

// Collectible Gems
static Gem level1Gems[] = {
	Gem(70, 280),
	Gem(880, 520),
	Gem(1100, 370)
};
const int gemCount = sizeof(level1Gems) / sizeof(level1Gems[0]);

// lift, switch, key, lootbox, and door
MovableLift level1Lift(150, 530, 100, 15, 450, 1);
TriggerSwitch level1Switch(350, 394, 30, 30);
Key level1Key(75, 620, 35, 25);
LootBox level1LootBox(1100, 146, 40, 40);
Door level1Door(1120, 465, 50, 70);

// Spawns player by querying Checkpoint.hpp with Level 1 default coordinates (15, 160)
inline void lvl1Initialize(int &px, int &py) {
	GameCheckpoint.loadSpawnPoint(1, px, py, level1Key.isCollected, 15, 160);
}

// bit masking for all platforms and others object
void level1Mat() {
	for (int row = 0; row < 720; row++) {
		for (int col = 0; col < 1200; col++) {
			mat[row][col] = 0;
		}
	}

	for (int i = 0; i < platformCount; i++) {
		Platform p = level1Platforms[i];
		for (int row = p.y; row < p.y + p.h && row < 720; row++) {
			for (int col = p.x; col < p.x + p.w && col < 1200; col++) {
				if (col >= 0 && row >= 0) {
					mat[row][col] = 1;
				}
			}
		}
	}

	level1Lift.writeToMatrix();

	for (int i = 0; i < stairCount; i++) {
		Stair s = level1Stairs[i];
		for (int row = s.y; row < s.y + s.h && row < 720; row++) {
			for (int col = s.x; col < s.x + s.w && col < 1200; col++) {
				if (col >= 0 && row >= 0) {
					mat[row][col] = 2;
				}
			}
		}
	}

	level1Switch.writeToMatrix();
}

// update level logics
void updateLevel1Logic() {
	level1Lift.update(level1Switch.activated);
	if (level1Lift.hasMoved) {
		level1Mat();
	}

	if (level1LootBox.isOpened) {
		level1Door.isOpened = true;
	}

	level1Door.update();

	for (int i = 0; i < cameraCount; i++) {
		level1Cameras[i].update();
	}

	for (int i = 0; i < gemCount; i++) {
		level1Gems[i].update();
	}

	for (int i = 0; i < laserCount; i++) {
		level1Lasers[i].update();
	}

	for (int i = 0; i < bombCount; i++) {
		level1Bombs[i].update();
	}

	for (int i = 0; i < cutterCount; i++) {
		level1Cutters[i].update();
	}
}

// Check collisions for key, lootbox, and gems
void checkLevel1ItemCollisions(int px, int py, int pSize) {
	bool keyWasCollected = level1Key.isCollected;
	level1Key.checkCollision(px, py, pSize);

	if (!keyWasCollected && level1Key.isCollected) {
		playPointSound();
		GameController.triggerKeyCollected();
		// Sets checkpoint using custom defined coordinates
		GameCheckpoint.setCheckpoint(lvl1CheckpointX, lvl1CheckpointY, 1);
		spawnFloatingText(lvl1CheckpointX, lvl1CheckpointY + 30, "CHECKPOINT REACHED!", 0, 255, 120);
	}

	bool lootBoxWasOpened = level1LootBox.isOpened;
	level1LootBox.checkCollision(px, py, pSize, level1Key.isCollected);
	if (!lootBoxWasOpened && level1LootBox.isOpened) {
		playPointSound();
		// Lootbox acts as a checkpoint — update spawn to lootbox position
		GameCheckpoint.setCheckpoint(level1LootBox.x, level1LootBox.y, 1);
		spawnFloatingText(level1LootBox.x - 20, level1LootBox.y + level1LootBox.h + 10, "CHECKPOINT SET!", 0, 255, 120);
	}

	for (int i = 0; i < gemCount; i++) {
		if (level1Gems[i].checkCollision(px, py, pSize)) {
			gemsCollected++;
			playerScore += 200;
			spawnFloatingText(level1Gems[i].x, level1Gems[i].y + 20, "+200 CASH!", 50, 255, 120);
			spawnCoinBurst(level1Gems[i].x, level1Gems[i].y, 10);
			playPointSound();
			GameController.triggerCashCollected();
		}
	}
}

// Render Level 1 objects
void drawLevel1() {
	iShowImage(0, 0, 1200, 860, backgroundImage);

	for (int i = 0; i < platformCount; i++) {
		level1Platforms[i].draw();
	}

	level1Lift.draw();

	for (int i = 0; i < stairCount; i++) {
		Stair s = level1Stairs[i];
		iShowImage(s.x, s.y, s.w, s.h, stairImage);
	}

	level1Switch.draw();

	for (int i = 0; i < gemCount; i++) {
		level1Gems[i].draw();
	}

	for (int i = 0; i < cameraCount; i++) {
		level1Cameras[i].draw();
	}

	level1Key.draw();
	level1LootBox.draw();
	level1Door.draw();

	for (int i = 0; i < laserCount; i++) {
		level1Lasers[i].draw();
	}

	for (int i = 0; i < bombCount; i++) {
		level1Bombs[i].draw();
	}

	for (int i = 0; i < cutterCount; i++) {
		level1Cutters[i].draw();
	}
}

// Collision checks
bool checkLevel1LaserCollision(int px, int py, int pSize) {
	for (int i = 0; i < laserCount; i++) {
		if (level1Lasers[i].checkCollision(px, py, pSize)) return true;
	}
	return false;
}

bool checkLevel1BombCollision(int px, int py, int pSize) {
	for (int i = 0; i < bombCount; i++) {
		if (level1Bombs[i].checkCollision(px, py, pSize)) return true;
	}
	return false;
}

bool checkLevel1CutterCollision(int px, int py, int pSize) {
	for (int i = 0; i < cutterCount; i++) {
		if (level1Cutters[i].checkCollision(px, py, pSize)) return true;
	}
	return false;
}

bool checkLevel1CameraCollision(int px, int py, int pSize) {
	for (int i = 0; i < cameraCount; i++) {
		if (level1Cameras[i].checkDetection(px, py, pSize)) return true;
	}
	return false;
}

#endif