#include "iGraphics.h"
#include <iostream>
#include <stdio.h>

// Custom Headers
#include "Effects.hpp"
#include "VisualFX.hpp"
#include "Scoring.hpp"
#include "utility.hpp"
#include "Character.hpp"
#include "Checkpoint.hpp"
#include "lvl1.hpp"
#include "lvl2.hpp"
#include "UI.hpp"
#include "MainMenu.hpp"
#include "MenuPage.hpp"

// Global position sync for parallax background
int PlayerX_For_Parallax = 0;

// Current active level (1 or 2)
int currentLevel = 1;

// Level 2 background image
int lvl2BgImage = 0;

// Initialize levels
void lvl1Initialize();
void lvl2Initialize();

// Starts a level fresh from the beginning spawn point
void startFreshLevel(int levelNum) {
	GameCheckpoint.clear(); // Clears active checkpoint state
	if (levelNum == 1) {
		lvl1Initialize();
	}
	else if (levelNum == 2) {
		lvl2Initialize();
	}
	currentGameState = STATE_PLAYING;
}

// Resets checkpoint and returns to main menu
void goToMainMenu() {
	GameCheckpoint.clear();
	stopAlarmSounds();
	stopLevelMusic();
	GameController.stopVibration();
	currentGameState = STATE_MAIN_MENU;
}

// Initial game state
GameState currentGameState = STATE_MAIN_MENU;

char uiMessage[128] = "";
int uiMessageTimer = 0;

// All image variables
int redImage;
int levelOne;
int menubackgroundImage;
int backgroundImage;
int lvlCompleteImage = 0;
int stairImage;
int laserImage;
int platformImage;
int platformLvl1Image;
int platformLvl2Image;
int liftPlatformImage;
int redSwitchImage;
int greenSwitchImage;
int keyImage;
int closedBoxImage;
int openedBoxImage;
int closedDoorImage;
int openedDoorImage;
int bompImage;
int restartBtnImage;
int menuBtnImage;
int soundOnImage = 0;
int soundOffImage = 0;
bool isSoundOn = true;
int cutterImages[3];
int cashImage = 0;
int escapeBarImages[10] = { 0 };
ControllerManager GameController;

// Menu Button image variables
int playBtnImage = 0;
int levelsBtnImage = 0;
int howToPlayBtnImage = 0;
int exitBtnImage = 0;
int backBtnImage = 0;

int restartBtn = 0;
int mainmenuBtn = 0;

// Level Buttons
int level1Btn = 0;
int level2Btn = 0;
int level3Btn = 0;

// Default boundary
int groundY = 250;
int leftWall = 0;
int rightWall = 1200;

// Bit masking array
int mat[720][1200] = { 0 };

// Game status flags
bool isGameOver = false;
bool isLevelComplete = false;

int countdownTimer = 15;
bool isCountdownActive = false;

// Main render loop
void iDraw()
{
	iClear();

	if (currentGameState == STATE_MAIN_MENU) {
		drawMainMenu();
		return;
	}

	if (currentGameState == STATE_LEVELS_MENU) {
		drawLevelPage();
		return;
	}

	if (currentGameState == STATE_HOW_TO_PLAY) {
		drawHowToPlayPage();
		return;
	}

	if (currentGameState == STATE_PAUSE_MENU) {
		if (currentLevel == 2) {
			drawLevel2();
		}
		else {
			drawLevel1();
		}
		playerMovements();
		drawParticles();
		drawFloatingTexts();
		drawHUD();
		drawPauseMenu();
		return;
	}

	if (isGameOver) {
		applyScreenShake();

		if (currentLevel == 2) {
			drawLevel2();
		}
		else {
			drawLevel1();
		}
		playerMovements();
		drawParticles();

		resetScreenShake();

		drawGameOverScreen();
		return;
	}

	if (isLevelComplete) {
		drawLevelCompleteScreen();
		return;
	}

	applyScreenShake();

	if (currentLevel == 2) {
		drawLevel2();
	}
	else {
		drawLevel1();
	}
	playerMovements();

	drawParticles();
	drawFloatingTexts();

	resetScreenShake();

	drawAlarmOverlay(isCountdownActive);

	drawHUD();
	drawUIMessage();
}

int curMouseX = -1;
int curMouseY = -1;

void iMouseMove(int mx, int my) {
	curMouseX = mx;
	curMouseY = my;
}

void iPassiveMouseMove(int mx, int my) {
	curMouseX = mx;
	curMouseY = my;
}

void iMouse(int button, int state, int mx, int my) {
	if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) {
		return;
	}

	if (isLevelComplete) {
		GameCheckpoint.clear();
		handleLevelCompleteClick(mx, my);
		return;
	}

	if (currentGameState == STATE_MAIN_MENU) {
		handleMainMenuClick(mx, my);
	}
	else if (currentGameState == STATE_LEVELS_MENU) {
		handleLevelPageClick(mx, my);
	}
	else if (currentGameState == STATE_HOW_TO_PLAY) {
		handleHowToPlayClick(mx, my);
	}
	else if (currentGameState == STATE_PAUSE_MENU) {
		handlePauseMenuClick(mx, my);
	}
	else if (currentGameState == STATE_PLAYING) {
		if (isGameOver) {
			handleGameOverClick(mx, my);
		}
		else if (mx >= 15 && mx <= 50 && my >= 805 && my <= 845) {
			playClickSound();
			restartCurrentLevel();
		}
		else if (mx >= 60 && mx <= 95 && my >= 805 && my <= 845) {
			playClickSound();
			currentGameState = STATE_PAUSE_MENU;
		}
		else if (handleSoundButtonClick(mx, my)) {
			playClickSound();
		}
	}
}

void boundaryCheck() {
	groundY = 0;
	leftWall = 0;
	rightWall = 1200;
	int ceilingY = 860;

	int midX = Player.x + (playerSize / 2);

	for (int i = Player.y + playerSize; i < 720; i++) {
		if (mat[i][midX] == 1) {
			ceilingY = i;
			break;
		}
	}

	if (Player.y + playerSize >= ceilingY && Player.vy > 0) {
		Player.y = ceilingY - playerSize;
		Player.vy = -1;
	}

	for (int i = Player.y + 10; i >= 0; i--) {
		if (mat[i][midX] == 1) {
			groundY = i;
			break;
		}
	}

	for (int i = midX; i < 1200; i++) {
		if (mat[Player.y + 15][i] == 1) {
			rightWall = i;
			break;
		}
	}

	for (int i = midX; i >= 0; i--) {
		if (mat[Player.y + 15][i] == 1) {
			leftWall = i;
			break;
		}
	}

	if (Player.x <= leftWall) {
		Player.x = leftWall;
	}
	if (Player.x >= rightWall - playerSize) {
		Player.x = rightWall - playerSize;
	}
}

void checkStairCollision() {
	int midX = Player.x + (playerSize / 2);
	int feetY = Player.y;
	int centerY = Player.y + (playerSize / 2);

	if (mat[centerY][midX] == 2 || mat[feetY][midX] == 2) {
		Player.isOnStair = true;
	}
	else {
		Player.isOnStair = false;
	}
}

void checkSwitchCollision() {
	int midX = Player.x + (playerSize / 2);
	int feetY = Player.y;
	int centerY = Player.y + (playerSize / 2);

	if (mat[feetY][midX] == 3 || mat[centerY][midX] == 3) {
		if (currentLevel == 2) {
			level2Switch.activated = true;
		}
		else {
			level1Switch.activated = true;
		}
	}
}

void countdown() {
	if (currentGameState != STATE_PLAYING || !isCountdownActive || isGameOver || isLevelComplete) {
		return;
	}

	countdownTimer--;

	if (countdownTimer <= 0) {
		isGameOver = true;
		isCountdownActive = false;
		stopAlarmSounds();
		GameController.triggerPlayerDeath();
	}
}

void fixedUpdate()
{
	GameController.update(0.025f);

	if (currentGameState == STATE_MAIN_MENU) {
		handleMainMenuController();
		return;
	}
	if (currentGameState == STATE_LEVELS_MENU) {
		handleLevelPageController();
		return;
	}
	if (currentGameState == STATE_HOW_TO_PLAY) {
		handleHowToPlayController();
		return;
	}
	if (currentGameState == STATE_PAUSE_MENU) {
		handlePauseMenuController();
		return;
	}

	if (isGameOver) {
		handleGameOverController();
		return;
	}
	if (isLevelComplete) {
		handleLevelCompleteController();
		return;
	}

	// Controller Options / Start button pauses the game during gameplay
	if (GameController.isOptionsPressed()) {
		playClickSound();
		currentGameState = STATE_PAUSE_MENU;
		return;
	}

	// Pulsating haptic feedback during alarm / whistle countdown
	GameController.setWhistlePulse(isCountdownActive);

	updateGlobalTimer();
	updateUIMessageTimer();
	updateParticles();
	updateShake();
	updateFloatingTexts();

	if (isGameOver || isLevelComplete) {
		return;
	}

	bool hitObstacle = false;
	bool hitCamera = false;
	bool hitGate = false;
	bool hitBomb = false;

	if (currentLevel == 1) {
		Player.isCarryingLoot = level1LootBox.isOpened;

		bool hitLaser = checkLevel1LaserCollision(Player.x, Player.y, playerSize - 8);
		hitBomb = checkLevel1BombCollision(Player.x, Player.y, playerSize - 8);
		bool hitCutter = checkLevel1CutterCollision(Player.x, Player.y, playerSize - 8);
		hitCamera = checkLevel1CameraCollision(Player.x, Player.y, playerSize - 8);

		hitObstacle = (hitLaser || hitBomb || hitCutter || hitCamera);
	}
	else if (currentLevel == 2) {
		Player.isCarryingLoot = level2LootBox.isOpened;

		bool hitLaser = checkLevel2LaserCollision(Player.x, Player.y, playerSize - 8);
		hitBomb = checkLevel2BombCollision(Player.x, Player.y, playerSize - 8);
		bool hitCutter = checkLevel2CutterCollision(Player.x, Player.y, playerSize - 8);
		hitCamera = checkLevel2CameraCollision(Player.x, Player.y, playerSize - 8);
		hitGate = checkLevel2GateCollision(Player.x, Player.y, playerSize - 8);

		hitObstacle = (hitLaser || hitBomb || hitCutter || hitCamera || hitGate);
	}

	if (hitObstacle) {
		spawnExplosionSmoke(Player.x + playerSize / 2, Player.y + playerSize / 2, 35);
		triggerShake(14, 20);

		if (hitBomb) {
			playBombSound();
			showUIMessage("ALERT: BLOWN UP BY A BOMB!", 150);
		}
		else if (hitCamera) {
			showUIMessage("ALERT: CAUGHT BY SECURITY CAMERA!", 150);
		}
		else if (hitGate) {
			showUIMessage("ALERT: ELECTROCUTED BY TIMED GATE!", 150);
		}

		isGameOver = true;
		isCountdownActive = false;
		stopAlarmSounds();
		GameController.triggerPlayerDeath();
		return;
	}

	if (currentLevel == 1) {
		bool atDoor = (Player.x + playerSize >= level1Door.x) && (Player.x <= level1Door.x + 60) &&
			(Player.y + playerSize >= level1Door.y) && (Player.y <= level1Door.y + 80);
		if (atDoor) {
			if (!level1LootBox.isOpened) {
				showUIMessage("OBJECTIVE: Loot the Golden Box before exiting!", 200);
			}
			else {
				isLevelComplete = true;
				isCountdownActive = false;
				stopAlarmSounds();
				GameController.stopVibration();
				reduceMusicVolumeForLevelComplete();
				spawnCoinBurst(level1Door.x + 25, level1Door.y + 35, 40);
				return;
			}
		}

		checkSwitchCollision();

		bool touchingBox = (Player.x + playerSize >= level1LootBox.x) && (Player.x <= level1LootBox.x + 40) &&
			(Player.y + playerSize >= level1LootBox.y) && (Player.y <= level1LootBox.y + 40);
		if (!level1Key.isCollected && !level1LootBox.isOpened && touchingBox) {
			showUIMessage("VAULT LOCKED: Find the Golden Key first!", 200);
		}

		bool wasBoxClosed = !level1LootBox.isOpened;
		checkLevel1ItemCollisions(Player.x, Player.y, playerSize);

		if (wasBoxClosed && level1LootBox.isOpened) {
			isCountdownActive = true;
			showUIMessage("ALARM TRIGGERED! Escape through the door now!", 400);
			playAlarmSounds();
		}

		updateLevel1Logic();
		level1Key.update();
	}
	else if (currentLevel == 2) {
		bool atDoor = (Player.x + playerSize >= level2Door.x) && (Player.x <= level2Door.x + 60) &&
			(Player.y + playerSize >= level2Door.y) && (Player.y <= level2Door.y + 80);
		if (atDoor) {
			if (!level2LootBox.isOpened) {
				showUIMessage("OBJECTIVE: Loot the Museum Vault before exiting!", 200);
			}
			else {
				isLevelComplete = true;
				isCountdownActive = false;
				stopAlarmSounds();
				GameController.stopVibration();
				reduceMusicVolumeForLevelComplete();
				spawnCoinBurst(level2Door.x + 25, level2Door.y + 35, 40);
				return;
			}
		}

		checkSwitchCollision();

		bool touchingBox = (Player.x + playerSize >= level2LootBox.x) && (Player.x <= level2LootBox.x + 40) &&
			(Player.y + playerSize >= level2LootBox.y) && (Player.y <= level2LootBox.y + 40);
		if (!level2Key.isCollected && !level2LootBox.isOpened && touchingBox) {
			showUIMessage("VAULT LOCKED: Find the Master Key on the roof!", 200);
		}

		bool wasBoxClosed = !level2LootBox.isOpened;
		checkLevel2ItemCollisions(Player.x, Player.y, playerSize);

		if (wasBoxClosed && level2LootBox.isOpened) {
			isCountdownActive = true;
			showUIMessage("ALARM TRIGGERED! Escape through the roof exit!", 400);
			playAlarmSounds();
		}

		updateLevel2Logic();
		level2Key.update();
	}

	boundaryCheck();
	checkStairCollision();
	handlePlayerInput(groundY);
}

// Level 1 initialization
void lvl1Initialize() {
	currentLevel = 1;
	totalGemsInLevel = gemCount;

	level1Lift.reset();
	level1Mat();

	updateLevel1Logic();

	GameCheckpoint.loadSpawnPoint(1, Player.x, Player.y, level1Key.isCollected, 15, 160);

	Player.vy = 0;
	Player.isJumping = false;
	Player.isOnStair = false;
	Player.isFacingRight = true;
	Player.isCarryingLoot = false;

	isGameOver = false;
	isLevelComplete = false;

	uiMessage[0] = '\0';
	uiMessageTimer = 0;

	level1LootBox.isOpened = false;
	level1Door.reset();
	level1Switch.activated = false;

	for (int i = 0; i < gemCount; i++) {
		level1Gems[i].isCollected = false;
	}

	clearParticles();
	resetShake();
	initFloatingTexts();
	resetScore();

	countdownTimer = 50;
	isCountdownActive = false;
	stopAlarmSounds();
	GameController.stopVibration();

	playLevel1Music();
}

// Level 2 initialization
void lvl2Initialize() {
	currentLevel = 2;
	totalGemsInLevel = gem2Count;

	level2Lift.reset();
	for (int i = 0; i < gate2Count; i++) {
		level2Gates[i].reset();
	}
	level2Mat();

	updateLevel2Logic();

	GameCheckpoint.loadSpawnPoint(2, Player.x, Player.y, level2Key.isCollected, 15, 160);

	Player.vy = 0;
	Player.isJumping = false;
	Player.isOnStair = false;
	Player.isFacingRight = true;
	Player.isCarryingLoot = false;

	isGameOver = false;
	isLevelComplete = false;

	uiMessage[0] = '\0';
	uiMessageTimer = 0;

	level2LootBox.isOpened = false;
	level2Door.reset();
	level2Switch.activated = false;

	for (int i = 0; i < gem2Count; i++) {
		level2Gems[i].isCollected = false;
	}

	clearParticles();
	resetShake();
	initFloatingTexts();
	resetScore();

	countdownTimer = 60;
	isCountdownActive = false;
	stopAlarmSounds();
	GameController.stopVibration();
	showUIMessage("MISSION: Reach the rooftop, secure the Master Key, then loot the vault!", 240);

	playLevel2Music();
}

void iLoadAllImages() {
	loadCharacter();

	restartBtn = iLoadImage("Images/Buttons/restartBtn.png");
	mainmenuBtn = iLoadImage("Images/Buttons/mainmenuBtn.png");

	playBtnImage = iLoadImage("Images/Buttons/playBtn.png");
	levelsBtnImage = iLoadImage("Images/Buttons/levelsBtn.png");
	howToPlayBtnImage = iLoadImage("Images/Buttons/htpBtn.png");
	exitBtnImage = iLoadImage("Images/Buttons/exitBtn.png");
	backBtnImage = iLoadImage("Images/Buttons/backBtn.png");
	level1Btn = iLoadImage("Images/Buttons/level1Btn.png");
	level2Btn = iLoadImage("Images/Buttons/level2Btn.png");

	redImage = iLoadImage("Images/red.png");
	levelOne = iLoadImage("Images/Background/LevelOne.png");
	lvl2BgImage = iLoadImage("Images/Background/bgLvl2.png");
	
	menubackgroundImage = iLoadImage("Images/Background/MenuBackground.png");
	backgroundImage = iLoadImage("Images/Background/Background.png");
	lvlCompleteImage = iLoadImage("Images/Background/lvlComplete.png");
	stairImage = iLoadImage("Images/Utility/Stairs.png");
	laserImage = iLoadImage("Images/Obstacle/RedLaser.png");
	platformLvl1Image = iLoadImage("Images/Utility/PlatformLvl1.png");
	platformLvl2Image = iLoadImage("Images/Utility/PlatformLvl2.png");
	platformImage = (platformLvl1Image > 0) ? platformLvl1Image : iLoadImage("Images/Utility/Platform.png");
	liftPlatformImage = iLoadImage("Images/Utility/LiftPlatform.png");
	redSwitchImage = iLoadImage("Images/Utility/RedSwitch.png");
	greenSwitchImage = iLoadImage("Images/Utility/GreenSwitch.png");
	keyImage = iLoadImage("Images/Utility/Key.png");
	closedBoxImage = iLoadImage("Images/Utility/ClosedBox.png");
	openedBoxImage = iLoadImage("Images/Utility/OpenedBox.png");
	closedDoorImage = iLoadImage("Images/Utility/ClosedDoor.png");
	openedDoorImage = iLoadImage("Images/Utility/OpenedDoor.png");
	bompImage = iLoadImage("Images/Obstacle/Bomp.png");
	restartBtnImage = iLoadImage("Images/Utility/Restart.png");
	menuBtnImage = iLoadImage("Images/Utility/Menu.png");
	soundOnImage = iLoadImage("Images/Utility/SoundOn.png");
	soundOffImage = iLoadImage("Images/Utility/SoundOff.png");

	cutterImages[0] = iLoadImage("Images/Obstacle/Cutter1.png");
	cutterImages[1] = iLoadImage("Images/Obstacle/Cutter2.png");
	cutterImages[2] = iLoadImage("Images/Obstacle/Cutter3.png");

	cashImage = iLoadImage("Images/Utility/Cash.png");

	for (int i = 0; i < 10; i++) {
		char escapePath[64];
		sprintf_s(escapePath, sizeof(escapePath), "Images/Escape_bar/escape_bar_%02d.png", i + 1);
		escapeBarImages[i] = iLoadImage(escapePath);
	}
}

int main()
{
	initAudio();

	iInitialize(1200, 860, "Grand Theft");

	iLoadAllImages();

	// Ensure game starts cleanly on the Main Menu without active checkpoints
	GameCheckpoint.clear();
	currentGameState = STATE_MAIN_MENU;

	iSetTimer(30, fixedUpdate);
	iSetTimer(1000, countdown);

	iStart();
	return 0;
}