#include "iGraphics.h"
#include <stdio.h>

#include "utility.hpp"
#include "Character.hpp"
#include "Checkpoint.hpp"
#include "lvl1.hpp"
#include "lvl2.hpp"
#include "lvl3.hpp"
#include "UI.hpp"
#include "MainMenu.hpp"
#include "MainMenuAnimation.hpp"
#include "MenuPage.hpp"
#include "FileHandler.hpp"
#include "NameWindow.hpp"

int PlayerX_For_Parallax = 0;

int currentLevel = 1;

bool playerInSubLevel = false;

bool reversedGravity = false;

int lvl2BgImage = 0;
int lvl3BgImage = 0;
int bgSubLvl3Image = 0;

extern const bool ENABLE_SUBLEVEL_DIRECTLY;
extern const int LEVEL3_SPAWN_X;
extern const int LEVEL3_SPAWN_Y;
extern const int SUBLEVEL_SPAWN_X;
extern const int SUBLEVEL_SPAWN_Y;

void lvl1Initialize();
void lvl2Initialize();
void lvl3Initialize();

void startFreshLevel(int levelNum) {
	GameCheckpoint.clear();
	if (levelNum == 1) {
		lvl1Initialize();
	}
	else if (levelNum == 2) {
		lvl2Initialize();
	}
	else if (levelNum == 3) {
		lvl3Initialize();
	}
	currentGameState = STATE_PLAYING;
}


void goToMainMenu() {
	GameCheckpoint.clear();
	stopAlarmSounds();
	stopLevelMusic();
	GameController.stopVibration();
	currentGameState = STATE_MAIN_MENU;
}


GameState currentGameState = STATE_STARTUP_SPLASH;

char uiMessage[128] = "";
int uiMessageTimer = 0;


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
int platformLvl3HoriImage = 0;
int platformLvl3VertiImage = 0;
int platformSubLvl3HoriImage = 0;
int platformSubLvl3VertiImage = 0;
int liftPlatformImage;
int redSwitchImage;
int greenSwitchImage;
int keyImage;
int keyReImage; // Reversed key for sublevel
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
int portalImage = 0;
int portalReImage = 0;
int closedGateImage = 0;
int openedGateImage = 0;
int cutterImages[3];
int cashImage = 0;
int escapeBarImages[10] = { 0 };
ControllerManager GameController;


int playBtnImage = 0;
int levelsBtnImage = 0;
int howToPlayBtnImage = 0;
int exitBtnImage = 0;
int backBtnImage = 0;
int scoreboardBtnImage = 0;
int aboutBtnImage = 0;
int aboutPageImage = 0;

int nameWindowImage = 0;
int namePlateImage = 0;
int submitBtnImage = 0;
int cancelBtnImage = 0;


int scoreboardBgImage = 0;

int restartBtn = 0;
int mainmenuBtn = 0;


int iGraphicsLogoImage = 0;
int shorovujStudioLogoImage = 0;
int startupSplashStartedAtMs = 0;
bool introSoundPlayed = false;
const int SPLASH_FADE_DURATION_MS = 700;
const int SPLASH_HOLD_DURATION_MS = 1100;
const int SPLASH_LOGO_DURATION_MS = (SPLASH_FADE_DURATION_MS * 2) + SPLASH_HOLD_DURATION_MS;

float getSplashAlpha(int elapsedMs) {
	if (elapsedMs < SPLASH_FADE_DURATION_MS) {
		return (float)elapsedMs / SPLASH_FADE_DURATION_MS;
	}

	if (elapsedMs < SPLASH_FADE_DURATION_MS + SPLASH_HOLD_DURATION_MS) {
		return 1.0f;
	}

	return 1.0f - (float)(elapsedMs - SPLASH_FADE_DURATION_MS - SPLASH_HOLD_DURATION_MS) / SPLASH_FADE_DURATION_MS;
}

void drawFadingSplashLogo(int image, float alpha) {
	const int logoWidth = 1000;
	const int logoHeight = 575;
	const int logoX = (1200 - logoWidth) / 2;
	const int logoY = (860 - logoHeight) / 2;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glColor4f(1.0f, 1.0f, 1.0f, alpha);

	glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex2f(logoX, logoY);
		glTexCoord2f(1, 0); glVertex2f(logoX + logoWidth, logoY);
		glTexCoord2f(1, -1); glVertex2f(logoX + logoWidth, logoY + logoHeight);
		glTexCoord2f(0, -1); glVertex2f(logoX, logoY + logoHeight);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void drawStartupSplash() {
	const int elapsedMs = glutGet(GLUT_ELAPSED_TIME) - startupSplashStartedAtMs;
	const int splashSequenceDurationMs = SPLASH_LOGO_DURATION_MS * 2;

	if (elapsedMs < 50 && !introSoundPlayed) {
		playIntroSound();
		introSoundPlayed = true;
	}

	if (elapsedMs >= splashSequenceDurationMs) {
		stopIntroSound();
		currentGameState = STATE_MAIN_MENU;
		return;
	}

	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	iFilledRectangle(0, 0, 1200, 860);

	const bool showIGraphicsLogo = elapsedMs < SPLASH_LOGO_DURATION_MS;
	const int logoElapsedMs = showIGraphicsLogo ? elapsedMs : elapsedMs - SPLASH_LOGO_DURATION_MS;
	drawFadingSplashLogo(showIGraphicsLogo ? iGraphicsLogoImage : shorovujStudioLogoImage,
		getSplashAlpha(logoElapsedMs));
}


int level1Btn = 0;
int level2Btn = 0;
int level3Btn = 0;


int groundY = 250;
int leftWall = 0;
int rightWall = 1200;


int mat[720][1200] = { 0 };


bool isGameOver = false;
bool isLevelComplete = false;

int countdownTimer = 15;
bool isCountdownActive = false;


void iDraw()
{
	iClear();

	if (currentGameState == STATE_STARTUP_SPLASH) {
		drawStartupSplash();
		return;
	}

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

	if (currentGameState == STATE_USERNAME_INPUT) {
		drawUsernameInput();
		return;
	}

	if (currentGameState == STATE_SCOREBOARD) {
		drawScoreboardPage();
		return;
	}

	if (currentGameState == STATE_ABOUT) {
		drawAboutPage();
		return;
	}

	if (currentGameState == STATE_PAUSE_MENU) {
		if (currentLevel == 3) {
			drawLevel3();
		}
		else if (currentLevel == 2) {
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

		if (currentLevel == 3) {
			drawLevel3();
		}
		else if (currentLevel == 2) {
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

	if (currentLevel == 3) {
		drawLevel3();
	}
	else if (currentLevel == 2) {
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

void iKeyboard(unsigned char key, int mx, int my) {

}

void iSpecialKeyboard(unsigned char key, int mx, int my) {
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
	else if (currentGameState == STATE_USERNAME_INPUT) {
		handleUsernameInputClick(mx, my);
	}
	else if (currentGameState == STATE_SCOREBOARD) {
		handleScoreboardClick(mx, my);
	}
	else if (currentGameState == STATE_ABOUT) {
		handleAboutClick(mx, my);
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

	if (reversedGravity) {
		for (int i = Player.y + playerSize; i < 720; i++) {
			if (mat[i][midX] == 1) {
				groundY = i;
				break;
			}
		}
		
		for (int i = Player.y; i >= 0; i--) {
			if (mat[i][midX] == 1) {
				ceilingY = i;
				break;
			}
		}
		
		if (Player.vy > 0 && Player.y + playerSize >= groundY && Player.y + playerSize <= groundY + 10) {
			Player.y = groundY - playerSize;
			Player.vy = 0;
			Player.isJumping = false;
		}
		
		if (Player.vy < 0 && Player.y <= ceilingY + playerSize && Player.y >= ceilingY - 10) {
			Player.y = ceilingY + playerSize;
			Player.vy = 0;
		}
	} else {
		for (int i = Player.y + playerSize; i < 720; i++) {
			if (mat[i][midX] == 1) {
				ceilingY = i;
				break;
			}
		}


		if (Player.vy > 0 && Player.y + playerSize >= ceilingY && Player.y + playerSize <= ceilingY + 10) {
			Player.y = ceilingY - playerSize;
			Player.vy = -1;
		}

		for (int i = Player.y + 10; i >= 0; i--) {
			if (mat[i][midX] == 1) {
				groundY = i;
				break;
			}
		}
		

		if (Player.vy < 0 && Player.y <= groundY && Player.y >= groundY - 10) {
			Player.y = groundY;
			Player.vy = 0;
			Player.isJumping = false;
		}
	}

	for (int i = midX; i < 1200; i++) {
		int checkY = reversedGravity ? Player.y + playerSize / 2 : Player.y + 15;
		if (mat[checkY][i] == 1) {
			rightWall = i;
			break;
		}
	}

	for (int i = midX; i >= 0; i--) {
		int checkY = reversedGravity ? Player.y + playerSize / 2 : Player.y + 15;
		if (mat[checkY][i] == 1) {
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
	int lowerY = Player.y + playerSize / 4;
	int centerY = Player.y + (playerSize / 2);
	int headY = Player.y + playerSize;


	if (midX < 0 || midX >= 1200) {
		Player.isOnStair = false;
		return;
	}

	if (reversedGravity) {
		bool onStair = false;
		if (headY >= 0 && headY < 720 && mat[headY][midX] == 2) onStair = true;
		if (centerY >= 0 && centerY < 720 && mat[centerY][midX] == 2) onStair = true;
		Player.isOnStair = onStair;
	} else {
		bool onStair = false;
		if (feetY >= 0 && feetY < 720 && mat[feetY][midX] == 2) onStair = true;
		if (lowerY >= 0 && lowerY < 720 && mat[lowerY][midX] == 2) onStair = true;
		if (centerY >= 0 && centerY < 720 && mat[centerY][midX] == 2) onStair = true;

		int aboveFeet = feetY + 5;
		if (aboveFeet >= 0 && aboveFeet < 720 && mat[aboveFeet][midX] == 2) onStair = true;
		Player.isOnStair = onStair;
	}
}

void checkSwitchCollision() {
	int midX = Player.x + (playerSize / 2);
	int feetY = Player.y;
	int centerY = Player.y + (playerSize / 2);

	if (mat[feetY][midX] == 3 || mat[centerY][midX] == 3) {
		if (currentLevel == 3) {
			level3Switch.activated = true;
		}
		else if (currentLevel == 2) {
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


	if (currentGameState == STATE_STARTUP_SPLASH) {
		return;
	}

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
	if (currentGameState == STATE_USERNAME_INPUT) {
		handleUsernameInput(); // Handle keyboard input (polling-based)
		handleUsernameInputController(); // Handle controller input
		return;
	}
	if (currentGameState == STATE_SCOREBOARD) {
		handleScoreboardController();
		return;
	}
	if (currentGameState == STATE_ABOUT) {
		handleAboutController();
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


	if (GameController.isOptionsPressed()) {
		playClickSound();
		currentGameState = STATE_PAUSE_MENU;
		return;
	}

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
	else if (currentLevel == 3) {
		Player.isCarryingLoot = level3LootBox.isOpened;

		bool hitLaser = checkLevel3LaserCollision(Player.x, Player.y, playerSize - 8);
		hitBomb = checkLevel3BombCollision(Player.x, Player.y, playerSize - 8);
		bool hitCutter = checkLevel3CutterCollision(Player.x, Player.y, playerSize - 8);
		hitCamera = checkLevel3CameraCollision(Player.x, Player.y, playerSize - 8);

		hitObstacle = (hitLaser || hitBomb || hitCutter || hitCamera);
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
		incrementDeathCount();
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
				updatePlayerScore(1, getCurrentDeathCount());
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
			GameController.triggerLootBoxLooted();
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
				updatePlayerScore(2, getCurrentDeathCount());
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
			GameController.triggerLootBoxLooted();
			isCountdownActive = true;
			showUIMessage("ALARM TRIGGERED! Escape through the roof exit!", 400);
			playAlarmSounds();
		}

		updateLevel2Logic();
		level2Key.update();
	}
	else if (currentLevel == 3) {
		bool atDoor = (Player.x + playerSize >= level3Door.x) && (Player.x <= level3Door.x + 60) &&
			(Player.y + playerSize >= level3Door.y) && (Player.y <= level3Door.y + 80);
		if (atDoor) {
			if (!level3LootBox.isOpened) {
				showUIMessage("OBJECTIVE: Loot the Vault before exiting!", 200);
			}
			else {
				isLevelComplete = true;
				isCountdownActive = false;
				stopAlarmSounds();
				GameController.stopVibration();
				reduceMusicVolumeForLevelComplete();
				spawnCoinBurst(level3Door.x + 25, level3Door.y + 35, 40);
				updatePlayerScore(3, getCurrentDeathCount());
				return;
			}
		}

		checkSwitchCollision();

		bool touchingBox = (Player.x + playerSize >= level3LootBox.x) && (Player.x <= level3LootBox.x + 40) &&
			(Player.y + playerSize >= level3LootBox.y) && (Player.y <= level3LootBox.y + 40);
		if (!level3Key.isCollected && !level3LootBox.isOpened && touchingBox) {
			showUIMessage("VAULT LOCKED: Find the Key in the Dimension Rift!", 200);
		}

		bool wasBoxClosed = !level3LootBox.isOpened;
		checkLevel3ItemCollisions(Player.x, Player.y, playerSize);

		if (wasBoxClosed && level3LootBox.isOpened) {
			GameController.triggerLootBoxLooted();
			isCountdownActive = true;
			showUIMessage("ALARM TRIGGERED! Escape through the door now!", 400);
			playAlarmSounds();
		}

		updateLevel3Logic();
		if (playerInSubLevel) {
			level3Key.update();
		}
	}

	boundaryCheck();
	checkStairCollision();
	handlePlayerInput(groundY);
}


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

	countdownTimer = 60;
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

// Level 3 initialization
void lvl3Initialize() {
	currentLevel = 3;
	totalGemsInLevel = gem3Count;

	// Check if there's a sublevel checkpoint first
	bool checkpointInSubLevel = GameCheckpoint.getSubLevelState();
	
	// Check development mode variable for direct sublevel access
	if (ENABLE_SUBLEVEL_DIRECTLY) {
		playerInSubLevel = true;
		reversedGravity = true;
		Player.gravity = 0.3f;
	} else if (checkpointInSubLevel) {
		// Use checkpoint state if available
		playerInSubLevel = true;
		reversedGravity = true;
		Player.gravity = 0.3f;
	} else {
		playerInSubLevel = false;
		reversedGravity = false;
		Player.gravity = -0.3f;
	}


	if (playerInSubLevel) {
		level3SubMat();
	} else {
		level3Lift.reset();
		level3MainMat();
	}

	updateLevel3Logic();

	int px, py;
	extern const int LEVEL3_SPAWN_X;
	extern const int LEVEL3_SPAWN_Y;
	extern const int SUBLEVEL_SPAWN_X;
	extern const int SUBLEVEL_SPAWN_Y;
	extern const int PORTAL_TO_SUBLEVEL_X;
	extern const int PORTAL_TO_SUBLEVEL_Y;
	
	if (ENABLE_SUBLEVEL_DIRECTLY) {
		GameCheckpoint.loadSpawnPoint(3, px, py, level3Key.isCollected, SUBLEVEL_SPAWN_X, SUBLEVEL_SPAWN_Y);
	} else if (checkpointInSubLevel) {
		GameCheckpoint.loadSpawnPoint(3, px, py, level3Key.isCollected, PORTAL_TO_SUBLEVEL_X, PORTAL_TO_SUBLEVEL_Y);
	} else {
		GameCheckpoint.loadSpawnPoint(3, px, py, level3Key.isCollected, LEVEL3_SPAWN_X, LEVEL3_SPAWN_Y);
	}
	Player.x = px;
	Player.y = py;

	Player.vy = 0;
	Player.isJumping = false;
	Player.isOnStair = false;
	Player.isFacingRight = true;
	Player.isCarryingLoot = false;

	isGameOver = false;
	isLevelComplete = false;

	uiMessage[0] = '\0';
	uiMessageTimer = 0;

	level3LootBox.isOpened = false;
	level3Door.reset();
	level3Switch.activated = false;

	for (int i = 0; i < gem3Count; i++) {
		level3Gems[i].isCollected = false;
	}

	clearParticles();
	resetShake();
	initFloatingTexts();
	resetScore();

	countdownTimer = 40;
	isCountdownActive = false;
	stopAlarmSounds();
	GameController.stopVibration();
	showUIMessage("MISSION: Enter the Dimension Rift, find the Key, then loot the Vault!", 240);

	playLevel2Music(); // Using level 2 music for level 3
}



void iLoadAllImages() {
	loadCharacter();
	loadMenuCharacterAnimation();

	iGraphicsLogoImage = iLoadImage("Images/iGraphics.png");
	shorovujStudioLogoImage = iLoadImage("Images/ShorovujStudio.png");

	restartBtn = iLoadImage("Images/Buttons/restartBtn.png");
	mainmenuBtn = iLoadImage("Images/Buttons/mainmenuBtn.png");

	playBtnImage = iLoadImage("Images/Buttons/playBtn.png");
	levelsBtnImage = iLoadImage("Images/Buttons/levelsBtn.png");
	howToPlayBtnImage = iLoadImage("Images/Buttons/htpBtn.png");
	exitBtnImage = iLoadImage("Images/Buttons/exitBtn.png");
	backBtnImage = iLoadImage("Images/Buttons/backBtn.png");
	level1Btn = iLoadImage("Images/Buttons/level1Btn.png");
	level2Btn = iLoadImage("Images/Buttons/level2Btn.png");
	level3Btn = iLoadImage("Images/Buttons/level3Btn.png");
	scoreboardBtnImage = iLoadImage("Images/Buttons/ScoreBoard.png");
	aboutBtnImage = iLoadImage("Images/Buttons/About.png");
	aboutPageImage = iLoadImage("Images/AboutPage.jpg");


	nameWindowImage = iLoadImage("Images/Background/NameWindow.jpg");
	namePlateImage = iLoadImage("Images/Background/NamePlate.png");
	submitBtnImage = iLoadImage("Images/Buttons/Submit.png");
	cancelBtnImage = iLoadImage("Images/Buttons/Cancel.png");


	scoreboardBgImage = iLoadImage("Images/Background/bgScoreBoard.jpg");

	redImage = iLoadImage("Images/red.png");
	levelOne = iLoadImage("Images/Background/LevelOne.png");
	lvl2BgImage = iLoadImage("Images/Background/bgLvl2.png");
	lvl3BgImage = iLoadImage("Images/Background/bgLvl3.png");
	bgSubLvl3Image = iLoadImage("Images/Background/bgSubLvl3.png");
	
	menubackgroundImage = iLoadImage("Images/Background/MenuBackground.png");
	backgroundImage = iLoadImage("Images/Background/Background.png");
	lvlCompleteImage = iLoadImage("Images/Background/lvlComplete.png");
	stairImage = iLoadImage("Images/Utility/Stairs.png");
	laserImage = iLoadImage("Images/Obstacle/RedLaser.png");
	platformLvl1Image = iLoadImage("Images/Utility/PlatformLvl1.png");
	platformLvl2Image = iLoadImage("Images/Utility/PlatformLvl2.png");
	platformLvl3HoriImage = iLoadImage("Images/Utility/PlatformLvl3Hori.png");
	platformLvl3VertiImage = iLoadImage("Images/Utility/PlatformLvl3Verti.png");
	platformSubLvl3HoriImage = iLoadImage("Images/Utility/PlatformSubLvl3Hori.png");
	platformSubLvl3VertiImage = iLoadImage("Images/Utility/PlatformSubLvl3Verti.png");
	platformImage = (platformLvl1Image > 0) ? platformLvl1Image : iLoadImage("Images/Utility/Platform.png");
	liftPlatformImage = iLoadImage("Images/Utility/LiftPlatform.png");
	redSwitchImage = iLoadImage("Images/Utility/RedSwitch.png");
	greenSwitchImage = iLoadImage("Images/Utility/GreenSwitch.png");
	keyImage = iLoadImage("Images/Utility/Key.png");
	keyReImage = iLoadImage("Images/Utility/KeyRe.png");
	closedBoxImage = iLoadImage("Images/Utility/ClosedBox.png");
	openedBoxImage = iLoadImage("Images/Utility/OpenedBox.png");
	closedDoorImage = iLoadImage("Images/Utility/ClosedDoor.png");
	openedDoorImage = iLoadImage("Images/Utility/OpenedDoor.png");
	bompImage = iLoadImage("Images/Obstacle/Bomp.png");
	restartBtnImage = iLoadImage("Images/Utility/Restart.png");
	menuBtnImage = iLoadImage("Images/Utility/Menu.png");
	soundOnImage = iLoadImage("Images/Utility/SoundOn.png");
	soundOffImage = iLoadImage("Images/Utility/SoundOff.png");
	portalImage = iLoadImage("Images/Utility/Portal.png");
	portalReImage = iLoadImage("Images/Utility/PortalRe.png");
	closedGateImage = iLoadImage("Images/Utility/ClosedGate.png");
	openedGateImage = iLoadImage("Images/Utility/OpenedGate.png");

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

	loadScoreboard();

	GameCheckpoint.clear();
	introSoundPlayed = false;
	currentGameState = STATE_STARTUP_SPLASH;
	startupSplashStartedAtMs = glutGet(GLUT_ELAPSED_TIME);

	iSetTimer(30, fixedUpdate);
	iSetTimer(1000, countdown);

	iStart();
	return 0;
}
