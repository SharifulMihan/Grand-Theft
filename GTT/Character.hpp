#ifndef CHARACTER_H
#define CHARACTER_H

#include <iostream>
#include <cstdio> 

#include "UI.hpp"


using namespace std;

const int playerSize = 30;
const int moveSpeed = 2;
const int climbSpeed = 2;
const int animSpeed = 2;

// player class
class Character {
public:
	int x;
	int y;

	int runFrame;
	int animFrameTimer;

	bool isFacingRight;
	bool isMovingRight;
	bool isMovingLeft;

	bool isJumping;
	float vy;
	float gravity;

	bool isOnStair;
	bool isClimbing;

	bool hasPlayedJumpSound;
	bool isCarryingLoot; // Loot weight flag (makes player slightly heavier)

	// Images
	int runForwardImages[6];
	int runBackwardImages[6];
	int jumpForwardImage, jumpBackwardImage;
	int standForwardImage, standBackwardImage;

	Character() {
		this->x = 0;
		this->y = 0;
		this->runFrame = 0;
		this->animFrameTimer = 0;
		this->isFacingRight = true;
		this->isMovingRight = false;
		this->isMovingLeft = false;
		this->isJumping = false;
		this->vy = 0;
		this->gravity = -0.3f;
		this->isOnStair = false;
		this->isClimbing = false;
		this->hasPlayedJumpSound = false;
		this->isCarryingLoot = false;
	}

	void runRight() {
		this->x += moveSpeed;
		this->isMovingRight = true;
		this->isMovingLeft = false;
		this->isFacingRight = true;
	}

	void runLeft() {
		this->x -= moveSpeed;
		this->isMovingLeft = true;
		this->isMovingRight = false;
		this->isFacingRight = false;
	}

	void stopHorizontal() {
		this->isMovingRight = false;
		this->isMovingLeft = false;
	}

	void jump() {
		if (!isJumping && !isOnStair) {
			this->isJumping = true;
			this->vy = 7.2f;
			playJumpSound();
			// Initial jump dust
			spawnDustParticles(this->x + playerSize / 2, this->y, 4);
		}
	}

	void climbUp() {
		if (isOnStair) {
			this->y += climbSpeed;
			this->isClimbing = true;
			this->isJumping = false;
			this->vy = 0;
		}
	}

	void climbDown(int groundY) {
		if (isOnStair) {
			if (this->y > groundY) {
				this->y -= climbSpeed;
				if (this->y < groundY) {
					this->y = groundY;
				}
			}
			this->isClimbing = true;
			this->isJumping = false;
			this->vy = 0;
		}
	}

	// Handles jumping and falling
	void updatePhysics(int groundY) {
		if (this->isOnStair) {
			this->isJumping = false;
			this->vy = 0;
			if (this->y < groundY) {
				this->y = groundY;
			}
			return;
		}

		if (this->isJumping) {
			this->y += (int)this->vy;
			this->vy += this->gravity;

			if (this->vy <= 0 && this->y <= groundY) {
				this->y = groundY;
				this->isJumping = false;
				this->vy = 0;
				// Landing dust effect
				spawnDustParticles(this->x + playerSize / 2, this->y, 6);
			}
		}
		else {
			if (this->y > groundY) {
				this->y -= 3;
				if (this->y < groundY) {
					this->y = groundY;
					// Landing dust effect
					spawnDustParticles(this->x + playerSize / 2, this->y, 5);
				}
			}
		}
	}

	void updateAnimationFrame() {
		if (isMovingRight || isMovingLeft) {
			animFrameTimer++;
			if (animFrameTimer >= animSpeed) {
				animFrameTimer = 0;
				runFrame = runFrame + 1;
				if (runFrame >= 6) {
					runFrame = 0;
				}
				// Running dust puff behind feet
				int feetX = isFacingRight ? this->x : this->x + playerSize;
				spawnDustParticles(feetX, this->y, 1);
			}
		}
		else {
			runFrame = 0;
		}
	}
};

Character Player;

// load Character Images
void loadCharacter() {
	char imagePath[100];

	for (int i = 0; i < 6; i++) {
		sprintf_s(imagePath, sizeof(imagePath), "Images/Character/Running/Forward/rR%d.png", i + 1);
		Player.runForwardImages[i] = iLoadImage(imagePath);

		sprintf_s(imagePath, sizeof(imagePath), "Images/Character/Running/Backward/rL%d.png", i + 1);
		Player.runBackwardImages[i] = iLoadImage(imagePath);
	}

	Player.jumpForwardImage = iLoadImage("Images/Character/Jump/JumpRight.png");
	Player.jumpBackwardImage = iLoadImage("Images/Character/Jump/JumpLeft.png");
	Player.standForwardImage = iLoadImage("Images/Character/Stand/IdleRight.png");
	Player.standBackwardImage = iLoadImage("Images/Character/Stand/IdleLeft.png");
}

#include "Controller.hpp"

//==================== Input Handling ====================
void handlePlayerInput(int groundY) {
	// Horizontal movement: A / D, Left / Right arrows, or Controller Left / Right
	bool moveLeft = isKeyPressed('a') || isKeyPressed('A') || isSpecialKeyPressed(GLUT_KEY_LEFT) || GameController.isLeft();
	bool moveRight = isKeyPressed('d') || isKeyPressed('D') || isSpecialKeyPressed(GLUT_KEY_RIGHT) || GameController.isRight();

	if (moveLeft && !moveRight) {
		Player.runLeft();
	}
	else if (moveRight && !moveLeft) {
		Player.runRight();
	}
	else {
		Player.stopHorizontal();
	}

	// Jump: Space bar on keyboard, OR strictly Xbox A / PS4 X on controller
	bool jumpRequested = isKeyPressed(' ') || GameController.isJumpPressed();
	if (jumpRequested) {
		Player.jump();
	}
	else {
		Player.hasPlayedJumpSound = false;
	}

	// Stair climbing: W / S, Up / Down arrows, or Controller Up / Down
	Player.isClimbing = false;
	if (Player.isOnStair) {
		bool climbUpRequested = isKeyPressed('w') || isKeyPressed('W') || isSpecialKeyPressed(GLUT_KEY_UP) || GameController.isUp();
		bool climbDownRequested = isKeyPressed('s') || isKeyPressed('S') || isSpecialKeyPressed(GLUT_KEY_DOWN) || GameController.isDown();

		if (climbUpRequested) {
			Player.climbUp();
		}
		else if (climbDownRequested) {
			Player.climbDown(groundY);
		}
	}

	// physics + animation update
	Player.updatePhysics(groundY);
	Player.updateAnimationFrame();
}

// character Drawing 
void playerMovements() {
	if (Player.isJumping) {
		if (Player.isFacingRight)
			iShowImage(Player.x, Player.y, playerSize - 8, playerSize, Player.jumpForwardImage);
		else
			iShowImage(Player.x, Player.y, playerSize - 8, playerSize, Player.jumpBackwardImage);
	}
	else if (Player.isMovingRight) {
		iShowImage(Player.x, Player.y, playerSize - 8, playerSize, Player.runForwardImages[Player.runFrame]);
	}
	else if (Player.isMovingLeft) {
		iShowImage(Player.x, Player.y, playerSize - 8, playerSize, Player.runBackwardImages[Player.runFrame]);
	}
	else {
		if (Player.isFacingRight)
			iShowImage(Player.x, Player.y, playerSize - 8, playerSize, Player.standForwardImage);
		else
			iShowImage(Player.x, Player.y, playerSize - 8, playerSize, Player.standBackwardImage);
	}
}

#endif