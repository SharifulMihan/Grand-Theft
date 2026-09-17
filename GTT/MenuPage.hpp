#ifndef MENU_PAGE_HPP
#define MENU_PAGE_HPP

#include "utility.hpp"
#include "lvl1.hpp"
#include "UI.hpp"

// Pause Menu Buttons (Bound to your restartBtn.png & mainmenuBtn.png images)
static ImageButton pauseRestartBtn(500, 440, 200, 45, restartBtn, "RESTART");
static ImageButton pauseMenuBtn(500, 350, 200, 45, mainmenuBtn, "MAIN MENU");

inline void drawPauseMenu() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.0f, 0.0f, 0.0f, 0.65f);
	iFilledRectangle(0, 0, 1200, 860);
	glDisable(GL_BLEND);

	iSetColor(10, 10, 15);
	iFilledRectangle(400, 290, 400, 300);
	iSetColor(255, 215, 0);
	iRectangle(400, 290, 400, 300);

	iSetColor(255, 255, 255);
	iText(530, 540, "GAME PAUSED", GLUT_BITMAP_TIMES_ROMAN_24);

	// Draw button images (with 25% hover expansion)
	pauseRestartBtn.draw();
	pauseMenuBtn.draw();
}

inline void handlePauseMenuClick(int mx, int my) {
	if (handleSoundButtonClick(mx, my)) {
		playClickSound();
		return;
	}
	if (pauseRestartBtn.isClicked(mx, my)) {
		restartCurrentLevel();
		currentGameState = STATE_PLAYING;
	}
	else if (pauseMenuBtn.isClicked(mx, my)) {
		isCountdownActive = false;
		stopAlarmSounds();
		stopLevelMusic();
		GameController.stopVibration();
		currentGameState = STATE_MAIN_MENU;
	}
}

// Controller navigation for Pause Menu
static int pauseSelectIndex = 0;
inline void handlePauseMenuController() {
	if (GameController.isNavUp()) {
		pauseSelectIndex = 0; // clamped to top (cannot wrap)
	}
	else if (GameController.isNavDown()) {
		pauseSelectIndex = 1; // clamped to bottom (cannot wrap)
	}

	pauseRestartBtn.isSelected = (pauseSelectIndex == 0);
	pauseMenuBtn.isSelected = (pauseSelectIndex == 1);

	// Confirm with Xbox A / PS4 X
	if (GameController.isConfirmPressed()) {
		playClickSound();
		if (pauseSelectIndex == 0) {
			restartCurrentLevel();
			currentGameState = STATE_PLAYING;
		}
		else {
			isCountdownActive = false;
			stopAlarmSounds();
			stopLevelMusic();
			GameController.stopVibration();
			currentGameState = STATE_MAIN_MENU;
		}
	}
	// Back with Xbox B / PS4 O, or Start/Options button to resume game
	else if (GameController.isBackPressed() || GameController.isOptionsPressed()) {
		playClickSound();
		currentGameState = STATE_PLAYING;
	}
}

#endif