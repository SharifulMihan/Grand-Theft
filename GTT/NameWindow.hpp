#ifndef NAMEWINDOW_HPP
#define NAMEWINDOW_HPP

#include "utility.hpp"
#include "FileHandler.hpp"
#include <cstring>

// ============================================================================
// NAME WINDOW RELATED CODE
// ============================================================================

// Forward declarations for name window images
extern int nameWindowImage;
extern int namePlateImage;
extern int submitBtnImage;
extern int cancelBtnImage;

// Forward declaration for currentUsername
extern char currentUsername[50];

// Forward declarations for buttons (defined in MainMenu.hpp)
extern ImageButton submitButton;
extern ImageButton cancelButton;

// Key state tracking for username input debounce
static unsigned int lastKeyState[512] = {0};

// ============================================================================
// KEYBOARD INPUT FOR USERNAME (Polling-based for iGraphics library)
// ============================================================================
inline void handleUsernameInput() {
	if (currentGameState != STATE_USERNAME_INPUT) {
		return;
	}
	
	int len = strlen(currentUsername);
	
	// Check all printable ASCII characters (32-126)
	for (int key = 32; key <= 126; key++) {
		// Key was just pressed (debounce: only trigger on rising edge)
		if (keyPressed[key] && lastKeyState[key] == 0 && len < 49) {
			currentUsername[len] = (unsigned char)key;
			currentUsername[len + 1] = '\0';
		}
		// Update last state
		lastKeyState[key] = keyPressed[key];
	}
	
	// Handle backspace (key 8)
	if (keyPressed[8] && lastKeyState[8] == 0 && len > 0) {
		currentUsername[len - 1] = '\0';
	}
	
	// Handle Enter key (key 13)
	if (keyPressed[13] && lastKeyState[13] == 0 && len > 0) {
		resetDeathCount();
		if (currentLevel == 1) {
			lvl1Initialize();
		} else if (currentLevel == 2) {
			lvl2Initialize();
		} else if (currentLevel == 3) {
			lvl3Initialize();
		}
		currentGameState = STATE_PLAYING;
	}
	
	// Update last states for special keys
	lastKeyState[8] = keyPressed[8];
	lastKeyState[13] = keyPressed[13];
}

// ============================================================================
// DRAW USERNAME INPUT SCREEN
// ============================================================================
inline void drawUsernameInput() {
	extern int menubackgroundImage;
	
	iShowImage(0, 0, 1200, 860, menubackgroundImage);
	
	// Draw name window
	if (nameWindowImage > 0) {
		iShowImage(350, 200, 500, 300, nameWindowImage);
	} else {
		// Fallback: draw a semi-transparent box
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(0.1f, 0.15f, 0.25f, 0.95f);
		iFilledRectangle(350, 200, 500, 300);
		glColor4f(0.0f, 0.8f, 1.0f, 1.0f);
		iRectangle(350, 200, 500, 300);
		glDisable(GL_BLEND);
	}
	
	// Draw name plate
	if (namePlateImage > 0) {
		iShowImage(400, 240, 400, 50, namePlateImage);
	} else {
		// Fallback: draw input box
		iSetColor(20, 30, 50);
		iFilledRectangle(400, 240, 400, 50);
		iSetColor(0, 200, 255);
		iRectangle(400, 240, 400, 50);
	}
	
	// Draw title - black, bold, uppercase
	iSetColor(0, 0, 0); // Black color
	iText(500, 450, "", GLUT_BITMAP_TIMES_ROMAN_24); // Bold-like font, larger
	
	// Draw current username - black, bold, uppercase
	iSetColor(0, 0, 0); // Black color
	// Convert username to uppercase for display
	char upperName[50] = {0};
	int len = strlen(currentUsername);
	for (int j = 0; j < len; j++) {
		if (currentUsername[j] >= 'a' && currentUsername[j] <= 'z') {
			upperName[j] = currentUsername[j] - 32; // Convert to uppercase
		} else {
			upperName[j] = currentUsername[j];
		}
	}
	iText(420, 260, upperName, GLUT_BITMAP_TIMES_ROMAN_24); // Bold-like font, larger, lowered position
	
	// Draw submit and cancel buttons
	extern ImageButton submitButton;
	extern ImageButton cancelButton;
	submitButton.draw();
	cancelButton.draw();
}

// ============================================================================
// HANDLE USERNAME INPUT CLICK
// ============================================================================
inline void handleUsernameInputClick(int mx, int my) {
	extern ImageButton submitButton;
	extern ImageButton cancelButton;
	
	if (submitButton.isClicked(mx, my)) {
		// Validate username
		if (strlen(currentUsername) > 0) {
			// Initialize the level
			resetDeathCount();
			if (currentLevel == 1) {
				lvl1Initialize();
			} else if (currentLevel == 2) {
				lvl2Initialize();
			} else if (currentLevel == 3) {
				lvl3Initialize();
			}
			currentGameState = STATE_PLAYING;
		}
	}
	else if (cancelButton.isClicked(mx, my)) {
		currentGameState = STATE_LEVELS_MENU;
	}
}

// ============================================================================
// HANDLE USERNAME INPUT CONTROLLER
// ============================================================================
static int nameWindowSelectIndex = 0; // 0 = Submit, 1 = Cancel

inline void handleUsernameInputController() {
	extern ImageButton submitButton;
	extern ImageButton cancelButton;

	// Navigate between Submit and Cancel with left/right
	if (GameController.isNavLeft()) {
		nameWindowSelectIndex = 0;
	}
	else if (GameController.isNavRight()) {
		nameWindowSelectIndex = 1;
	}

	submitButton.isSelected = (nameWindowSelectIndex == 0);
	cancelButton.isSelected = (nameWindowSelectIndex == 1);

	if (GameController.isConfirmPressed()) {
		playClickSound();
		if (nameWindowSelectIndex == 0) {
			// Submit
			if (strlen(currentUsername) > 0) {
				resetDeathCount();
				if (currentLevel == 1) {
					lvl1Initialize();
				} else if (currentLevel == 2) {
					lvl2Initialize();
				} else if (currentLevel == 3) {
					lvl3Initialize();
				}
				currentGameState = STATE_PLAYING;
			}
		}
		else {
			// Cancel
			currentGameState = STATE_LEVELS_MENU;
		}
	}
	else if (GameController.isBackPressed()) {
		playClickSound();
		currentGameState = STATE_LEVELS_MENU;
	}
}

#endif // NAMEWINDOW_HPP
