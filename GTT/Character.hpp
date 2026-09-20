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

// External gravity reversal flag
extern bool reversedGravity;

// External matrix for collision detection
extern int mat[720][1200];

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
			// Reverse jump direction based on gravity
			if (reversedGravity) {
				this->vy = -7.2f; // Jump downward when gravity is reversed
			} else {
				this->vy = 7.2f; // Normal jump upward
			}
			playJumpSound();
			// Initial jump dust (reduced from 4 to 2 particles)
			spawnDustParticles(this->x + playerSize / 2, this->y, 2);
		}
	}

	void climbUp() {
		if (isOnStair) {
			int newY;
			if (reversedGravity) {
				// In reversed gravity, climbing up means going toward the ceiling (upward)
				newY = this->y - climbSpeed;
			} else {
				// Normal gravity, climbing up means going away from ground (upward)
				newY = this->y + climbSpeed;
			}
			
			// Check if we would hit a platform while climbing up
			int midX = this->x + (playerSize / 2);
			int checkY = newY + playerSize / 2;
			
			// Check if new position would be inside a platform
			if (checkY >= 0 && checkY < 720 && midX >= 0 && midX < 1200) {
				if (mat[checkY][midX] == 1) {
					// Stop at platform edge
					return;
				}
			}
			this->y = newY;
			this->isClimbing = true;
			this->isJumping = false;
			this->vy = 0;
		}
	}

	void climbDown(int groundY) {
		if (isOnStair) {
			int newY;
			if (reversedGravity) {
				// In reversed gravity, climbing down means going toward the floor (downward)
				newY = this->y + climbSpeed;
			} else {
				// Normal gravity, climbing down means going toward the ground
				newY = this->y - climbSpeed;
			}
			
			// Check if we would hit a platform while climbing down
			int midX = this->x + (playerSize / 2);
			int checkY = newY + playerSize / 2;
			
			// Check if new position would be inside a platform
			if (checkY >= 0 && checkY < 720 && midX >= 0 && midX < 1200) {
				if (mat[checkY][midX] == 1) {
					// Stop at platform edge
					return;
				}
			}
			this->y = newY;
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
			if (reversedGravity) {
				// In reversed gravity, ensure player is below ground (ceiling)
				if (this->y > groundY) {
					this->y = groundY;
				}
			} else {
				// In normal gravity, ensure player is above ground
				if (this->y < groundY) {
					this->y = groundY;
				}
			}
			return;
		}

		if (this->isJumping) {
			// Check for platform collision before moving
			int nextY = this->y + (int)this->vy;
			int midX = this->x + (playerSize / 2);
			
			// Check if movement would pass through a platform
			bool wouldHitPlatform = false;
			int platformY = 0;
			
			if (reversedGravity) {
				// In reversed gravity, check for platforms above (head collision)
				if (this->vy > 0) { // Moving upward
					for (int i = this->y + playerSize; i <= nextY + playerSize && i < 720; i++) {
						if (mat[i][midX] == 1) {
							wouldHitPlatform = true;
							platformY = i;
							break;
						}
					}
				}
				// Check for platforms below (feet collision)
				else if (this->vy < 0) { // Moving downward
					for (int i = this->y; i >= nextY && i >= 0; i--) {
						if (mat[i][midX] == 1) {
							wouldHitPlatform = true;
							platformY = i;
							break;
						}
					}
				}
			} else {
				// In normal gravity, check for platforms above (head collision)
				if (this->vy > 0) { // Moving upward
					for (int i = this->y + playerSize; i <= nextY + playerSize && i < 720; i++) {
						if (mat[i][midX] == 1) {
							wouldHitPlatform = true;
							platformY = i;
							break;
						}
					}
				}
				// Check for platforms below (feet collision)
				else if (this->vy < 0) { // Moving downward
					for (int i = this->y; i >= nextY && i >= 0; i--) {
						if (mat[i][midX] == 1) {
							wouldHitPlatform = true;
							platformY = i;
							break;
						}
					}
				}
			}
			
			// If hitting a platform, stop at the collision point
			if (wouldHitPlatform) {
				if (reversedGravity) {
					if (this->vy > 0) { // Hit platform from below
						this->y = platformY - playerSize;
						this->isJumping = false;
						this->vy = 0;
						spawnDustParticles(this->x + playerSize / 2, this->y + playerSize, 3);
					} else { // Hit platform from above
						this->y = platformY + playerSize;
						this->vy = 0;
					}
				} else {
					if (this->vy > 0) { // Hit platform from below
						this->y = platformY - playerSize;
						this->vy = -1;
					} else { // Hit platform from above (landing)
						this->y = platformY;
						this->isJumping = false;
						this->vy = 0;
						spawnDustParticles(this->x + playerSize / 2, this->y, 3);
					}
				}
			} else {
				// No collision, apply normal movement
				this->y = nextY;
				this->vy += this->gravity;
			}
		}
		else {
			// When not jumping, handle falling based on gravity direction
			if (reversedGravity) {
				// Reversed gravity: fall upward to ceiling (platform from below)
				// Only fall if there's space above the player
				if (this->y + playerSize < groundY - 5) {
					this->y += 3;
					if (this->y + playerSize > groundY) {
						this->y = groundY - playerSize;
						// Landing dust effect
						spawnDustParticles(this->x + playerSize / 2, this->y + playerSize, 2);
					}
				} else {
					// Keep player positioned on platform
					this->y = groundY - playerSize;
				}
			} else {
				// Normal gravity: fall downward to ground
				if (this->y > groundY + 5) {
					this->y -= 3;
					if (this->y < groundY) {
						this->y = groundY;
						// Landing dust effect
						spawnDustParticles(this->x + playerSize / 2, this->y, 2);
					}
				} else {
					// Keep player positioned on ground
					this->y = groundY;
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

		// Reverse up/down controls when gravity is reversed
		if (reversedGravity) {
			bool temp = climbUpRequested;
			climbUpRequested = climbDownRequested;
			climbDownRequested = temp;
		}

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
	int imageToDraw;
	
	// Determine which image to use based on state
	if (Player.isJumping) {
		imageToDraw = Player.isFacingRight ? Player.jumpForwardImage : Player.jumpBackwardImage;
	}
	else if (Player.isMovingRight) {
		imageToDraw = Player.runForwardImages[Player.runFrame];
	}
	else if (Player.isMovingLeft) {
		imageToDraw = Player.runBackwardImages[Player.runFrame];
	}
	else {
		imageToDraw = Player.isFacingRight ? Player.standForwardImage : Player.standBackwardImage;
	}
	
	// Apply vertical flip when gravity is reversed
	if (reversedGravity) {
		glPushMatrix();
		glTranslatef(Player.x + (playerSize - 8) / 2, Player.y + playerSize / 2, 0);
		glRotatef(180, 1, 0, 0); // Flip vertically
		glTranslatef(-(Player.x + (playerSize - 8) / 2), -(Player.y + playerSize / 2), 0);
		iShowImage(Player.x, Player.y, playerSize - 8, playerSize, imageToDraw);
		glPopMatrix();
	} else {
		iShowImage(Player.x, Player.y, playerSize - 8, playerSize, imageToDraw);
	}
}

#endif