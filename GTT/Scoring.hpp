#ifndef SCORING_HPP
#define SCORING_HPP

#include <iostream>
#include <cmath>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#include "iGraphics.h"
#include "Effects.hpp"

using namespace std;

// ============================================================================
// AUDIO SYSTEM
// ============================================================================
inline void playPointSound() {
	mciSendString("close pointSound", NULL, 0, NULL);
	mciSendString("open \"Sound/point.mp3\" type mpegvideo alias pointSound", NULL, 0, NULL);
	mciSendString("play pointSound", NULL, 0, NULL);
}

inline void playBombSound() {
	mciSendString("close bombSound", NULL, 0, NULL);
	mciSendString("open \"Sound/bomb.mp3\" type mpegvideo alias bombSound", NULL, 0, NULL);
	mciSendString("play bombSound", NULL, 0, NULL);
}

// ============================================================================
// CASH COLLECTIBLES (Replaces Gems)
// ============================================================================
extern int cashImage;

struct Gem {
	int x, y;
	int size;
	bool isCollected;
	double animTimer;

	Gem(int startX = 0, int startY = 0, int gemSize = 14) {
		this->x = startX;
		this->y = startY;
		this->size = gemSize;
		this->isCollected = false;
		this->animTimer = (rand() % 100) / 10.0;
	}

	void update() {
		if (!isCollected) {
			animTimer += 0.05;
		}
	}

	void draw() {
		if (isCollected) return;

		double offset = sin(animTimer) * 3.5;
		double cy = y + offset;

		if (cashImage > 0) {
			int cashW = 30;
			int cashH = 26;
			iShowImage(x - cashW / 2, (int)cy - cashH / 2, cashW, cashH, cashImage);
		}
		else {
			// Fallback cyan diamond shape using polygon if image is missing
			double px[4] = { (double)x, (double)x + size / 2.0, (double)x, (double)x - size / 2.0 };
			double py[4] = { cy + size / 2.0, cy, cy - size / 2.0, cy };

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(0.2f, 0.9f, 1.0f, 0.95f);
			iFilledPolygon(px, py, 4);
			glDisable(GL_BLEND);
		}
	}

	bool checkCollision(int px, int py, int pSize) {
		if (isCollected) return false;
		bool overlapX = px < x + size && px + pSize > x - size / 2;
		bool overlapY = py < y + size && py + pSize > y - size / 2;
		if (overlapX && overlapY) {
			isCollected = true;
			return true;
		}
		return false;
	}
};


// ============================================================================
// SCORE AND STAR RATING SYSTEM
// ============================================================================
static int playerScore = 0;
static int gemsCollected = 0;
static int totalGemsInLevel = 3;

inline void resetScore() {
	playerScore = 0;
	gemsCollected = 0;
}

// Draw 5-pointed star icon using OpenGL polygon
inline void drawStar(double cx, double cy, double radius, bool filled) {
	double px[10], py[10];
	double pi = acos(-1.0);

	for (int i = 0; i < 10; i++) {
		double r = (i % 2 == 0) ? radius : radius * 0.4;
		double angle = i * (pi / 5.0) - (pi / 2.0);
		px[i] = cx + r * cos(angle);
		py[i] = cy + r * sin(angle);
	}

	if (filled) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(1.0f, 0.85f, 0.0f, 0.95f); // Golden yellow
		iFilledPolygon(px, py, 10);
		glColor4f(1.0f, 1.0f, 0.6f, 0.8f);  // Inner shine
		iPolygon(px, py, 10);
		glDisable(GL_BLEND);
	}
	else {
		iSetColor(100, 100, 120);
		iPolygon(px, py, 10);
	}
}

// Draw 3 stars for the completion screen
inline void drawStarsRating(double x, double y, int starCount) {
	for (int i = 0; i < 3; i++) {
		drawStar(x + i * 45, y, 18.0, i < starCount);
	}
}

// Calculate earned stars based on performance
inline int calculateEarnedStars(int remainingTime, int collectedGems, int maxGems) {
	int stars = 1; // 1 star for escaping alive
	if (collectedGems >= maxGems) stars++; // 2nd star for getting all gems
	if (remainingTime >= 20) stars++;      // 3rd star for fast escape (20+ sec left)
	return stars;
}

#endif // SCORING_HPP