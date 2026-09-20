#ifndef LVL3_H
#define LVL3_H

#include <iostream>
#include "utility.hpp"
#include "Obstacle.hpp"
#include "VisualFX.hpp"
#include "Scoring.hpp"
#include "Checkpoint.hpp"
#include "Controller.hpp"
#include "Character.hpp"

using namespace std;

extern int mat[720][1200];
extern int portalImage;
extern int portalReImage; // Reversed portal for sublevel
extern int totalGemsInLevel;
extern bool playerInSubLevel;
extern int gemsCollected;
extern int playerScore;
extern int PlayerX_For_Parallax;
extern int laserImage;
extern int platformImage;
extern int platformLvl1Image;
extern int platformLvl2Image;
extern int platformLvl3HoriImage;
extern int platformLvl3VertiImage;
extern int platformSubLvl3HoriImage;
extern int platformSubLvl3VertiImage;
extern int stairImage;
extern int currentLevel;
extern int lvl3BgImage;
extern int bgSubLvl3Image;
extern Character Player;

// ============================================================================
// LEVEL 3 SPAWN POSITION - EASY TO MODIFY
// ============================================================================
// Change these values to set where the player spawns at the start of Level 3
const int LEVEL3_SPAWN_X = 20;   // Starting X position (horizontal)
const int LEVEL3_SPAWN_Y = 422;  // Starting Y position (vertical)

// ============================================================================
// PORTAL TELEPORTATION COORDINATES - EASY TO MODIFY
// ============================================================================
// Change these values to set where the player spawns when using portals
const int PORTAL_TO_SUBLEVEL_X = 1100;  // X position when entering sublevel
const int PORTAL_TO_SUBLEVEL_Y = 570;   // Y position when entering sublevel
const int PORTAL_TO_MAIN_X = 570;      // X position when returning to main level 480, 238,
const int PORTAL_TO_MAIN_Y = 250;       // Y position when returning to main level

// ============================================================================
// DEVELOPMENT MODE - DIRECT SUBLEVEL ACCESS
// ============================================================================
// Set this to true to directly start in the sublevel without using the portal
// Set this to false for normal gameplay (start in main level, use portal to access sublevel)
const bool ENABLE_SUBLEVEL_DIRECTLY = false;

// Sublevel spawn position (when ENABLE_SUBLEVEL_DIRECTLY is true)
const int SUBLEVEL_SPAWN_X = 1100;  // Top right spawn in sublevel
const int SUBLEVEL_SPAWN_Y = 570;  // Top right spawn in sublevel

// Custom Checkpoint Coordinates for Level 3
int lvl3CheckpointX = 650;
int lvl3CheckpointY = 300;

// Portal structure for teleportation mechanism
struct Portal {
	int x, y, w, h;
	bool isActive;
	bool isInSubLevel;
	
	Portal(int startX = 0, int startY = 0, int width = 50, int height = 70, bool inSubLevel = false) {
		this->x = startX;
		this->y = startY;
		this->w = width;
		this->h = height;
		this->isActive = true;
		this->isInSubLevel = inSubLevel;
	}
	
	void draw() {
		if (isActive) {
			// Use reversed portal image when this portal is in sublevel
			extern bool playerInSubLevel;
			if (isInSubLevel) {
				iShowImage(x, y, w, h, portalReImage);
			} else {
				iShowImage(x, y, w, h, portalImage);
			}
		}
	}
	
	bool checkCollision(int px, int py, int pSize) {
		if (!isActive) return false;
		bool overlapX = px < x + w && px + pSize > x;
		bool overlapY = py < y + h && py + pSize > y;
		return overlapX && overlapY;
	}
};

// Level 3 Portal (main level)
Portal level3Portal(480, 238, 50, 70);

// Return Portal (sublevel)
Portal level3ReturnPortal(50, 430, 50, 70, true);

// ============================================================================
// MAIN LEVEL PLATFORMS (Redesigned layout)
// ============================================================================
static Platform level3MainPlatforms[] = {
	// Main ground platform
	Platform(0, 400, 200, 20, platformLvl3HoriImage),
	
	
	
	
	// Vertical platforms for climbing
	Platform(260, 200, 60, 240, platformLvl3VertiImage),    // Vertical connector
	Platform(370, 150, 40, 340, platformLvl3VertiImage),    // Vertical connector
	Platform(460, 490, 40, 300, platformLvl3VertiImage),

	Platform(460, 360, 200, 20, platformLvl3HoriImage),
	Platform(460, 220, 200, 20, platformLvl3HoriImage), // portal platform
	Platform(700, 140, 200, 20, platformLvl3HoriImage),




	Platform(710, 240, 15, 170, platformLvl3VertiImage),
	Platform(790, 290, 20, 170, platformLvl3VertiImage),
	Platform(870, 200, 15, 170, platformLvl3VertiImage),
	Platform(940, 250, 15, 170, platformLvl3VertiImage),



	Platform(1000, 440, 200, 20, platformLvl3HoriImage), // most left platform
	

};
const int platform3MainCount = sizeof(level3MainPlatforms) / sizeof(level3MainPlatforms[0]);

// ============================================================================
// SUB LEVEL PLATFORMS (accessible through portal) - REVERSED GRAVITY DESIGN
// ============================================================================
static Platform level3SubPlatforms[] = {
	// Ceiling platform (acts as ground in reversed gravity)
	Platform(0, 700, 1200, 120, platformSubLvl3HoriImage),
	
	// Starting platform near spawn (top right)
	Platform(950, 550, 200, 20, platformSubLvl3HoriImage),
	
	// Central platforms for navigation
	Platform(650, 500, 250, 20, platformSubLvl3HoriImage),
	Platform(250, 420, 150, 15, platformSubLvl3HoriImage), //Top platform
	Platform(250, 500, 250, 20, platformSubLvl3HoriImage),
	
	// Vertical platforms for climbing between levels
	Platform(170, 450, 20, 200, platformSubLvl3VertiImage),
	Platform(570, 450, 30, 250, platformSubLvl3VertiImage),
	//Platform(850, 250, 20, 300, platformSubLvl3VertiImage),
	
	// Platform near return portal (bottom left area)
	Platform(50, 500, 50, 20, platformSubLvl3HoriImage),
};
const int platform3SubCount = sizeof(level3SubPlatforms) / sizeof(level3SubPlatforms[0]);

// ============================================================================
// STAIRS
// ============================================================================
// Main level stairs
static Stair level3Stairs[] = {
	//{ 200, 150, 50, 130 },
	{ 660, 140, 40, 240 },
};
const int stair3Count = sizeof(level3Stairs) / sizeof(level3Stairs[0]);

// Sub level stairs (separate for editing sublevel independently)
static Stair level3SubStairs[] = {
	{ 910, 550, 40, 150 }
};
const int stair3SubCount = sizeof(level3SubStairs) / sizeof(level3SubStairs[0]);

// ============================================================================
// MAIN LEVEL OBSTACLES
// ============================================================================
static Laser level3Lasers[] = {
	Laser(2000, 3015, 80, 8, 300, 400, 1.5),
	
};
const int laser3Count = sizeof(level3Lasers) / sizeof(level3Lasers[0]);

static Bomb level3Bombs[] = {
	Bomb(400, 3035, 20, 20, 3.0, 0.25),
	
};
const int bomb3Count = sizeof(level3Bombs) / sizeof(level3Bombs[0]);

static Cutter level3Cutters[] = {
	Cutter(300, 2000, 30, 30, 200, 450, 1.5),
	
};
const int cutter3Count = sizeof(level3Cutters) / sizeof(level3Cutters[0]);

static SecurityCamera level3Cameras[] = {
	SecurityCamera(400, 2080, -60.0, 60.0, 0.6, 150.0),
	
};
const int camera3Count = sizeof(level3Cameras) / sizeof(level3Cameras[0]);

static Gem level3Gems[] = {
	Gem(3500, 310),
	
};
const int gem3Count = sizeof(level3Gems) / sizeof(level3Gems[0]);

// ============================================================================
// SUB LEVEL OBSTACLES (separate from main level)
// ============================================================================
static Laser level3SubLasers[1] = {Laser(0, 0, 0, 0, 0, 0, 0)};
const int laser3SubCount = 0;

static Bomb level3SubBombs[1] = {Bomb(0, 0, 0, 0, 0, 0)};
const int bomb3SubCount = 0;

static Cutter level3SubCutters[1] = {Cutter(0, 0, 0, 0, 0, 0, 0)};
const int cutter3SubCount = 0;

static SecurityCamera level3SubCameras[1] = {SecurityCamera(0, 0, 0, 0, 0, 0)};
const int camera3SubCount = 0;

static Gem level3SubGems[1] = {Gem(0, 0)};
const int gem3SubCount = 0;

// ============================================================================
// SWITCH, LIFT, KEY, LOOTBOX, DOOR
// ============================================================================
static TriggerSwitch level3Switch(50, 5600, 30, 30);
static MovableLift   level3Lift(400, 5060, 100, 15, 400, 1.5);
static Key     level3Key(250, 370, 35, 25);  // Key in sub-level (on bottom platform)
static LootBox level3LootBox(800, 160, 40, 40);  // Lootbox in main level (near portal)
static Door    level3Door(1130, 460, 50, 70);

// Player position tracking for portal teleportation
int playerSpawnX = 15;
int playerSpawnY = 160;
extern bool playerInSubLevel;

// Gravity reversal for sublevel
extern bool reversedGravity;



// ============================================================================
// BIT-MASKING FOR MAIN LEVEL
// ============================================================================
void level3MainMat() {
	for (int row = 0; row < 720; row++)
	for (int col = 0; col < 1200; col++)
	mat[row][col] = 0;

	for (int i = 0; i < platform3MainCount; i++) {
		Platform p = level3MainPlatforms[i];
		for (int row = p.y; row < p.y + p.h && row < 720; row++)
		for (int col = p.x; col < p.x + p.w && col < 1200; col++)
		if (col >= 0 && row >= 0) mat[row][col] = 1;
	}

	level3Lift.writeToMatrix();

	for (int i = 0; i < stair3Count; i++) {
		Stair s = level3Stairs[i];
		for (int row = s.y; row < s.y + s.h && row < 720; row++)
		for (int col = s.x; col < s.x + s.w && col < 1200; col++)
		if (col >= 0 && row >= 0) mat[row][col] = 2;
	}

	level3Switch.writeToMatrix();
}

// ============================================================================
// BIT-MASKING FOR SUB LEVEL
// ============================================================================
void level3SubMat() {
	for (int row = 0; row < 720; row++)
	for (int col = 0; col < 1200; col++)
	mat[row][col] = 0;

	for (int i = 0; i < platform3SubCount; i++) {
		Platform p = level3SubPlatforms[i];
		for (int row = p.y; row < p.y + p.h && row < 720; row++)
		for (int col = p.x; col < p.x + p.w && col < 1200; col++)
		if (col >= 0 && row >= 0) mat[row][col] = 1;
	}

	for (int i = 0; i < stair3SubCount; i++) {
		Stair s = level3SubStairs[i];
		for (int row = s.y; row < s.y + s.h && row < 720; row++)
		for (int col = s.x; col < s.x + s.w && col < 1200; col++)
		if (col >= 0 && row >= 0) mat[row][col] = 2;
	}
}

// ============================================================================
// UPDATE
// ============================================================================
void updateLevel3Logic() {
	level3Lift.update(level3Switch.activated);
	if (level3Lift.hasMoved) {
		if (playerInSubLevel) {
			level3SubMat();
		} else {
			level3MainMat();
		}
	}

	if (level3LootBox.isOpened) level3Door.isOpened = true;
	level3Door.update();

	// Update main level obstacles only when in main level
	if (!playerInSubLevel) {
		for (int i = 0; i < camera3Count; i++) level3Cameras[i].update();
		for (int i = 0; i < gem3Count; i++) level3Gems[i].update();
		for (int i = 0; i < laser3Count; i++) level3Lasers[i].update();
		for (int i = 0; i < bomb3Count; i++) level3Bombs[i].update();
		for (int i = 0; i < cutter3Count; i++) level3Cutters[i].update();
	} else {
		// Update sublevel obstacles only when in sublevel
		for (int i = 0; i < camera3SubCount; i++) level3SubCameras[i].update();
		for (int i = 0; i < gem3SubCount; i++) level3SubGems[i].update();
		for (int i = 0; i < laser3SubCount; i++) level3SubLasers[i].update();
		for (int i = 0; i < bomb3SubCount; i++) level3SubBombs[i].update();
		for (int i = 0; i < cutter3SubCount; i++) level3SubCutters[i].update();
	}
}

// ============================================================================
// ITEM COLLISIONS
// ============================================================================
void checkLevel3ItemCollisions(int px, int py, int pSize) {
	// Check portal collision based on current level
	if (!playerInSubLevel) {
		// Only check main level portal when in main level
		if (level3Portal.checkCollision(px, py, pSize)) {
			// Teleport to sub-level
			playerInSubLevel = true;
			level3Portal.isInSubLevel = true;
			reversedGravity = true; // Enable reversed gravity
			Player.gravity = 0.3f; // Set player gravity to reversed
			Player.x = PORTAL_TO_SUBLEVEL_X; // Use portal spawn coordinates
			Player.y = PORTAL_TO_SUBLEVEL_Y;  // Use portal spawn coordinates
			Player.vy = 0; // Reset velocity
			Player.isJumping = false; // Reset jump state
			level3SubMat();
			spawnFloatingText(500, 400, "GRAVITY REVERSED!", 150, 100, 255);
			playPointSound();
		}
	} else {
		// Only check return portal when in sublevel
		if (level3ReturnPortal.checkCollision(px, py, pSize)) {
			// Check if player has collected the key before allowing return
			if (level3Key.isCollected) {
				// Return to main level
				playerInSubLevel = false;
				level3Portal.isInSubLevel = false;
				reversedGravity = false; // Restore normal gravity
				Player.gravity = -0.3f; // Set player gravity to normal
				Player.x = PORTAL_TO_MAIN_X; // Use portal return coordinates
				Player.y = PORTAL_TO_MAIN_Y;  // Use portal return coordinates
				Player.vy = 0; // Reset velocity
				Player.isJumping = false; // Reset jump state
				level3MainMat();
				spawnFloatingText(500, 400, "GRAVITY RESTORED!", 100, 255, 150);
				playPointSound();
			} else {
				// Key not collected, show message
				spawnFloatingText(500, 400, "COLLECT THE KEY FIRST!", 255, 100, 100);
			}
		}
	}

	// Key collision - only in sublevel
	bool keyWasCollected = level3Key.isCollected;
	if (playerInSubLevel) {
		level3Key.checkCollision(px, py, pSize);
	}

	if (!keyWasCollected && level3Key.isCollected) {
		playPointSound();
		GameController.triggerKeyCollected();
		GameCheckpoint.setCheckpoint(lvl3CheckpointX, lvl3CheckpointY, 3, true); // Sublevel checkpoint
		spawnFloatingText(lvl3CheckpointX, lvl3CheckpointY + 30, "CHECKPOINT REACHED!", 0, 255, 120);
	}

	// Lootbox collision - only in main level
	bool lootBoxWasOpened = level3LootBox.isOpened;
	if (!playerInSubLevel) {
		level3LootBox.checkCollision(px, py, pSize, level3Key.isCollected);
	}
	if (!lootBoxWasOpened && level3LootBox.isOpened) {
		playPointSound();
		GameCheckpoint.setCheckpoint(level3LootBox.x, level3LootBox.y, 3, false); // Main level checkpoint
		spawnFloatingText(level3LootBox.x - 20, level3LootBox.y + level3LootBox.h + 10, "CHECKPOINT SET!", 0, 255, 120);
	}

	// Check gem collisions based on current level
	if (playerInSubLevel) {
		for (int i = 0; i < gem3SubCount; i++) {
			if (level3SubGems[i].checkCollision(px, py, pSize)) {
				gemsCollected++;
				playerScore += 200;
				spawnFloatingText(level3SubGems[i].x, level3SubGems[i].y + 20, "+200 CASH!", 50, 255, 120);
				spawnCoinBurst(level3SubGems[i].x, level3SubGems[i].y, 10);
				playPointSound();
				GameController.triggerCashCollected();
			}
		}
	} else {
		for (int i = 0; i < gem3Count; i++) {
			if (level3Gems[i].checkCollision(px, py, pSize)) {
				gemsCollected++;
				playerScore += 200;
				spawnFloatingText(level3Gems[i].x, level3Gems[i].y + 20, "+200 CASH!", 50, 255, 120);
				spawnCoinBurst(level3Gems[i].x, level3Gems[i].y, 10);
				playPointSound();
				GameController.triggerCashCollected();
			}
		}
	}
}

// ============================================================================
// OBSTACLE COLLISIONS
// ============================================================================
bool checkLevel3LaserCollision(int px, int py, int pSize) {
	if (playerInSubLevel) {
		for (int i = 0; i < laser3SubCount; i++)
		if (level3SubLasers[i].checkCollision(px, py, pSize)) return true;
	} else {
		for (int i = 0; i < laser3Count; i++)
		if (level3Lasers[i].checkCollision(px, py, pSize)) return true;
	}
	return false;
}

bool checkLevel3BombCollision(int px, int py, int pSize) {
	if (playerInSubLevel) {
		for (int i = 0; i < bomb3SubCount; i++)
		if (level3SubBombs[i].checkCollision(px, py, pSize)) return true;
	} else {
		for (int i = 0; i < bomb3Count; i++)
		if (level3Bombs[i].checkCollision(px, py, pSize)) return true;
	}
	return false;
}

bool checkLevel3CutterCollision(int px, int py, int pSize) {
	if (playerInSubLevel) {
		for (int i = 0; i < cutter3SubCount; i++)
		if (level3SubCutters[i].checkCollision(px, py, pSize)) return true;
	} else {
		for (int i = 0; i < cutter3Count; i++)
		if (level3Cutters[i].checkCollision(px, py, pSize)) return true;
	}
	return false;
}

bool checkLevel3CameraCollision(int px, int py, int pSize) {
	if (playerInSubLevel) {
		for (int i = 0; i < camera3SubCount; i++)
		if (level3SubCameras[i].checkDetection(px, py, pSize)) return true;
	} else {
		for (int i = 0; i < camera3Count; i++)
		if (level3Cameras[i].checkDetection(px, py, pSize)) return true;
	}
	return false;
}

// ============================================================================
// RENDER
// ============================================================================
void drawLevel3() {
	// Draw different background based on level
	if (playerInSubLevel) {
		iShowImage(0, 0, 1200, 860, bgSubLvl3Image);
	} else {
		iShowImage(0, 0, 1200, 860, lvl3BgImage);
	}

	// Draw appropriate platforms
	if (playerInSubLevel) {
		for (int i = 0; i < platform3SubCount; i++) level3SubPlatforms[i].draw();
	} else {
		for (int i = 0; i < platform3MainCount; i++) level3MainPlatforms[i].draw();
		level3Lift.draw();
	}

	// Draw stairs based on current level
	if (playerInSubLevel) {
		for (int i = 0; i < stair3SubCount; i++) {
			Stair s = level3SubStairs[i];
			iShowImage(s.x, s.y, s.w, s.h, stairImage);
		}
	} else {
		for (int i = 0; i < stair3Count; i++) {
			Stair s = level3Stairs[i];
			iShowImage(s.x, s.y, s.w, s.h, stairImage);
		}
	}

	// Draw switch (only in main level)
	if (!playerInSubLevel) {
		level3Switch.draw();
	}

	// Draw gems, cameras, obstacles based on current level
	if (playerInSubLevel) {
		// Draw sublevel elements
		for (int i = 0; i < gem3SubCount; i++) level3SubGems[i].draw();
		for (int i = 0; i < camera3SubCount; i++) level3SubCameras[i].draw();
		for (int i = 0; i < laser3SubCount; i++) level3SubLasers[i].draw();
		for (int i = 0; i < bomb3SubCount; i++) level3SubBombs[i].draw();
		for (int i = 0; i < cutter3SubCount; i++) level3SubCutters[i].draw();
	} else {
		// Draw main level elements
		for (int i = 0; i < gem3Count; i++) level3Gems[i].draw();
		for (int i = 0; i < camera3Count; i++) level3Cameras[i].draw();
		for (int i = 0; i < laser3Count; i++) level3Lasers[i].draw();
		for (int i = 0; i < bomb3Count; i++) level3Bombs[i].draw();
		for (int i = 0; i < cutter3Count; i++) level3Cutters[i].draw();
	}

	// Draw key (only in sub-level)
	if (playerInSubLevel) {
		level3Key.draw();
	}

	// Draw lootbox and door (only in main level)
	if (!playerInSubLevel) {
		level3LootBox.draw();
		level3Door.draw();
	}

	// Draw portal
	if (!playerInSubLevel) {
		level3Portal.draw();
	} else {
		// Draw return portal in sub-level
		level3ReturnPortal.draw();
	}
}

#endif