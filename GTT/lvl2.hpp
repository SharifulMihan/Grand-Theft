#ifndef LVL2_H
#define LVL2_H

#include <iostream>
#include "utility.hpp"
#include "Obstacle.hpp"
#include "VisualFX.hpp"
#include "Scoring.hpp"
#include "Checkpoint.hpp"
#include "Controller.hpp"

using namespace std;

extern int mat[720][1200];
extern int lvl2BgImage;
extern int backgroundImage;

// Custom Checkpoint Coordinates for Level 2 (Modify these X and Y values)
int lvl2CheckpointX = 1100;
int lvl2CheckpointY = 180;

// ============================================================================
// PLATFORMS
// ============================================================================
static Platform level2Platforms[] = {
	Platform(0, 0, 1200, 150, platformLvl2Image),

	Platform(0, 280, 50, 20, platformLvl2Image),
	Platform(100, 280, 280, 20, platformLvl2Image),
	Platform(450, 280, 280, 20, platformLvl2Image),
	Platform(800, 280, 400, 20, platformLvl2Image),

	Platform(1100, 340, 140, 15, platformLvl2Image),

	Platform(0, 420, 330, 15, platformLvl2Image),
	Platform(390, 420, 370, 15, platformLvl2Image),
	Platform(800, 420, 250, 15, platformLvl2Image),
	Platform(1099, 420, 101, 15, platformLvl2Image),

	Platform(0, 540, 50, 15, platformLvl2Image),
	Platform(100, 540, 1140, 15, platformLvl2Image),

	Platform(650, 635, 265, 15, platformLvl2Image),//top
	Platform(1000, 635, 200, 15, platformLvl2Image)
};
const int platform2Count = sizeof(level2Platforms) / sizeof(level2Platforms[0]);

// ============================================================================
// STAIRS
// ============================================================================
static Stair level2Stairs[] = {
	{ 50, 150, 50, 155 },
	{ 1049, 300, 50, 135 },
	{ 50, 435, 50, 120 },
};
const int stair2Count = sizeof(level2Stairs) / sizeof(level2Stairs[0]);

// ============================================================================
// TIMED GATES
// ============================================================================
static TimedGate level2Gates[] = {
	TimedGate(1050, 150, 22, 135, 100, 70),
	//TimedGate(620, 255, 22, 115, 90, 80),
	TimedGate(772, 425, 12, 120, 90, 95),


	TimedGate(650, 560, 10, 75, 100, 110),
	TimedGate(700, 560, 10, 75, 60, 70),
	TimedGate(750, 560, 10, 75, 100, 110),
	TimedGate(800, 560, 10, 75, 70, 70),
	TimedGate(850, 560, 10, 75, 100, 110),
	TimedGate(900, 560, 10, 75, 60, 90),

	TimedGate(1010, 560, 10, 75, 60, 70),
	TimedGate(1060, 560, 10, 75, 75, 80),
	TimedGate(1110, 560, 10, 75, 85, 65),

};
const int gate2Count = sizeof(level2Gates) / sizeof(level2Gates[0]);

// ============================================================================
// LASERS
// ============================================================================
static Laser level2Lasers[] = {
	Laser(500, 435, 80, 8, 310, 420, 1.8),
	Laser(850, 435, 70, 8, 310, 420, 1.8),
	Laser(980, 435, 70, 8, 310, 420, 2.1),

	//Laser(200, 575, 80, 8, 450, 580, 1.5),
	//Laser(700, 575, 100, 8, 450, 580, 1.8),
};
const int laser2Count = sizeof(level2Lasers) / sizeof(level2Lasers[0]);

// ============================================================================
// BOMBS
// ============================================================================
static Bomb level2Bombs[] = {
	Bomb(150, 305, 20, 20, 4.0, 0.30),
	Bomb(650, 305, 20, 20, 4.0, 0.30),
	//Bomb(500, 560, 20, 20, 3.5, 0.28),
	//Bomb(850, 560, 20, 20, 3.5, 0.28),
};
const int bomb2Count = sizeof(level2Bombs) / sizeof(level2Bombs[0]);

// ============================================================================
// CUTTERS
// ============================================================================
static Cutter level2Cutters[] = {
	Cutter(250, 150, 30, 30, 140, 370, 1.3),
	Cutter(900, 150, 30, 30, 740, 1000, 1.3),
	Cutter(300, 558, 30, 30, 100, 500, 1.8),
};
const int cutter2Count = sizeof(level2Cutters) / sizeof(level2Cutters[0]);

// ============================================================================
// SECURITY CAMERAS
// ============================================================================
static SecurityCamera level2Cameras[] = {
	SecurityCamera(600, 280, -85.0, 85.0, 0.40, 130.0),
	SecurityCamera(220, 540, -80.0, 85.0, 0.55, 110.0),
	SecurityCamera(880, 540, -10.0, 80.0, 0.55, 110.0),
	SecurityCamera(600, 540, -85.0, 55.0, 0.50, 100.0),
};
const int camera2Count = sizeof(level2Cameras) / sizeof(level2Cameras[0]);

// ============================================================================
// COLLECTIBLE GEMS
// ============================================================================
static Gem level2Gems[] = {
	Gem(220, 310),
	Gem(560, 455),
	Gem(880, 455),
	Gem(380, 580),
	Gem(940, 580),
};
const int gem2Count = sizeof(level2Gems) / sizeof(level2Gems[0]);

// ============================================================================
// SWITCH, LIFT, KEY, LOOTBOX, DOOR
// ============================================================================
static TriggerSwitch level2Switch(1150, 560, 30, 30);
static MovableLift   level2Lift(920, 540, 50, 15, 580, 2);
static Key     level2Key(1120, 170, 35, 25);
static LootBox level2LootBox(1160, 351, 40, 40);
static Door    level2Door(670, 650, 50, 70);

// Spawns player by querying Checkpoint.hpp with Level 2 default coordinates (15, 160)
inline void lvl2Initialize(int &px, int &py) {
	GameCheckpoint.loadSpawnPoint(2, px, py, level2Key.isCollected, 15, 160);
}

// ============================================================================
// BIT-MASKING
// ============================================================================
inline void level2Mat() {
	for (int row = 0; row < 720; row++)
	for (int col = 0; col < 1200; col++)
		mat[row][col] = 0;

	for (int i = 0; i < platform2Count; i++) {
		Platform p = level2Platforms[i];
		for (int row = p.y; row < p.y + p.h && row < 720; row++)
		for (int col = p.x; col < p.x + p.w && col < 1200; col++)
		if (col >= 0 && row >= 0) mat[row][col] = 1;
	}

	level2Lift.writeToMatrix();

	for (int i = 0; i < stair2Count; i++) {
		Stair s = level2Stairs[i];
		for (int row = s.y; row < s.y + s.h && row < 720; row++)
		for (int col = s.x; col < s.x + s.w && col < 1200; col++)
		if (col >= 0 && row >= 0) mat[row][col] = 2;
	}

	level2Switch.writeToMatrix();
}

// ============================================================================
// UPDATE
// ============================================================================
inline void updateLevel2Logic() {
	level2Lift.update(level2Switch.activated);
	if (level2Lift.hasMoved) level2Mat();

	if (level2LootBox.isOpened) level2Door.isOpened = true;
	level2Door.update();

	for (int i = 0; i < camera2Count; i++) level2Cameras[i].update();
	for (int i = 0; i < gem2Count; i++) level2Gems[i].update();
	for (int i = 0; i < laser2Count; i++) level2Lasers[i].update();
	for (int i = 0; i < bomb2Count; i++) level2Bombs[i].update();
	for (int i = 0; i < cutter2Count; i++) level2Cutters[i].update();
	for (int i = 0; i < gate2Count; i++) level2Gates[i].update();
}

// ============================================================================
// MUSEUM BACKDROP
// ============================================================================
inline void drawLevel2MuseumBackdrop() {
	iSetColor(8, 15, 31);
	iFilledRectangle(0, 0, 1200, 860);

	iSetColor(17, 31, 55);
	iFilledRectangle(25, 105, 1150, 555);
	iSetColor(31, 52, 82);
	iFilledRectangle(40, 120, 1120, 520);

	for (int x = 90; x < 1140; x += 160) {
		for (int y = 185; y < 620; y += 145) {
			iSetColor(13, 31, 54);
			iFilledRectangle(x, y, 88, 55);
			iSetColor(47, 102, 145);
			iRectangle(x, y, 88, 55);
		}
	}

	iSetColor(174, 141, 57);
	iFilledRectangle(430, 665, 340, 30);
	iSetColor(10, 20, 36);
	iText(500, 674, "MUSEUM VAULT  //  SECTOR 02", GLUT_BITMAP_HELVETICA_18);

	iSetColor(70, 105, 140);
	iLine(45, 245, 1155, 245);
	iLine(45, 415, 1155, 415);
	iLine(45, 555, 1155, 555);
}

// ============================================================================
// ITEM COLLISIONS
// ============================================================================
inline void checkLevel2ItemCollisions(int px, int py, int pSize) {
	bool keyWasCollected = level2Key.isCollected;
	level2Key.checkCollision(px, py, pSize);

	if (!keyWasCollected && level2Key.isCollected) {
		playPointSound();
		GameController.triggerKeyCollected();
		// Sets checkpoint using custom defined coordinates
		GameCheckpoint.setCheckpoint(lvl2CheckpointX, lvl2CheckpointY, 2);
		spawnFloatingText(lvl2CheckpointX, lvl2CheckpointY + 30, "CHECKPOINT REACHED!", 0, 255, 120);
	}

	bool lootBoxWasOpened = level2LootBox.isOpened;
	level2LootBox.checkCollision(px, py, pSize, level2Key.isCollected);
	if (!lootBoxWasOpened && level2LootBox.isOpened) {
		playPointSound();
		// Lootbox acts as a checkpoint — update spawn to lootbox position
		GameCheckpoint.setCheckpoint(level2LootBox.x, level2LootBox.y, 2);
		spawnFloatingText(level2LootBox.x - 20, level2LootBox.y + level2LootBox.h + 10, "CHECKPOINT SET!", 0, 255, 120);
	}

	for (int i = 0; i < gem2Count; i++) {
		if (level2Gems[i].checkCollision(px, py, pSize)) {
			gemsCollected++;
			playerScore += 200;
			spawnFloatingText(level2Gems[i].x, level2Gems[i].y + 20, "+200 CASH!", 50, 255, 120);
			spawnCoinBurst(level2Gems[i].x, level2Gems[i].y, 10);
			playPointSound();
			GameController.triggerCashCollected();
		}
	}
}

// ============================================================================
// OBSTACLE COLLISIONS
// ============================================================================
inline bool checkLevel2LaserCollision(int px, int py, int pSize) {
	for (int i = 0; i < laser2Count; i++)
	if (level2Lasers[i].checkCollision(px, py, pSize)) return true;
	return false;
}

inline bool checkLevel2BombCollision(int px, int py, int pSize) {
	for (int i = 0; i < bomb2Count; i++)
	if (level2Bombs[i].checkCollision(px, py, pSize)) return true;
	return false;
}

inline bool checkLevel2CutterCollision(int px, int py, int pSize) {
	for (int i = 0; i < cutter2Count; i++)
	if (level2Cutters[i].checkCollision(px, py, pSize)) return true;
	return false;
}

inline bool checkLevel2CameraCollision(int px, int py, int pSize) {
	for (int i = 0; i < camera2Count; i++)
	if (level2Cameras[i].checkDetection(px, py, pSize)) return true;
	return false;
}

inline bool checkLevel2GateCollision(int px, int py, int pSize) {
	for (int i = 0; i < gate2Count; i++)
	if (level2Gates[i].checkCollision(px, py, pSize)) return true;
	return false;
}

// ============================================================================
// RENDER
// ============================================================================
inline void drawLevel2() {
	iShowImage(0, 0, 1200, 860, lvl2BgImage);

	for (int i = 0; i < platform2Count; i++) level2Platforms[i].draw();

	level2Lift.draw();

	for (int i = 0; i < stair2Count; i++) {
		Stair s = level2Stairs[i];
		iShowImage(s.x, s.y, s.w, s.h, stairImage);
	}

	level2Switch.draw();

	for (int i = 0; i < gate2Count; i++) level2Gates[i].draw();
	for (int i = 0; i < gem2Count; i++) level2Gems[i].draw();
	for (int i = 0; i < camera2Count; i++) level2Cameras[i].draw();

	level2Key.draw();
	level2LootBox.draw();
	level2Door.draw();

	for (int i = 0; i < laser2Count; i++) level2Lasers[i].draw();
	for (int i = 0; i < bomb2Count; i++) level2Bombs[i].draw();
	for (int i = 0; i < cutter2Count; i++) level2Cutters[i].draw();
}

#endif