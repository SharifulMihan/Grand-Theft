#include <cassert>
#include <cstdio>

#define main grandTheftMain
#include "../iMain.cpp"
#undef main

void releaseInputs() {
	memset(keyPressed, 0, sizeof(keyPressed));
	memset(specialKeyPressed, 0, sizeof(specialKeyPressed));
}

void step(int frames) {
	for (int i = 0; i < frames; i++) {
		fixedUpdate();
		assert(!isGameOver);
	}
}

void enterPortal(int x, int y) {
	releaseInputs();
	Player.x = x;
	Player.y = y;
	Player.isJumping = false;
	Player.isOnStair = false;
	Player.vy = 0;
	step(30);
	keyPressed['e'] = 1;
	step(1);
}

void testPortalProgression() {
	startFreshLevel(3);
	assert(currentLevel == 3 && level3Room == PORTAL_VAULT);
	assert(!level3Key.isCollected && !level3LootBox.isOpened);
	assert(!isCountdownActive && playerScore == 0);

	checkLevel3ItemCollisions(level3Key.x, (int)level3Key.baseY, playerSize);
	assert(!level3Key.isCollected);
	checkLevel3ItemCollisions(level3LootBox.x, level3LootBox.y, playerSize);
	assert(!level3LootBox.isOpened && !level3Door.isOpened);

	enterPortal(1040, 320);
	assert(level3Room == KEY_CHAMBER && Player.x == 190 && Player.y == 150);
	assert(GameCheckpoint.isActive && !level3Key.isCollected);
	Player.x = 80;
	step(60);
	assert(level3Room == KEY_CHAMBER);
	restartCurrentLevel();
	assert(level3Room == KEY_CHAMBER && !level3Key.isCollected);
	assert(Player.x == 190 && Player.y == 150);

	enterPortal(80, 150);
	assert(level3Room == PORTAL_VAULT && !level3Key.isCollected);
	enterPortal(1040, 320);
	checkLevel3ItemCollisions(260, 150, playerSize);
	assert(gemsCollected == 1 && playerScore == 200);
	checkLevel3ItemCollisions(600, 320, playerSize);
	for (int i = 0; i < 80; i++) updateLevel3Logic();
	assert(level3Switch.activated && level3Lift.y == level3Lift.targetY);
	checkLevel3ItemCollisions(1080, 340, playerSize);
	assert(level3Key.isCollected && !level3LootBox.isOpened);
	assert(!isCountdownActive);
	assert(!checkLevel3ObstacleCollision(350, 160, playerSize));

	restartCurrentLevel();
	assert(level3Room == KEY_CHAMBER && level3Key.isCollected);
	assert(level3Switch.activated && level3Lift.y == level3Lift.targetY);
	assert(gemsCollected == 1 && playerScore == 200);
	enterPortal(80, 150);
	assert(level3Room == PORTAL_VAULT && level3Key.isCollected);
	assert(!checkLevel3ObstacleCollision(255, 150, playerSize));
	enterPortal(1040, 320);
	checkLevel3ItemCollisions(260, 150, playerSize);
	assert(gemsCollected == 1 && playerScore == 200);
	enterPortal(80, 150);

	releaseInputs();
	Player.x = 460;
	Player.y = 150;
	step(1);
	assert(level3LootBox.isOpened && level3Door.isOpened && isCountdownActive);
	assert(playerScore == 1200 && countdownTimer == level3EscapeTime);
	level3PortalCooldown = 0;
	level3PortalHeld = false;
	int x = 1040, y = 320;
	assert(!useLevel3Portal(x, y, playerSize, true));
	countdown();
	assert(countdownTimer == level3EscapeTime - 1);
	currentGameState = STATE_PAUSE_MENU;
	int gateTimer = level3Gate.timer;
	countdown();
	fixedUpdate();
	assert(countdownTimer == level3EscapeTime - 1 && level3Gate.timer == gateTimer);
	currentGameState = STATE_PLAYING;
	while (!isGameOver) countdown();
	restartCurrentLevel();
	assert(level3Room == PORTAL_VAULT && level3Key.isCollected && level3LootBox.isOpened);
	assert(Player.x == 510 && Player.y == 150);
	assert(playerScore == 1200 && countdownTimer == level3EscapeTime);

	keyPressed['d'] = 1;
	step(310);
	assert(isLevelComplete && !isCountdownActive && playerScore == 1200);
	releaseInputs();
	startFreshLevel(3);
	assert(level3Room == PORTAL_VAULT && Player.x == 60);
	assert(!level3Key.isCollected && !level3Switch.activated && !level3LootBox.isOpened);
	assert(!isLevelComplete && !isCountdownActive && playerScore == 0 && gemsCollected == 0);
	for (int i = 0; i < gem3Count; i++) assert(!level3Gems[i].isCollected);
}

void testMenuAndPreviousLevels() {
	handleLevelPageClick(280, 365);
	assert(currentLevel == 3 && currentGameState == STATE_PLAYING);
	currentGameState = STATE_PAUSE_MENU;
	handlePauseMenuClick(600, 460);
	assert(currentLevel == 3 && currentGameState == STATE_PLAYING);
	handleLevelPageClick(280, 505);
	assert(currentLevel == 1 && !level1Key.isCollected);
	restartCurrentLevel();
	assert(currentLevel == 1);
	handleLevelPageClick(280, 435);
	assert(currentLevel == 2 && !level2Key.isCollected);
	restartCurrentLevel();
	assert(currentLevel == 2);
	handleLevelPageClick(280, 365);
	assert(currentLevel == 3 && !level3Key.isCollected);
}

int main() {
	testPortalProgression();
	testMenuAndPreviousLevels();
	puts("Level 3 progression, checkpoint, timer, menu and regression checks passed.");
	return 0;
}
