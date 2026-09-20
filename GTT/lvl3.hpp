#ifndef LVL3_HPP
#define LVL3_HPP

#include <cstring>
#include "lvl1.hpp"

extern int portalImage;

const int level3EscapeTime = 45;
enum Level3Room { PORTAL_VAULT, KEY_CHAMBER };
static Level3Room level3Room = PORTAL_VAULT;
static bool level3PortalHeld = false;
static int level3PortalCooldown = 0;

static Platform level3VaultPlatforms[] = {
	Platform(0, 0, 1200, 150, platformLvl2Image),
	Platform(560, 300, 90, 20, platformLvl2Image),
	Platform(710, 300, 490, 20, platformLvl2Image)
};
static Platform level3ChamberPlatforms[] = {
	Platform(0, 0, 1200, 150, platformLvl1Image),
	Platform(430, 300, 80, 20, platformLvl1Image),
	Platform(570, 300, 130, 20, platformLvl1Image),
	Platform(860, 300, 340, 20, platformLvl1Image)
};
static Stair level3VaultStair = { 650, 150, 60, 190 };
static Stair level3ChamberStair = { 510, 150, 60, 190 };
static TriggerSwitch level3Switch(620, 320, 30, 30);
static MovableLift level3Lift(720, 440, 100, 15, 300, 2);
static Key level3Key(1080, 340, 35, 25);
static LootBox level3LootBox(460, 150, 44, 40);
static Door level3Door(1100, 150, 50, 70);
static Cutter level3Cutter(255, 150, 26, 26, 240, 335, 0.6);
static Bomb level3Bomb(545, 151, 18, 18, 2.0, 0.15);
static Laser level3Laser(350, 230, 46, 8, 150, 280, 0.8);
static TimedGate level3Gate(840, 320, 16, 90, 200, 60);
static SecurityCamera level3Camera(1030, 250, -65, 65, 0.35, 85);
static Gem level3Gems[] = {
	Gem(390, 175), Gem(770, 345), Gem(950, 345),
	Gem(260, 175), Gem(650, 345), Gem(1030, 175)
};
const int gem3Count = sizeof(level3Gems) / sizeof(level3Gems[0]);

struct Level3Checkpoint {
	Level3Room room;
	bool hasKey, hasLoot, switchActive;
	bool cash[gem3Count];
	int score, cashCount;
};
static Level3Checkpoint level3Checkpoint = {};

inline bool level3Overlaps(int px, int py, int size, int x, int y, int w, int h) {
	return px < x + w && px + size > x && py < y + h && py + size > y;
}

inline void saveLevel3Checkpoint(int x, int y) {
	GameCheckpoint.setCheckpoint(x, y, 3);
	level3Checkpoint.room = level3Room;
	level3Checkpoint.hasKey = level3Key.isCollected;
	level3Checkpoint.hasLoot = level3LootBox.isOpened;
	level3Checkpoint.switchActive = level3Switch.activated;
	level3Checkpoint.score = playerScore;
	level3Checkpoint.cashCount = gemsCollected;
	for (int i = 0; i < gem3Count; i++) {
		level3Checkpoint.cash[i] = level3Gems[i].isCollected;
	}
}

inline void level3Mat() {
	memset(mat, 0, sizeof(mat));
	Platform* platforms = level3Room == PORTAL_VAULT ? level3VaultPlatforms : level3ChamberPlatforms;
	int count = level3Room == PORTAL_VAULT
		? sizeof(level3VaultPlatforms) / sizeof(level3VaultPlatforms[0])
		: sizeof(level3ChamberPlatforms) / sizeof(level3ChamberPlatforms[0]);
	for (int i = 0; i < count; i++) {
		const Platform& p = platforms[i];
		for (int y = p.y; y < p.y + p.h && y < 720; y++) {
			for (int x = p.x; x < p.x + p.w && x < 1200; x++) {
				mat[y][x] = 1;
			}
		}
	}
	const Stair& stair = level3Room == PORTAL_VAULT ? level3VaultStair : level3ChamberStair;
	for (int y = stair.y; y < stair.y + stair.h; y++) {
		for (int x = stair.x; x < stair.x + stair.w; x++) {
			mat[y][x] = 2;
		}
	}
	if (level3Room == KEY_CHAMBER) {
		level3Lift.writeToMatrix();
		level3Switch.writeToMatrix();
	}
}

inline void resetLevel3(int& px, int& py) {
	bool resume = GameCheckpoint.isActive && GameCheckpoint.levelID == 3;
	level3Room = resume ? level3Checkpoint.room : PORTAL_VAULT;
	level3Key = Key(1080, 340, 35, 25);
	level3Key.isCollected = resume && level3Checkpoint.hasKey;
	level3LootBox.isOpened = resume && level3Checkpoint.hasLoot;
	level3Door.isOpened = level3LootBox.isOpened;
	level3Switch.activated = resume && level3Checkpoint.switchActive;
	level3Lift.reset();
	if (level3Switch.activated) level3Lift.y = level3Lift.targetY;
	level3Cutter = Cutter(255, 150, 26, 26, 240, 335, 0.6);
	level3Bomb = Bomb(545, 151, 18, 18, 2.0, 0.15);
	level3Laser = Laser(350, 230, 46, 8, 150, 280, 0.8);
	level3Camera = SecurityCamera(1030, 250, -65, 65, 0.35, 85);
	level3Gate.reset();
	for (int i = 0; i < gem3Count; i++) {
		level3Gems[i].isCollected = resume && level3Checkpoint.cash[i];
	}
	playerScore = resume ? level3Checkpoint.score : 0;
	gemsCollected = resume ? level3Checkpoint.cashCount : 0;
	px = resume ? GameCheckpoint.x : 60;
	py = resume ? GameCheckpoint.y : 150;
	level3PortalCooldown = 20;
	level3PortalHeld = true;
	level3Mat();
}

inline bool useLevel3Portal(int& px, int& py, int size, bool held) {
	bool pressed = held && !level3PortalHeld;
	level3PortalHeld = held;
	if (level3PortalCooldown > 0) {
		level3PortalCooldown--;
		return false;
	}
	int portalX = level3Room == PORTAL_VAULT ? 1040 : 80;
	int portalY = level3Room == PORTAL_VAULT ? 320 : 150;
	if (!pressed || level3LootBox.isOpened ||
		!level3Overlaps(px, py, size, portalX - 16, portalY, 104, 100)) return false;

	if (level3Room == PORTAL_VAULT) {
		level3Room = KEY_CHAMBER;
		px = 190;
		py = 150;
	}
	else {
		level3Room = PORTAL_VAULT;
		px = 960;
		py = 320;
	}
	level3PortalCooldown = 25;
	clearParticles();
	initFloatingTexts();
	resetShake();
	level3Mat();
	saveLevel3Checkpoint(px, py);
	return true;
}

inline void updateLevel3Logic() {
	if (level3Room == PORTAL_VAULT) {
		level3Cutter.update();
		level3Bomb.update();
		level3Gate.update();
	}
	else {
		level3Lift.update(level3Switch.activated);
		if (level3Lift.hasMoved) level3Mat();
		level3Laser.update();
		level3Camera.update();
		level3Key.update();
	}
	if (level3Key.isCollected) level3Gate.isOpen = true;
	level3Door.isOpened = level3LootBox.isOpened;
	for (int i = 0; i < gem3Count; i++) level3Gems[i].update();
}

inline bool checkLevel3ObstacleCollision(int px, int py, int size) {
	if (level3Key.isCollected) return false;
	if (level3Room == PORTAL_VAULT) {
		return level3Cutter.checkCollision(px, py, size) ||
			level3Bomb.checkCollision(px, py, size) || level3Gate.checkCollision(px, py, size);
	}
	return level3Laser.checkCollision(px, py, size) || level3Camera.checkDetection(px, py, size);
}

inline void checkLevel3ItemCollisions(int px, int py, int size) {
	int first = level3Room == PORTAL_VAULT ? 0 : 3;
	for (int i = first; i < first + 3; i++) {
		if (level3Gems[i].checkCollision(px, py, size)) {
			gemsCollected++;
			playerScore += 200;
			spawnFloatingText(level3Gems[i].x, level3Gems[i].y + 20, "+200 CASH!", 50, 255, 120);
			spawnCoinBurst(level3Gems[i].x, level3Gems[i].y, 10);
			playPointSound();
			GameController.triggerCashCollected();
		}
	}
	if (level3Room == KEY_CHAMBER) {
		if (!level3Switch.activated && level3Overlaps(px, py, size,
			level3Switch.x, level3Switch.y, level3Switch.w, level3Switch.h)) {
			level3Switch.activated = true;
			spawnFloatingText(580, 370, "BRIDGE LOWERING!", 100, 255, 220);
		}
		bool hadKey = level3Key.isCollected;
		level3Key.checkCollision(px, py, size);
		if (!hadKey && level3Key.isCollected) {
			playPointSound();
			GameController.triggerKeyCollected();
			saveLevel3Checkpoint(1080, 320);
		}
	}
	else {
		bool hadLoot = level3LootBox.isOpened;
		level3LootBox.checkCollision(px, py, size, level3Key.isCollected);
		if (!hadLoot && level3LootBox.isOpened) {
			level3Door.isOpened = true;
			playPointSound();
			saveLevel3Checkpoint(510, 150);
		}
	}
}

inline const char* level3Objective() {
	if (level3LootBox.isOpened) return "LOOT SECURED  /  Follow the ground floor RIGHT to the exit!";
	if (level3Room == KEY_CHAMBER) {
		return level3Key.isCollected ? "KEY FOUND  /  Security asleep. Return to the LEFT portal."
			: "Find the switch upstairs, cross the bridge, take the key.";
	}
	return level3Key.isCollected ? "BACK IN THE VAULT  /  Drop down and open the golden box."
		: "The key is in another room. Reach the portal on the RIGHT.";
}

inline void drawLevel3Portal(int x, int y) {
	iSetColor(65, 185, 225);
	iEllipse(x + 36, y + 45, 47 + sin(globalTimer) * 3, 59, 50);
	iShowImage(x, y, 72, 96, portalImage);
	iSetColor(180, 245, 255);
	iText(x - 34, y + 116, (char*)(level3LootBox.isOpened ? "ESCAPE BELOW" : "E / UP / PAD UP"),
		GLUT_BITMAP_HELVETICA_12);
}

inline void drawLevel3() {
	bool chamber = level3Room == KEY_CHAMBER;
	for (int y = 0; y < 860; y += 20) {
		iSetColor(chamber ? 19 + y / 70 : 8, 17 + y / 90, 34 + y / 30);
		iFilledRectangle(0, y, 1200, 20);
	}
	for (int x = 40; x < 1200; x += 160) {
		iSetColor(chamber ? 72 : 28, 55, 91);
		iRectangle(x, 175, 110, 475);
		iRectangle(x + 7, 182, 96, 461);
		iSetColor(65, 125, 150);
		iLine(x + 55, 570, x + 55, 615);
		iCircle(x + 55, 550, 12, 20);
	}
	iSetColor(10, 19, 35);
	iFilledRectangle(30, 700, 1140, 85);
	iSetColor(116, 222, 240);
	iText(55, 752, (char*)(chamber ? "LEVEL 3  /  THE ECHO CHAMBER" : "LEVEL 3  /  THE PORTAL VAULT"),
		GLUT_BITMAP_TIMES_ROMAN_24);
	iSetColor(240, 213, 149);
	iText(55, 719, (char*)level3Objective(), GLUT_BITMAP_HELVETICA_18);
	iSetColor(140, 182, 200);
	iText(55, 670, "PORTAL  >  KEY  >  RETURN  >  LOOT  >  EXIT", GLUT_BITMAP_HELVETICA_12);
	iText(55, 55, "A/D: move    SPACE: jump    W/S: stairs    E or UP: portal    Cash is optional",
		GLUT_BITMAP_HELVETICA_18);
	iText(55, 28, "Portals save progress. The key puts security to sleep. No timer until you open the box.",
		GLUT_BITMAP_HELVETICA_12);

	Platform* platforms = chamber ? level3ChamberPlatforms : level3VaultPlatforms;
	int count = chamber ? sizeof(level3ChamberPlatforms) / sizeof(level3ChamberPlatforms[0])
		: sizeof(level3VaultPlatforms) / sizeof(level3VaultPlatforms[0]);
	for (int i = 0; i < count; i++) platforms[i].draw();
	const Stair& stair = chamber ? level3ChamberStair : level3VaultStair;
	iShowImage(stair.x, stair.y, stair.w, stair.h, stairImage);
	if (chamber) {
		level3Lift.draw();
		level3Switch.draw();
		level3Key.draw();
		drawLevel3Portal(80, 150);
		if (!level3Key.isCollected) {
			level3Laser.draw();
			level3Camera.draw();
		}
		iSetColor(180, 245, 255);
		iText(590, 400, "SWITCH", GLUT_BITMAP_HELVETICA_12);
		iText(930, 365, "KEY THIS WAY >", GLUT_BITMAP_HELVETICA_12);
	}
	else {
		level3LootBox.draw();
		level3Door.draw();
		drawLevel3Portal(1040, 320);
		if (!level3Key.isCollected) {
			level3Cutter.draw();
			level3Bomb.draw();
		}
		level3Gate.draw();
		iSetColor(240, 213, 149);
		iText(435, 215, "GOLDEN BOX", GLUT_BITMAP_HELVETICA_12);
		iText(1090, 240, "EXIT", GLUT_BITMAP_HELVETICA_18);
		iText(800, 440, (char*)(level3Key.isCollected ? "SECURITY ASLEEP" : "WAIT FOR GREEN"),
			GLUT_BITMAP_HELVETICA_12);
	}
	int first = chamber ? 3 : 0;
	for (int i = first; i < first + 3; i++) level3Gems[i].draw();
}

#endif
