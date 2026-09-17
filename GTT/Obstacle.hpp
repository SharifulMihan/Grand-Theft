#ifndef OBSTACLE_HPP
#define OBSTACLE_HPP

#include "utility.hpp"

extern int bompImage;
extern int cutterImages[3]; // Updated to reference the 3-frame animation array[cite: 5]

// laser 
struct Laser {
	int x;
	double y;
	int w;
	int h;
	int minY;
	double maxY;
	double speed;
	bool movingUp;

	Laser(int startX = 0, double startY = 160.0, int width = 140, int height = 10, int minYVal = 160, double maxYVal = 360.0, double moveSpeed = 3.0) {
		this->x = startX;
		this->y = startY;
		this->w = width;
		this->h = height;
		this->minY = minYVal;
		this->maxY = maxYVal;
		this->speed = moveSpeed;
		this->movingUp = true;
	}

	void update() {
		if (movingUp) {
			y += speed;
			if (y + h >= maxY) {
				y = maxY - h;
				movingUp = false;
			}
		}
		else {
			y -= speed;
			if (y <= minY) {
				y = minY;
				movingUp = true;
			}
		}
	}

	void draw() {
		iShowImage(x, (int)y, w, h, laserImage);
	}

	bool checkCollision(int px, int py, int pSize) {
		bool overlapX = px < x + w && px + pSize > x;
		bool overlapY = py < y + h && py + pSize > y;
		return overlapX && overlapY;
	}
};






// bomp
struct Bomb {
	int x;
	double baseY;
	double currentY;
	int w, h;
	double floatRange;
	double floatSpeed;
	bool floatingUp;

	Bomb(int startX = 0, double startY = 0.0, int width = 30, int height = 30, double range = 8.0, double speed = 0.4) {
		this->x = startX;
		this->baseY = startY;
		this->currentY = startY;
		this->w = width;
		this->h = height;
		this->floatRange = range;
		this->floatSpeed = speed;
		this->floatingUp = true;
	}

	void update() {
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
	}

	void draw() {
		iShowImage(x, (int)currentY, w, h, bompImage);
	}

	bool checkCollision(int px, int py, int pSize) {
		bool overlapX = px < x + w && px + pSize > x;
		bool overlapY = py < currentY + h && py + pSize > currentY;
		return overlapX && overlapY;
	}
};







// cutter
struct Cutter {
	double x;
	int y;
	int w, h;
	double minX;
	double maxX;
	double speed;
	bool movingRight;

	// Animation variables[cite: 5]
	int frameIndex;
	int animCounter;

	Cutter(double startX = 0, int startY = 0, int width = 30, int height = 30, double minXVal = 0, double maxXVal = 100, double moveSpeed = 2.0) {
		this->x = startX;
		this->y = startY;
		this->w = width;
		this->h = height;
		this->minX = minXVal;
		this->maxX = maxXVal;
		this->speed = moveSpeed;
		this->movingRight = true;

		// Initialize animation trackers[cite: 5]
		this->frameIndex = 0;
		this->animCounter = 0;
	}

	void update() {
		if (movingRight) {
			x += speed;
			if (x + w >= maxX) {
				x = maxX - w;
				movingRight = false;
			}
		}
		else {
			x -= speed;
			if (x <= minX) {
				x = minX;
				movingRight = true;
			}
		}

		// Animation frame cycling logic[cite: 5]
		animCounter++;
		if (animCounter >= 4) {
			frameIndex++;
			if (frameIndex > 2) {
				frameIndex = 0;
			}
			animCounter = 0;
		}
	}

	void draw() {
		// Draw the current frame from the array[cite: 5]
		iShowImage((int)x, y, w, h, cutterImages[frameIndex]);
	}

	bool checkCollision(int px, int py, int pSize) {
		bool overlapX = px < x + w && px + pSize > x;
		bool overlapY = py < y + h && py + pSize > y;
		return overlapX && overlapY;
	}
};




// ============================================================================
// TIMED GATE (NEW in Level 2)
// ============================================================================
// A barrier that automatically opens and closes on a fixed timer.
// When CLOSED (red), it kills the player on touch.
// When OPEN (green outline), the player can walk through safely.
struct TimedGate {
	int x, y, w, h;
	int openDuration;    // How many frames the gate stays open
	int closedDuration;  // How many frames the gate stays closed
	int timer;           // Internal frame counter
	bool isOpen;         // Is the gate open right now?

	TimedGate(int startX = 0, int startY = 0, int width = 20, int height = 80,
		int openDur = 90, int closedDur = 60) {
		this->x = startX;
		this->y = startY;
		this->w = width;
		this->h = height;
		this->openDuration = openDur;
		this->closedDuration = closedDur;
		this->timer = 0;
		this->isOpen = false; // starts closed
	}

	void update() {
		timer++;
		int cycle = openDuration + closedDuration;
		int pos = timer % cycle;
		// First 'openDuration' frames = open, rest = closed
		isOpen = (pos < openDuration);
	}

	// Keep every new attempt fair: gates begin closed, then enter their
	// regular open/closed rhythm on the next game tick.
	void reset() {
		timer = openDuration;
		isOpen = false;
	}

	void draw() {
		if (!isOpen) {
			// Solid red barrier with gold warning border (DANGER - DO NOT TOUCH)
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(0.85f, 0.08f, 0.08f, 0.95f);
			iFilledRectangle(x, y, w, h);
			// Gold warning stripes
			glColor4f(1.0f, 0.85f, 0.0f, 0.90f);
			iRectangle(x, y, w, h);
			iRectangle(x + 2, y + 2, w - 4, h - 4);
			glDisable(GL_BLEND);
		}
		else {
			// Faint green outline only -- gate is passable
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(0.0f, 1.0f, 0.25f, 0.20f);
			iFilledRectangle(x, y, w, h);
			glColor4f(0.0f, 1.0f, 0.25f, 0.55f);
			iRectangle(x, y, w, h);
			glDisable(GL_BLEND);
		}
	}

	// Gate kills the player ONLY when it is CLOSED
	bool checkCollision(int px, int py, int pSize) {
		if (isOpen) return false;
		bool overlapX = px < x + w && px + pSize > x;
		bool overlapY = py < y + h && py + pSize > y;
		return overlapX && overlapY;
	}
};

#endif