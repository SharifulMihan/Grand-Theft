#ifndef UTILITY_H
#define UTILITY_H

#include "Effects.hpp"
#include "Scoring.hpp"

extern int mat[720][1200];

// game states
enum GameState {
	STATE_MAIN_MENU,
	STATE_PLAYING,
	STATE_PAUSE_MENU,
	STATE_LEVELS_MENU,
	STATE_HOW_TO_PLAY
};

extern GameState currentGameState;

// Plays the UI button hover sound (Sound/hover.mp3, opened in initAudio())
inline void playHoverSound() {
	mciSendString("play hoversound from 0", NULL, 0, NULL);
}

// Plays the UI button click sound (Sound/click.mp3, opened in initAudio())
inline void playClickSound() {
	mciSendString("play clicksound from 0", NULL, 0, NULL);
}

// All the image IDs
extern int backgroundImage;
extern int stairImage;
extern int laserImage;
extern int platformImage;
extern int platformLvl1Image;
extern int platformLvl2Image;
extern int liftPlatformImage;
extern int redSwitchImage;
extern int greenSwitchImage;
extern int keyImage;
extern int closedBoxImage;
extern int openedBoxImage;
extern int closedDoorImage;
extern int openedDoorImage;
extern int bumpImage;
extern int restartBtnImage;
extern int menuBtnImage;
extern int soundOnImage;
extern int soundOffImage;
extern bool isSoundOn;

// Menu Button Images (Assign loaded image IDs to these variables)
extern int playBtnImage;
extern int levelsBtnImage;
extern int howToPlayBtnImage;
extern int exitBtnImage;
extern int backBtnImage;
extern int level1Btn;
extern int level2Btn;
extern int level3Btn;
extern int restartBtn;
extern int mainmenuBtn;

// Current mouse cursor coordinates (for button hover detection)
extern int curMouseX;
extern int curMouseY;

// Which level is currently active (1, 2, or 3)
extern int currentLevel;

// Level 2 background image
extern int lvl2BgImage;

// Forward declarations for level initialization
void lvl1Initialize();
void lvl2Initialize();
void lvl3Initialize();

inline void restartCurrentLevel() {
	if (currentLevel == 2) {
		lvl2Initialize();
	}
	else if (currentLevel == 3) {
		lvl3Initialize();
	}
	else {
		lvl1Initialize();
	}
}

// ============================================================================
// IMAGE BUTTON COMPONENT (Supports direct image drawing and 25% hover expansion)
// ============================================================================
struct ImageButton {
	int x, y, w, h;
	const int* imagePtr;
	const char* fallbackLabel;
	bool wasHovered;   // tracks mouse hover entry for sound (fired in draw)
	bool isSelected;   // set true when navigated to via controller
	bool wasSelected;  // tracks previous frame isSelected, for sound on controller nav change

	ImageButton(int posX = 0, int posY = 0, int width = 0, int height = 0, const int& imgVar = playBtnImage, const char* label = "") {
		this->x = posX;
		this->y = posY;
		this->w = width;
		this->h = height;
		this->imagePtr = &imgVar;
		this->fallbackLabel = label;
		this->wasHovered = false;
		this->isSelected = false;
		this->wasSelected = false;
	}

	bool isHovered() {
		return (curMouseX >= x && curMouseX <= x + w && curMouseY >= y && curMouseY <= y + h);
	}

	void draw(bool enableHover = true) {
		bool mouseOver = enableHover && isHovered();
		bool active = mouseOver || isSelected;

		// Mouse hover-in: sound fires once when mouse enters
		if (mouseOver && !wasHovered) {
			playHoverSound();
		}
		wasHovered = mouseOver;

		// Controller selection: sound fires once exactly when isSelected becomes true
		if (isSelected && !wasSelected) {
			playHoverSound();
		}
		wasSelected = isSelected;

		int drawX = x;
		int drawY = y;
		int drawW = w;
		int drawH = h;

		// When active (mouse hover OR controller selected), expand size by 25% centered
		if (active) {
			drawW = (int)(w * 1.25);
			drawH = (int)(h * 1.25);
			drawX = x - (drawW - w) / 2;
			drawY = y - (drawH - h) / 2;
		}

		// If you attached an image, render it directly!
		if (imagePtr != NULL && *imagePtr > 0) {
			iShowImage(drawX, drawY, drawW, drawH, *imagePtr);
		}
		else {
			// Clean placeholder with hover feedback if image is not loaded yet
			if (active) {
				iSetColor(50, 60, 85);
			}
			else {
				iSetColor(35, 40, 55);
			}
			iFilledRectangle(drawX, drawY, drawW, drawH);

			if (active) {
				iSetColor(0, 220, 255);
			}
			else {
				iSetColor(255, 215, 0);
			}
			iRectangle(drawX, drawY, drawW, drawH);

			iSetColor(255, 255, 255);
			iText(drawX + 25, drawY + (drawH / 2) - 6, (char*)fallbackLabel, GLUT_BITMAP_TIMES_ROMAN_24);
		}
	}

	bool isClicked(int mx, int my) {
		int drawW = (int)(w * 1.25);
		int drawH = (int)(h * 1.25);
		int drawX = x - (drawW - w) / 2;
		int drawY = y - (drawH - h) / 2;
		bool clicked = (mx >= drawX && mx <= drawX + drawW && my >= drawY && my <= drawY + drawH);
		if (clicked) {
			playClickSound();
		}
		return clicked;
	}
};

// The exit door
struct Door {
	int x, y, w, h;
	bool isOpened;

	Door(int startX = 0, int startY = 0, int width = 50, int height = 70) {
		this->x = startX;
		this->y = startY;
		this->w = width;
		this->h = height;
		this->isOpened = false;
	}

	void reset() {
		isOpened = false;
	}

	void update() {}

	void draw() {
		if (isOpened) {
			iShowImage(x, y, w, h, openedDoorImage);
		}
		else {
			iShowImage(x, y, w, h, closedDoorImage);
		}
	}


	bool checkCollision(int px, int py, int pSize) {
		if (!isOpened) {
			return false;
		}
		bool overlapX = px < x + w && px + pSize > x;
		bool overlapY = py < y + h && py + pSize > y;
		return overlapX && overlapY;
	}
};




// The key with floating motion and golden sparkle emitter
struct Key {
	int x;
	double baseY;
	double currentY;
	int w, h;
	bool isCollected;
	double floatRange;
	double floatSpeed;
	bool floatingUp;

	Key(int startX = 0, double startY = 0.0, int width = 25, int height = 25, double range = 4.0, double speed = 0.25) {
		this->x = startX;
		this->baseY = startY;
		this->currentY = startY;
		this->w = width;
		this->h = height;
		this->isCollected = false;
		this->floatRange = range;
		this->floatSpeed = speed;
		this->floatingUp = true;
	}

	// Moves the key up and down
	void update() {
		if (!isCollected) {
			// Floating animation
			if (floatingUp) {
				currentY += floatSpeed;
				if (currentY >= baseY + floatRange) {
					currentY = baseY + floatRange;
					floatingUp = false;
				}
			}
			else {
				currentY -= floatSpeed;
				if (currentY <= baseY - floatRange) {
					currentY = baseY - floatRange;
					floatingUp = true;
				}
			}

			// Emit golden sparkles around key
			spawnKeySparkles((double)x + w / 2, currentY + h / 2);
		}
	}

	void draw() {
		if (!isCollected) {
			iShowImage(x, (int)currentY, w, h, keyImage);
		}
	}

	void checkCollision(int px, int py, int pSize) {
		if (isCollected) {
			return;
		}
		bool overlapX = px < x + w && px + pSize > x;
		bool overlapY = py < currentY + h && py + pSize > currentY;
		if (overlapX && overlapY) {
			isCollected = true;
			spawnFloatingText(x, currentY + 30, "KEY ACQUIRED!", 255, 220, 0);
			spawnCoinBurst(x + w / 2, currentY + h / 2, 8);
		}
	}
};





// The loot box with coin burst explosion effect
struct LootBox {
	int x, y, w, h;
	bool isOpened;

	LootBox(int startX = 0, int startY = 0, int width = 40, int height = 40) {
		this->x = startX;
		this->y = startY;
		this->w = width;
		this->h = height;
		this->isOpened = false;
	}

	void draw() {
		if (isOpened) {
			iShowImage(x, y, w, h, openedBoxImage);
		}
		else {
			iShowImage(x, y, w, h, closedBoxImage);
		}
	}

	void checkCollision(int px, int py, int pSize, bool playerHasKey) {
		if (isOpened || !playerHasKey) {
			return;
		}
		bool overlapX = px < x + w && px + pSize > x;
		bool overlapY = py < y + h && py + pSize > y;
		if (overlapX && overlapY) {
			isOpened = true;
			// Trigger FX on looting
			spawnCoinBurst(x + w / 2, y + h / 2, 25);
			spawnFloatingText(x - 20, y + h + 10, "+1000 GOLD LOOTED!", 255, 215, 0);
			triggerShake(6, 12);
			playerScore += 1000;
		}
	}
};




// switch 
struct TriggerSwitch {
	int x, y, w, h;
	bool activated;

	TriggerSwitch(int startX = 0, int startY = 0, int width = 30, int height = 10) {
		this->x = startX;
		this->y = startY;
		this->w = width;
		this->h = height;
		this->activated = false;
	}

	void draw() {
		if (activated) {
			iShowImage(x, y, w, h, greenSwitchImage);
		}
		else {
			iShowImage(x, y, w, h, redSwitchImage);
		}
	}

	// bit masking
	void writeToMatrix() {
		for (int row = y; row < y + h && row < 720; row++) {
			for (int col = x; col < x + w && col < 1200; col++) {
				if (col >= 0 && row >= 0) {
					mat[row][col] = 3;
				}
			}
		}
	}
};





// lift
struct MovableLift {
	int x, y, w, h;
	int initialY;
	int targetY;
	int speed;
	bool hasMoved;

	MovableLift(int inputX = 0, int inputY = 0, int inputW = 0, int inputH = 0, int inputTargetY = 0, int inputSpeed = 1) {
		this->x = inputX;
		this->y = inputY;
		this->initialY = inputY;
		this->w = inputW;
		this->h = inputH;
		this->targetY = inputTargetY;
		this->speed = inputSpeed;
		this->hasMoved = false;
	}

	// reset the platforms position
	void reset() {
		y = initialY;
		hasMoved = false;
	}

	// moves the lift
	void update(bool isSwitchActive) {
		hasMoved = false;

		if (!isSwitchActive) {
			return;
		}
		if (y == targetY) {
			return;
		}
		if (y > targetY) {
			y -= speed;
			if (y < targetY) {
				y = targetY;
			}
		}
		else {
			y += speed;
			if (y > targetY) {
				y = targetY;
			}
		}
		hasMoved = true;
	}

	void draw() {
		iShowImage(x, y, w, h, liftPlatformImage);
	}

	// bit masking
	void writeToMatrix() {
		for (int row = y; row < y + h && row < 720; row++) {
			for (int col = x; col < x + w && col < 1200; col++) {
				if (col >= 0 && row >= 0) {
					mat[row][col] = 1;
				}
			}
		}
	}
};

struct BouncingPlatform {
	int x, w, h;
	double currentY;
	int minY, maxY;
	double speed;
	bool movingUp;
	bool hasMoved;

	BouncingPlatform(int posX = 0, int posY = 0, int width = 90, int height = 15, int minVal = 0, int maxVal = 100, double spd = 1.0) {
		x = posX;
		currentY = (double)posY;
		w = width; h = height;
		minY = minVal; maxY = maxVal;
		speed = spd;
		movingUp = true; hasMoved = false;
	}

	int getY() { return (int)currentY; }

	void update() {
		hasMoved = false;
		if (movingUp) {
			currentY += speed;
			if (currentY >= maxY) { currentY = maxY; movingUp = false; }
		}
		else {
			currentY -= speed;
			if (currentY <= minY) { currentY = minY; movingUp = true; }
		}
		hasMoved = true;
	}

	void draw() {
		iShowImage(x, (int)currentY, w, h, liftPlatformImage);
	}

	void writeToMatrix() {
		int y = (int)currentY;
		for (int row = y; row < y + h && row < 720; row++)
		for (int col = x; col < x + w && col < 1200; col++)
		if (col >= 0 && row >= 0) mat[row][col] = 1;
	}
};



#endif
