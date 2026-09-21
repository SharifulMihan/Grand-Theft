#ifndef MAIN_MENU_HPP
#define MAIN_MENU_HPP

#include "utility.hpp"

#include <cmath>
#include "Checkpoint.hpp"
#include "lvl1.hpp"
#include "lvl2.hpp"
#include "lvl3.hpp"
#include "UI.hpp"
#include "FileHandler.hpp"
#include "NameWindow.hpp"
#include "MainMenuAnimation.hpp"
#include "glut.h"

// Forward declarations for NameWindow functions
inline void handleUsernameInput();
inline void handleUsernameInputClick(int mx, int my);
inline void handleUsernameInputController();




// Forward declaration for keyPressed array from iGraphics.h
extern unsigned int keyPressed[512];

extern int menubackgroundImage;
extern int scoreboardBgImage;

extern int playBtnImage;
extern int levelsBtnImage;
extern int howToPlayBtnImage;
extern int exitBtnImage;
extern int backBtnImage;
extern int scoreboardBtnImage;
extern int aboutBtnImage;
extern int aboutPageImage;

extern int nameWindowImage;
extern int namePlateImage;
extern int submitBtnImage;
extern int cancelBtnImage;

static ImageButton playButton(150, 450, 260, 60, playBtnImage, "PLAY");
static ImageButton scoreboardButton(150, 380, 260, 60, scoreboardBtnImage, "SCOREBOARD");
static ImageButton levelsButton(150, 310, 260, 60, levelsBtnImage, "LEVELS");
static ImageButton howToPlayButton(150, 240, 260, 60, howToPlayBtnImage, "HOW TO PLAY");
static ImageButton aboutButton(150, 170, 260, 60, aboutBtnImage, "ABOUT");
static ImageButton exitButton(150, 100, 260, 60, exitBtnImage, "EXIT");

static ImageButton level1Button(150, 480, 260, 50, level1Btn, "LEVEL 1");
static ImageButton level2Button(150, 410, 260, 50, level2Btn, "LEVEL 2");
static ImageButton level3Button(150, 340, 260, 50, level3Btn, "LEVEL 3");
static ImageButton backButton(40, 80, 120, 30, backBtnImage, "BACK");

static ImageButton submitButton(500, 350, 120, 40, submitBtnImage, "SUBMIT");
static ImageButton cancelButton(660, 350, 120, 40, cancelBtnImage, "CANCEL");
inline void drawHeistBackdrop(bool animate = false) {
	static float menuPulse = 0.0f;
	if (animate) menuPulse += 0.025f;

	for (int y = 0; y < 860; y += 12) {
		float shade = (float)y / 860.0f;
		iSetColor((int)(5 + 8 * shade), (int)(10 + 18 * shade), (int)(28 + 38 * shade));
		iFilledRectangle(0, y, 1200, 12);
	}

	const int buildings[] = { 85, 150, 105, 215, 125, 175, 260, 120, 185, 230, 145, 290, 170, 110, 205 };
	int x = 0;
	for (int i = 0; i < 15; i++) {
		int width = 58 + (i % 3) * 18;
		iSetColor(10, 20, 42);
		iFilledRectangle(x, 70, width, buildings[i]);
		iSetColor(27, 76, 108);
		for (int wy = 92; wy < buildings[i] + 55; wy += 30) {
			for (int wx = x + 12; wx < x + width - 8; wx += 22) {
				iFilledRectangle(wx, wy, 6, 9);
			}
		}
		x += width + 20;
	}

	double glow = sin(menuPulse) * 65.0;
	double leftBeamX[3] = { 830.0, 1080.0 + glow, 1000.0 + glow };
	double leftBeamY[3] = { 105.0, 810.0, 810.0 };
	double rightBeamX[3] = { 1030.0, 700.0 - glow, 785.0 - glow };
	double rightBeamY[3] = { 105.0, 790.0, 790.0 };
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.0f, 0.78f, 1.0f, 0.08f);
	iFilledPolygon(leftBeamX, leftBeamY, 3);
	glColor4f(0.85f, 0.12f, 0.28f, 0.08f);
	iFilledPolygon(rightBeamX, rightBeamY, 3);
	glDisable(GL_BLEND);

	iSetColor(18, 34, 57);
	iFilledRectangle(765, 240, 330, 330);
	iSetColor(0, 210, 255);
	iRectangle(765, 240, 330, 330);
	iRectangle(771, 246, 318, 318);
	iSetColor(13, 25, 45);
	iFilledRectangle(825, 300, 210, 210);
	iSetColor(222, 181, 61);
	iRectangle(825, 300, 210, 210);
	iRectangle(832, 307, 196, 196);
	iSetColor(0, 215, 255);
	iLine(930, 318, 930, 492);
	iLine(843, 405, 1017, 405);
	iSetColor(230, 190, 70);
	iFilledCircle(930, 405, 32, 16);
	iSetColor(10, 22, 42);
	iFilledCircle(930, 405, 12, 12);

	iSetColor(0, 200, 255);
	iLine(40, 45, 1160, 45);
	iSetColor(222, 181, 61);
	iLine(40, 48, 1160, 48);
	iSetColor(120, 155, 185);
	iText(860, 195, "SECURE VAULT NETWORK", GLUT_BITMAP_HELVETICA_12);
}

inline void drawMenuAction(ImageButton& button, const char* label, const char* hint) {
	bool hovered = button.isHovered();
	int x = button.x;
	int y = button.y;
	int w = button.w;
	int h = button.h;
	if (hovered) {
		x -= 8; y -= 4; w += 16; h += 8;
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.01f, 0.03f, 0.09f, hovered ? 0.96f : 0.86f);
	iFilledRectangle(x, y, w, h);
	glColor4f(hovered ? 0.0f : 0.72f, hovered ? 0.92f : 0.60f, hovered ? 1.0f : 0.12f, 1.0f);
	iRectangle(x, y, w, h);
	iRectangle(x + 3, y + 3, w - 6, h - 6);
	glDisable(GL_BLEND);

	iSetColor(hovered ? 255 : 220, hovered ? 245 : 190, 125);
	iText(x + 22, y + h / 2 - 8, (char*)label, GLUT_BITMAP_TIMES_ROMAN_24);
	iSetColor(115, 200, 225);
	iText(x + w - 72, y + h / 2 - 5, (char*)hint, GLUT_BITMAP_HELVETICA_12);
}


inline void drawMainMenu() {
	iShowImage(0, 0, 1200, 860, menubackgroundImage);

	playButton.draw();
	scoreboardButton.draw();
	levelsButton.draw();
	howToPlayButton.draw();
	aboutButton.draw();
	exitButton.draw();

	updateMenuCharacterAnimation();
	drawMenuCharacterAnimation();

}


inline void drawHowToPlayPage() {
	iShowImage(0, 0, 1200, 860, menubackgroundImage);

	

	

	iSetColor(0, 220, 255);
	iText(240, 630, "OBJECTIVES:", GLUT_BITMAP_HELVETICA_18);

	iSetColor(255, 255, 255);
	iText(150, 595, "1. Steal the Key", GLUT_BITMAP_HELVETICA_18);
	iText(150, 560, "2. Unlock the Golden Loot Box", GLUT_BITMAP_HELVETICA_18);
	iText(150, 525, "3. Dodge Lasers, Bombs & Security Cameras", GLUT_BITMAP_HELVETICA_18);
	iText(150, 490, "4. Escape through the Exit Door before time runs out!", GLUT_BITMAP_HELVETICA_18);

	iSetColor(0, 220, 255);
	iText(240, 435, "You can use both Keyboard and Controller for CONTROLS:", GLUT_BITMAP_HELVETICA_18);


	iSetColor(255, 255, 255);
	iText(150, 400, "[ A / D L1 Stick / Right and Left Arrow]  :  Move Left / Right", GLUT_BITMAP_HELVETICA_18);
	iText(150, 365, "[ SPACE / X / A]  :  Jump", GLUT_BITMAP_HELVETICA_18);
	iText(150, 330, "[ W / S / L1 Stick / Up and Down Arrow]  :  Climb Stairs Up / Down", GLUT_BITMAP_HELVETICA_18);
	iText(150, 295, "[ X / A]  :  Enter", GLUT_BITMAP_HELVETICA_18);
	iText(150, 260, "[ O / B]  :  Back", GLUT_BITMAP_HELVETICA_18);

	backButton.draw();
}


inline void drawLevelPage() {
	iShowImage(0, 0, 1200, 860, menubackgroundImage);

	

	level1Button.draw();
	level2Button.draw();
	level3Button.draw();
	backButton.draw();
}



inline void drawScoreboardPage() {
	if (scoreboardBgImage > 0) {
		iShowImage(0, 0, 1200, 860, scoreboardBgImage);
	} else {
		iShowImage(0, 0, 1200, 860, menubackgroundImage);
	}
	
	iSetColor(0, 220, 255);
	iText(550, 800, "LEADERBOARD", GLUT_BITMAP_TIMES_ROMAN_24);
	
	extern int leaderboardCount;
	
	if (leaderboardCount == 0) {
		iSetColor(255, 100, 100);
		iText(450, 650, "No scores yet!", GLUT_BITMAP_HELVETICA_18);
		backButton.draw();
		return;
	}
	
	PlayerScore level1Rankings[MAX_PLAYERS];
	PlayerScore level2Rankings[MAX_PLAYERS];
	PlayerScore level3Rankings[MAX_PLAYERS];
	int count1, count2, count3;
	
	getLevel1Rankings(level1Rankings, &count1);
	getLevel2Rankings(level2Rankings, &count2);
	getLevel3Rankings(level3Rankings, &count3);
	
	iSetColor(255, 255, 255);
	iText(200, 700, "LEVEL 1", GLUT_BITMAP_HELVETICA_18);
	iText(600, 700, "LEVEL 2", GLUT_BITMAP_HELVETICA_18);
	iText(1000, 700, "LEVEL 3", GLUT_BITMAP_HELVETICA_18);
	
	int startY = 650;
	for (int i = 0; i < 10 && i < count1; i++) {
		char upperName[MAX_USERNAME_LEN];
		strcpy_s(upperName, sizeof(upperName), level1Rankings[i].username);
		for (int j = 0; upperName[j] != '\0'; j++) {
			if (upperName[j] >= 'a' && upperName[j] <= 'z') {
				upperName[j] = upperName[j] - 32;
			}
		}
		
		iSetColor(255, 255, 255);
		char rankNum[16];
		sprintf_s(rankNum, sizeof(rankNum), "%d.", i + 1);
		iText(150, startY - i * 40, rankNum, GLUT_BITMAP_HELVETICA_18);
		
		iSetColor(255, 165, 0);
		iText(180, startY - i * 40, upperName, GLUT_BITMAP_TIMES_ROMAN_24);
		
		iSetColor(55, 255, 55);
		char deathText[32];
		sprintf_s(deathText, sizeof(deathText), "%d", level1Rankings[i].level1Deaths);
		iText(180 + 150, startY - i * 40, deathText, GLUT_BITMAP_TIMES_ROMAN_24);
	}
	
	for (int i = 0; i < 10 && i < count2; i++) {
		char upperName[MAX_USERNAME_LEN];
		strcpy_s(upperName, sizeof(upperName), level2Rankings[i].username);
		for (int j = 0; upperName[j] != '\0'; j++) {
			if (upperName[j] >= 'a' && upperName[j] <= 'z') {
				upperName[j] = upperName[j] - 32;
			}
		}
		
		iSetColor(255, 255, 255);
		char rankNum[16];
		sprintf_s(rankNum, sizeof(rankNum), "%d.", i + 1);
		iText(550, startY - i * 40, rankNum, GLUT_BITMAP_HELVETICA_18);
		
		iSetColor(255, 165, 0);
		iText(580, startY - i * 40, upperName, GLUT_BITMAP_TIMES_ROMAN_24);
		
		iSetColor(55, 255, 55);
		char deathText[32];
		sprintf_s(deathText, sizeof(deathText), "%d", level2Rankings[i].level2Deaths);
		iText(580 + 150, startY - i * 40, deathText, GLUT_BITMAP_TIMES_ROMAN_24);
	}
	
	for (int i = 0; i < 10 && i < count3; i++) {
		char upperName[MAX_USERNAME_LEN];
		strcpy_s(upperName, sizeof(upperName), level3Rankings[i].username);
		for (int j = 0; upperName[j] != '\0'; j++) {
			if (upperName[j] >= 'a' && upperName[j] <= 'z') {
				upperName[j] = upperName[j] - 32;
			}
		}
		
		iSetColor(255, 255, 255);
		char rankNum[16];
		sprintf_s(rankNum, sizeof(rankNum), "%d.", i + 1);
		iText(950, startY - i * 40, rankNum, GLUT_BITMAP_HELVETICA_18);
		
		iSetColor(255, 165, 0);
		iText(980, startY - i * 40, upperName, GLUT_BITMAP_TIMES_ROMAN_24);
		
		iSetColor(55, 255, 55);
		char deathText[32];
		sprintf_s(deathText, sizeof(deathText), "%d", level3Rankings[i].level3Deaths);
		iText(980 + 150, startY - i * 40, deathText, GLUT_BITMAP_TIMES_ROMAN_24);
	}
	
	backButton.draw();
}


inline void drawAboutPage() {
	iShowImage(0, 0, 1200, 860, aboutPageImage);
	backButton.draw();
}


inline void handleMainMenuClick(int mx, int my) {
	if (playButton.isClicked(mx, my)) {
		GameCheckpoint.clear(); 
		currentLevel = 1;
		currentGameState = STATE_USERNAME_INPUT;
	}
	else if (scoreboardButton.isClicked(mx, my)) {
		currentGameState = STATE_SCOREBOARD;
	}
	else if (levelsButton.isClicked(mx, my)) {
		currentGameState = STATE_LEVELS_MENU;
	}
	else if (howToPlayButton.isClicked(mx, my)) {
		currentGameState = STATE_HOW_TO_PLAY;
	}
	else if (aboutButton.isClicked(mx, my)) {
		currentGameState = STATE_ABOUT;
	}
	else if (exitButton.isClicked(mx, my)) {
		exit(0);
	}
}

inline void handleHowToPlayClick(int mx, int my) {
	if (backButton.isClicked(mx, my)) {
		currentGameState = STATE_MAIN_MENU;
	}
}

inline void handleLevelPageClick(int mx, int my) {
	if (level1Button.isClicked(mx, my)) {
		GameCheckpoint.clear();
		currentLevel = 1;
		currentGameState = STATE_USERNAME_INPUT;
	}
	else if (level2Button.isClicked(mx, my)) {
		GameCheckpoint.clear();
		currentLevel = 2;
		currentGameState = STATE_USERNAME_INPUT;
	}
	else if (level3Button.isClicked(mx, my)) {
		GameCheckpoint.clear();
		currentLevel = 3;
		currentGameState = STATE_USERNAME_INPUT;
	}
	else if (backButton.isClicked(mx, my)) {
		currentGameState = STATE_MAIN_MENU;
	}
}


inline void handleScoreboardClick(int mx, int my) {
	if (backButton.isClicked(mx, my)) {
		currentGameState = STATE_MAIN_MENU;
	}
}

inline void handleAboutClick(int mx, int my) {
	if (backButton.isClicked(mx, my)) {
		currentGameState = STATE_MAIN_MENU;
	}
}


static int mainSelectIndex = 0;
inline void handleMainMenuController() {
	if (GameController.isNavUp()) {
		if (mainSelectIndex > 0) {
			mainSelectIndex--;
		}
	}
	else if (GameController.isNavDown()) {
		if (mainSelectIndex < 5) {
			mainSelectIndex++;
		}
	}

	playButton.isSelected = (mainSelectIndex == 0);
	scoreboardButton.isSelected = (mainSelectIndex == 1);
	levelsButton.isSelected = (mainSelectIndex == 2);
	howToPlayButton.isSelected = (mainSelectIndex == 3);
	aboutButton.isSelected = (mainSelectIndex == 4);
	exitButton.isSelected = (mainSelectIndex == 5);

	if (GameController.isConfirmPressed()) {
		playClickSound();
		if (mainSelectIndex == 0) {
			GameCheckpoint.clear();
			currentLevel = 1;
			currentGameState = STATE_USERNAME_INPUT;
		}
		else if (mainSelectIndex == 1) {
			currentGameState = STATE_SCOREBOARD;
		}
		else if (mainSelectIndex == 2) {
			currentGameState = STATE_LEVELS_MENU;
		}
		else if (mainSelectIndex == 3) {
			currentGameState = STATE_HOW_TO_PLAY;
		}
		else if (mainSelectIndex == 4) {
			currentGameState = STATE_ABOUT;
		}
		else if (mainSelectIndex == 5) {
			exit(0);
		}
	}
}

inline void handleHowToPlayController() {
	backButton.isSelected = true;

	if (GameController.isConfirmPressed() || GameController.isBackPressed()) {
		playClickSound();
		currentGameState = STATE_MAIN_MENU;
	}
}

static int levelSelectIndex = 0;
inline void handleLevelPageController() {
	if (GameController.isNavUp()) {
		if (levelSelectIndex > 0) {
			levelSelectIndex--;
		}
	}
	else if (GameController.isNavDown()) {
		if (levelSelectIndex < 3) {
			levelSelectIndex++;
		}
	}

	level1Button.isSelected = (levelSelectIndex == 0);
	level2Button.isSelected = (levelSelectIndex == 1);
	level3Button.isSelected = (levelSelectIndex == 2);
	backButton.isSelected = (levelSelectIndex == 3);

	
	if (GameController.isConfirmPressed()) {
		playClickSound();
		if (levelSelectIndex == 0) {
			GameCheckpoint.clear();
			currentLevel = 1;
			currentGameState = STATE_USERNAME_INPUT;
		}
		else if (levelSelectIndex == 1) {
			GameCheckpoint.clear();
			currentLevel = 2;
			currentGameState = STATE_USERNAME_INPUT;
		}
		else if (levelSelectIndex == 2) {
			GameCheckpoint.clear();
			currentLevel = 3;
			currentGameState = STATE_USERNAME_INPUT;
		}
		else if (levelSelectIndex == 3) {
			currentGameState = STATE_MAIN_MENU;
		}
	}
	
	else if (GameController.isBackPressed()) {
		playClickSound();
		currentGameState = STATE_MAIN_MENU;
	}
}

inline void handleScoreboardController() {
	backButton.isSelected = true;

	if (GameController.isConfirmPressed() || GameController.isBackPressed()) {
		playClickSound();
		currentGameState = STATE_MAIN_MENU;
	}
}

inline void handleAboutController() {
	backButton.isSelected = true;

	if (GameController.isConfirmPressed() || GameController.isBackPressed()) {
		playClickSound();
		currentGameState = STATE_MAIN_MENU;
	}
}

#endif
