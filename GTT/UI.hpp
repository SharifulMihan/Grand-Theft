#ifndef UI_HPP
#define UI_HPP

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "utility.hpp"
#include "lvl1.hpp"
#include "lvl2.hpp"

extern char uiMessage[128];
extern int uiMessageTimer;

extern int redImage;
extern int keyImage;
extern int closedBoxImage;
extern int openedBoxImage;
extern int countdownTimer;
extern bool isCountdownActive;
extern bool isGameOver;
extern bool isLevelComplete;
extern int levelOne;
extern int lvlCompleteImage;

void lvl1Initialize();

// Opens the sound files
inline void initAudio() {
	mciSendString("open \"Sound//Level1BG.mp3\" alias level1song", NULL, 0, NULL);
	mciSendString("open \"Sound//Level2BG.mp3\" alias level2song", NULL, 0, NULL);
	mciSendString("open \"Sound//Whistle.mp3\" alias whistlesong", NULL, 0, NULL);
	mciSendString("open \"Sound//jump.mp3\" alias jumpsound", NULL, 0, NULL);
	mciSendString("open \"Sound//hover.mp3\" alias hoversound", NULL, 0, NULL);
	mciSendString("open \"Sound//click.mp3\" alias clicksound", NULL, 0, NULL);
}

// Ensures SoundOn and SoundOff UI images are loaded
inline void loadSoundUIImages() {
	if (soundOnImage <= 0) {
		soundOnImage = iLoadImage("Images/Utility/SoundOn.png");
	}
	if (soundOffImage <= 0) {
		soundOffImage = iLoadImage("Images/Utility/SoundOff.png");
	}
}

// Sets volume (0 to 1000) for a given sound alias
inline void setMusicVolume(const char* alias, int volume = 1000) {
	char cmd[80];
	sprintf_s(cmd, sizeof(cmd), "setaudio %s volume to %d", alias, volume);
	mciSendString(cmd, NULL, 0, NULL);
}

// Stops whichever level track(s) might be playing (safe to call anytime)
inline void stopLevelMusic() {
	mciSendString("stop level1song", NULL, 0, NULL);
	mciSendString("stop level2song", NULL, 0, NULL);
}

// Plays Level 1 background music (Level1BG.mp3) at full volume
inline void playLevel1Music() {
	stopLevelMusic();
	if (!isSoundOn) return;
	mciSendString("play level1song repeat", NULL, 0, NULL);
	setMusicVolume("level1song", 1000);
}

// Plays Level 2 background music (Level2BG.mp3) at full volume
inline void playLevel2Music() {
	stopLevelMusic();
	if (!isSoundOn) return;
	mciSendString("play level2song repeat", NULL, 0, NULL);
	setMusicVolume("level2song", 1000);
}

// Resumes or plays the current level's background music if sound is enabled
inline void playCurrentLevelMusic() {
	if (!isSoundOn) return;
	if (currentLevel == 2) {
		playLevel2Music();
	}
	else {
		playLevel1Music();
	}
}

// Toggles sound on/off and updates background music
inline void toggleSound() {
	isSoundOn = !isSoundOn;
	if (isSoundOn) {
		playCurrentLevelMusic();
	}
	else {
		stopLevelMusic();
	}
}

// Handles click on sound toggle button in HUD
inline bool handleSoundButtonClick(int mx, int my) {
	if (mx >= 105 && mx <= 140 && my >= 805 && my <= 845) {
		toggleSound();
		return true;
	}
	return false;
}

// Decreases the currently active level's music volume to 60% for the level complete screen
inline void reduceMusicVolumeForLevelComplete() {
	if (!isSoundOn) return;
	const char* alias = (currentLevel == 2) ? "level2song" : "level1song";
	char cmd[64];
	sprintf_s(cmd, sizeof(cmd), "play %s repeat", alias);
	mciSendString(cmd, NULL, 0, NULL);
	setMusicVolume(alias, 600); // 60% volume
}

inline void stopAlarmSounds() {
	mciSendString("stop ticksong", NULL, 0, NULL);
	mciSendString("stop whistlesong", NULL, 0, NULL);
}

inline void playAlarmSounds() {
	mciSendString("play whistlesong repeat", NULL, 0, NULL);
}

inline void playJumpSound(){
	mciSendString("play jumpsound from 0", NULL, 0, NULL);
}
// shows text message
inline void showUIMessage(const char* text, int durationFrames = 75) {
	strcpy_s(uiMessage, sizeof(uiMessage), text);
	uiMessageTimer = durationFrames;
}

// Counts the message timer down to 0, then clears the message
inline void updateUIMessageTimer() {
	if (uiMessageTimer > 0) {
		uiMessageTimer--;
		if (uiMessageTimer == 0) {
			uiMessage[0] = '\0';
		}
	}
}

// Styled static hint and mission message banner (no blinking)
inline void drawUIMessage() {
	if (uiMessageTimer > 0 && uiMessage[0] != '\0') {
		int textLen = (int)strlen(uiMessage);
		int msgW = textLen * 10 + 60;
		if (msgW < 340) msgW = 340;
		if (msgW > 850) msgW = 850;
		int msgH = 46;
		int msgX = (1200 - msgW) / 2;
		int msgY = 100;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// 1. Sleek dark navy translucent card
		glColor4f(0.06f, 0.09f, 0.18f, 0.90f);
		iFilledRectangle(msgX, msgY, msgW, msgH);

		// 2. Solid gold/cyan border (no blinking)
		glColor4f(1.0f, 0.85f, 0.0f, 0.95f);
		iRectangle(msgX, msgY, msgW, msgH);
		iRectangle(msgX + 2, msgY + 2, msgW - 4, msgH - 4);

		// 3. Cyan decorative side accent
		glColor4f(0.0f, 0.85f, 1.0f, 0.95f);
		iFilledRectangle(msgX + 4, msgY + 4, 6, msgH - 8);

		glDisable(GL_BLEND);

		// 4. Bright, crisp hint text
		iSetColor(255, 235, 120);
		iText(msgX + 22, msgY + 14, uiMessage, GLUT_BITMAP_HELVETICA_18);
	}
}

bool act = true;
// Draws the buttons/icons/text 
inline void drawHUD() {
	bool hasKey = (currentLevel == 2) ? level2Key.isCollected : level1Key.isCollected;
	bool hasLoot = (currentLevel == 2) ? level2LootBox.isOpened : level1LootBox.isOpened;

	loadSoundUIImages();

	// Action buttons (Restart, Menu, Sound)
	iShowImage(15, 815, 30, 30, restartBtnImage);
	iShowImage(60, 815, 30, 30, menuBtnImage);
	iShowImage(105, 815, 30, 30, isSoundOn ? soundOnImage : soundOffImage);

	// Key Status Icon & text
	iShowImage(380, 812, 30, 30, keyImage);
	if (hasKey) {
		iSetColor(0, 255, 120);
		iText(418, 820, "KEY: FOUND", GLUT_BITMAP_HELVETICA_18);
	}
	else {
		iSetColor(180, 180, 180);
		iText(418, 820, "KEY: NEEDED", GLUT_BITMAP_HELVETICA_18);
	}

	// Lootbox Status Icon & text
	if (hasLoot) {
		iShowImage(560, 810, 35, 35, openedBoxImage);
		iSetColor(255, 215, 0);
		iText(602, 820, "LOOT: SECURED!", GLUT_BITMAP_HELVETICA_18);
	}
	else {
		iShowImage(560, 810, 35, 35, closedBoxImage);
		iSetColor(180, 180, 180);
		iText(602, 820, "LOOT: LOCKED", GLUT_BITMAP_HELVETICA_18);
	}

extern int escapeBarImages[10];

	// Score & Cash Display
	iSetColor(0, 220, 255);
	char scoreBuf[40];
	sprintf_s(scoreBuf, "SCORE: %d", playerScore);
	iText(760, 820, scoreBuf, GLUT_BITMAP_HELVETICA_18);

	// Escape Countdown Timer with visual image progress bar
	if (isCountdownActive) {
		int maxTime = (currentLevel == 2) ? 60 : 50;
		double timerRatio = (double)countdownTimer / (double)maxTime;
		if (timerRatio < 0.0) timerRatio = 0.0;
		if (timerRatio > 1.0) timerRatio = 1.0;

		// Frame 0 is full bar (escape_bar_01), Frame 9 is empty bar (escape_bar_10)
		int frameIndex = (int)((1.0 - timerRatio) * 10.0);
		if (frameIndex < 0) frameIndex = 0;
		if (frameIndex > 9) frameIndex = 9;

		int barX = 945;
		int barY = 814;
		int barW = 230;
		int barH = 30;

		if (escapeBarImages[frameIndex] > 0) {
			iShowImage(barX, barY, barW, barH, escapeBarImages[frameIndex]);
		}
		else {
			// Fallback vector bar
			iSetColor(40, 15, 20);
			iFilledRectangle(barX, barY, barW, barH);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(0.9f, 0.2f, 0.2f, 0.85f);
			iFilledRectangle(barX + 2, barY + 2, (barW - 4) * timerRatio, barH - 4);
			glDisable(GL_BLEND);
			iSetColor(255, 215, 0);
			iRectangle(barX, barY, barW, barH);
		}

		char timeText[30];
		sprintf_s(timeText, "%ds", countdownTimer);
		iSetColor(255, 255, 255);
		iText(barX + barW - 42, barY + 8, timeText, GLUT_BITMAP_HELVETICA_12);
	}
}

// Game Over screen buttons (Bound to your restartBtn.png & mainmenuBtn.png images)
static ImageButton gameOverRestartBtn(450, 400, 300, 50, restartBtn, "RESTART");
static ImageButton gameOverMenuBtn(450, 310, 300, 50, mainmenuBtn, "MAIN MENU");

inline void drawGameOverScreen() {
	// Dark semi-transparent dim overlay over full screen
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.0f, 0.0f, 0.0f, 0.75f);
	iFilledRectangle(0, 0, 1200, 860);
	glDisable(GL_BLEND);

	// Game Over Card Frame
	iSetColor(20, 22, 32);
	iFilledRectangle(400, 270, 400, 320);
	iSetColor(255, 50, 50);
	iRectangle(400, 270, 400, 320);
	iRectangle(404, 274, 392, 312);

	iSetColor(255, 60, 60);
	iText(490, 530, "HEIST FAILED!", GLUT_BITMAP_TIMES_ROMAN_24);

	iSetColor(200, 200, 210);
	iText(460, 490, "You were caught by security!", GLUT_BITMAP_HELVETICA_12);

	// Draw button images
	gameOverRestartBtn.draw();
	gameOverMenuBtn.draw();
}

inline void handleGameOverClick(int mx, int my) {
	if (gameOverRestartBtn.isClicked(mx, my)) {
		restartCurrentLevel();
		currentGameState = STATE_PLAYING;
	}
	else if (gameOverMenuBtn.isClicked(mx, my)) {
		isGameOver = false;
		stopAlarmSounds();
		stopLevelMusic();
		currentGameState = STATE_MAIN_MENU;
	}
}

// Level Complete screen buttons
static ImageButton completeRestartBtn(400, 310, 180, 50, restartBtn, "RESTART");
static ImageButton completeMenuBtn(620, 310, 180, 50, mainmenuBtn, "MAIN MENU");

inline void drawLevelCompleteScreen() {
	// Full-screen background image
	if (lvlCompleteImage > 0) {
		iShowImage(0, 0, 1200, 860, lvlCompleteImage);
	}
	else {
		// Fallback dark background if image fails to load
		iSetColor(8, 12, 24);
		iFilledRectangle(0, 0, 1200, 860);
	}

	// --- FUNNY LEVEL 2 MESSAGE ---
	if (currentLevel == 2) {
		// Main congratulatory joke
		iSetColor(255, 215, 0); // Gold
		iText(350, 640, "Great, you ACTUALLY completed Level 2!", GLUT_BITMAP_TIMES_ROMAN_24);

		// The canteen follow-up
		iSetColor(0, 255, 255); // Cyan
		iText(350, 600, "Quick, meet me in the canteen!", GLUT_BITMAP_HELVETICA_18);
	}
	else {
		// Generic text for Level 1
		iSetColor(255, 215, 0);
		iText(500, 620, "LEVEL CLEARED!", GLUT_BITMAP_TIMES_ROMAN_24);
	}
	// ------------------------------

	// Draw 3-star performance rating
	int stars = calculateEarnedStars(countdownTimer, gemsCollected, totalGemsInLevel);
	drawStarsRating(530, 530, stars); // Shifted slightly down to make room for the text

	// Display detailed stats
	iSetColor(255, 255, 255);
	char statsBuf[64];

	sprintf_s(statsBuf, "Total Score: %d", playerScore);
	iText(480, 470, statsBuf, GLUT_BITMAP_HELVETICA_18);

	sprintf_s(statsBuf, "Cash Collected: %d / %d", gemsCollected, totalGemsInLevel);
	iText(480, 435, statsBuf, GLUT_BITMAP_HELVETICA_18);

	sprintf_s(statsBuf, "Time Remaining: %d s", countdownTimer);
	iText(480, 400, statsBuf, GLUT_BITMAP_HELVETICA_18);

	// Draw button images
	completeRestartBtn.draw();
	completeMenuBtn.draw();
}

inline void handleLevelCompleteClick(int mx, int my) {
	if (completeRestartBtn.isClicked(mx, my)) {
		restartCurrentLevel();
		currentGameState = STATE_PLAYING;
	}
	else if (completeMenuBtn.isClicked(mx, my)) {
		isLevelComplete = false;
		stopAlarmSounds();
		stopLevelMusic();
		currentGameState = STATE_MAIN_MENU;
	}
}

// Controller navigation for Game Over (clamped top/bottom, A/X confirm)
static int gameOverSelectIndex = 0;
inline void handleGameOverController() {
	if (GameController.isNavUp()) {
		gameOverSelectIndex = 0; // clamped to top (cannot wrap)
	}
	else if (GameController.isNavDown()) {
		gameOverSelectIndex = 1; // clamped to bottom (cannot wrap)
	}

	gameOverRestartBtn.isSelected = (gameOverSelectIndex == 0);
	gameOverMenuBtn.isSelected = (gameOverSelectIndex == 1);

	if (GameController.isConfirmPressed()) {
		playClickSound();
		if (gameOverSelectIndex == 0) {
			restartCurrentLevel();
			currentGameState = STATE_PLAYING;
		}
		else {
			isGameOver = false;
			stopAlarmSounds();
			stopLevelMusic();
			currentGameState = STATE_MAIN_MENU;
		}
	}
}

// Controller navigation for Level Complete (clamped left/right, A/X confirm)
static int completeSelectIndex = 0;
inline void handleLevelCompleteController() {
	if (GameController.isNavLeft()) {
		completeSelectIndex = 0; // clamped to left (cannot wrap)
	}
	else if (GameController.isNavRight()) {
		completeSelectIndex = 1; // clamped to right (cannot wrap)
	}

	completeRestartBtn.isSelected = (completeSelectIndex == 0);
	completeMenuBtn.isSelected = (completeSelectIndex == 1);

	if (GameController.isConfirmPressed()) {
		playClickSound();
		if (completeSelectIndex == 0) {
			restartCurrentLevel();
			currentGameState = STATE_PLAYING;
		}
		else {
			isLevelComplete = false;
			stopAlarmSounds();
			stopLevelMusic();
			currentGameState = STATE_MAIN_MENU;
		}
	}
}

#endif