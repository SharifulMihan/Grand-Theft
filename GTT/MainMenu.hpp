#ifndef MAIN_MENU_HPP
#define MAIN_MENU_HPP

#include "utility.hpp"
#include "Checkpoint.hpp"
#include "lvl1.hpp"
#include "lvl2.hpp"
#include "UI.hpp"
#include <cmath>

extern int menubackgroundImage;

// ============================================================================
// MAIN MENU BUTTON PLACEMENTS
// ============================================================================
static ImageButton playButton(150, 450, 260, 60, playBtnImage, "PLAY");
static ImageButton levelsButton(150, 370, 260, 60, levelsBtnImage, "LEVELS");
static ImageButton howToPlayButton(150, 290, 260, 60, howToPlayBtnImage, "HOW TO PLAY");
static ImageButton exitButton(150, 210, 260, 60, exitBtnImage, "EXIT");

// Navigation buttons
static ImageButton level1Button(150, 480, 260, 50, level1Btn, "LEVEL 1");
static ImageButton level2Button(150, 410, 260, 50, level2Btn, "LEVEL 2");
static ImageButton backButton(200, 240, 150, 35, backBtnImage, "BACK");

// ============================================================================
// NEON HEIST MENU ART
// ============================================================================
inline void drawHeistBackdrop(bool animate = false) {
	static float menuPulse = 0.0f;
	if (animate) menuPulse += 0.025f;

	// Midnight gradient
	for (int y = 0; y < 860; y += 12) {
		float shade = (float)y / 860.0f;
		iSetColor((int)(5 + 8 * shade), (int)(10 + 18 * shade), (int)(28 + 38 * shade));
		iFilledRectangle(0, y, 1200, 12);
	}

	// Distant city skyline
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

	// Animated cyan searchlights
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

	// Large vault
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

	// Framing details
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

// ============================================================================
// 1. MAIN MENU SCREEN
// ============================================================================
inline void drawMainMenu() {
	iShowImage(0, 0, 1200, 860, menubackgroundImage);

	playButton.draw();
	levelsButton.draw();
	howToPlayButton.draw();
	exitButton.draw();

}

// ============================================================================
// 2. HOW TO PLAY SCREEN
// ============================================================================
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
	iText(240, 435, "CONTROLS:", GLUT_BITMAP_HELVETICA_18);

	iSetColor(255, 255, 255);
	iText(150, 400, "[ A / D ]  :  Move Left / Right", GLUT_BITMAP_HELVETICA_18);
	iText(150, 365, "[ SPACE ]  :  Jump", GLUT_BITMAP_HELVETICA_18);
	iText(150, 330, "[ W / S ]  :  Climb Stairs Up / Down", GLUT_BITMAP_HELVETICA_18);

	backButton.draw();
}

// ============================================================================
// 3. LEVEL SELECT SCREEN
// ============================================================================
inline void drawLevelPage() {
	iShowImage(0, 0, 1200, 860, menubackgroundImage);

	

	level1Button.draw();
	level2Button.draw();
	backButton.draw();
}

// ============================================================================
// CLICK HANDLERS
// ============================================================================
inline void handleMainMenuClick(int mx, int my) {
	if (playButton.isClicked(mx, my)) {
		GameCheckpoint.clear(); // Clears checkpoint to ensure starting from initial spawn
		currentLevel = 1;
		lvl1Initialize();
		currentGameState = STATE_PLAYING;
	}
	else if (levelsButton.isClicked(mx, my)) {
		currentGameState = STATE_LEVELS_MENU;
	}
	else if (howToPlayButton.isClicked(mx, my)) {
		currentGameState = STATE_HOW_TO_PLAY;
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
		GameCheckpoint.clear(); // Clears checkpoint to ensure starting from initial spawn
		currentLevel = 1;
		lvl1Initialize();
		currentGameState = STATE_PLAYING;
	}
	else if (level2Button.isClicked(mx, my)) {
		GameCheckpoint.clear(); // Clears checkpoint to ensure starting from initial spawn
		currentLevel = 2;
		lvl2Initialize();
		currentGameState = STATE_PLAYING;
	}
	else if (backButton.isClicked(mx, my)) {
		currentGameState = STATE_MAIN_MENU;
	}
}

// ============================================================================
// CONTROLLER NAVIGATION (Clean, clamped, no skipping)
// ============================================================================
static int mainSelectIndex = 0;
inline void handleMainMenuController() {
	if (GameController.isNavUp()) {
		if (mainSelectIndex > 0) {
			mainSelectIndex--;
		}
	}
	else if (GameController.isNavDown()) {
		if (mainSelectIndex < 3) {
			mainSelectIndex++;
		}
	}

	playButton.isSelected = (mainSelectIndex == 0);
	levelsButton.isSelected = (mainSelectIndex == 1);
	howToPlayButton.isSelected = (mainSelectIndex == 2);
	exitButton.isSelected = (mainSelectIndex == 3);

	if (GameController.isConfirmPressed()) {
		playClickSound();
		if (mainSelectIndex == 0) {
			GameCheckpoint.clear();
			currentLevel = 1;
			lvl1Initialize();
			currentGameState = STATE_PLAYING;
		}
		else if (mainSelectIndex == 1) {
			currentGameState = STATE_LEVELS_MENU;
		}
		else if (mainSelectIndex == 2) {
			currentGameState = STATE_HOW_TO_PLAY;
		}
		else if (mainSelectIndex == 3) {
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
		if (levelSelectIndex < 2) {
			levelSelectIndex++;
		}
	}

	level1Button.isSelected = (levelSelectIndex == 0);
	level2Button.isSelected = (levelSelectIndex == 1);
	backButton.isSelected = (levelSelectIndex == 2);

	// Confirm with Xbox A / PS4 X
	if (GameController.isConfirmPressed()) {
		playClickSound();
		if (levelSelectIndex == 0) {
			GameCheckpoint.clear();
			currentLevel = 1;
			lvl1Initialize();
			currentGameState = STATE_PLAYING;
		}
		else if (levelSelectIndex == 1) {
			GameCheckpoint.clear();
			currentLevel = 2;
			lvl2Initialize();
			currentGameState = STATE_PLAYING;
		}
		else if (levelSelectIndex == 2) {
			currentGameState = STATE_MAIN_MENU;
		}
	}
	// Back with Xbox B / PS4 O
	else if (GameController.isBackPressed()) {
		playClickSound();
		currentGameState = STATE_MAIN_MENU;
	}
}

#endif