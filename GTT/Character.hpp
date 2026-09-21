#ifndef CHARACTER_H
#define CHARACTER_H

#include <cstdio> 

#include "UI.hpp"

const int playerSize = 30;
const int moveSpeed = 2;
const int climbSpeed = 2;
const int animSpeed = 2;

extern bool reversedGravity;

extern int mat[720][1200];
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
	bool isCarryingLoot;
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
			if (reversedGravity) {
				this->vy = -7.2f;
			} else {
				this->vy = 7.2f;
			}
			playJumpSound();
			spawnDustParticles(this->x + playerSize / 2, this->y, 2);
		}
	}

	void climbUp() {
		if (isOnStair) {
			int newY;
			if (reversedGravity) {
				newY = this->y - climbSpeed;
			} else {
				newY = this->y + climbSpeed;
			}
			
			int midX = this->x + (playerSize / 2);
			int checkY = newY + playerSize / 2;
			
			if (checkY >= 0 && checkY < 720 && midX >= 0 && midX < 1200) {
				if (mat[checkY][midX] == 1) {
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
				newY = this->y + climbSpeed;
			} else {
				newY = this->y - climbSpeed;
			}
			
			int midX = this->x + (playerSize / 2);
			int checkY = newY + playerSize / 2;
			
			if (checkY >= 0 && checkY < 720 && midX >= 0 && midX < 1200) {
				if (mat[checkY][midX] == 1) {
					return;
				}
			}
			this->y = newY;
			this->isClimbing = true;
			this->isJumping = false;
			this->vy = 0;
		}
	}


	void updatePhysics(int groundY) {
		if (this->isOnStair) {
			this->isJumping = false;
			this->vy = 0;
			if (reversedGravity) {
				if (this->y > groundY) {
					this->y = groundY;
				}
			} else {
				if (this->y < groundY) {
					this->y = groundY;
				}
			}
			return;
		}

		if (this->isJumping) {
			int nextY = this->y + (int)this->vy;
			int midX = this->x + (playerSize / 2);
			
			bool wouldHitPlatform = false;
			int platformY = 0;
			
			if (reversedGravity) {
				if (this->vy > 0) {
					for (int i = this->y + playerSize; i <= nextY + playerSize && i < 720; i++) {
						if (mat[i][midX] == 1) {
							wouldHitPlatform = true;
							platformY = i;
							break;
						}
					}
				}
				else if (this->vy < 0) {
					for (int i = this->y; i >= nextY && i >= 0; i--) {
						if (mat[i][midX] == 1) {
							wouldHitPlatform = true;
							platformY = i;
							break;
						}
					}
				}
			} else {
				if (this->vy > 0) {
					for (int i = this->y + playerSize; i <= nextY + playerSize && i < 720; i++) {
						if (mat[i][midX] == 1) {
							wouldHitPlatform = true;
							platformY = i;
							break;
						}
					}
				}
				else if (this->vy < 0) {
					for (int i = this->y; i >= nextY && i >= 0; i--) {
						if (mat[i][midX] == 1) {
							wouldHitPlatform = true;
							platformY = i;
							break;
						}
					}
				}
			}
			
			if (wouldHitPlatform) {
				if (reversedGravity) {
					if (this->vy > 0) {
						this->y = platformY - playerSize;
						this->isJumping = false;
						this->vy = 0;
						spawnDustParticles(this->x + playerSize / 2, this->y + playerSize, 3);
					} else {
						this->y = platformY + playerSize;
						this->vy = 0;
					}
				} else {
					if (this->vy > 0) {
						this->y = platformY - playerSize;
						this->vy = -1;
					} else {
						this->y = platformY;
						this->isJumping = false;
						this->vy = 0;
						spawnDustParticles(this->x + playerSize / 2, this->y, 3);
					}
				}
			} else {
				this->y = nextY;
				this->vy += this->gravity;
			}
		}
		else {
			if (reversedGravity) {
				if (this->y + playerSize < groundY - 5) {
					this->y += 3;
					if (this->y + playerSize > groundY) {
						this->y = groundY - playerSize;
						spawnDustParticles(this->x + playerSize / 2, this->y + playerSize, 2);
					}
				} else {
					this->y = groundY - playerSize;
				}
			} else {
				if (this->y > groundY + 5) {
					this->y -= 3;
					if (this->y < groundY) {
						this->y = groundY;
						spawnDustParticles(this->x + playerSize / 2, this->y, 2);
					}
				} else {
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


void handlePlayerInput(int groundY) {

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


	bool jumpRequested = isKeyPressed(' ') || GameController.isJumpPressed();
	if (jumpRequested) {
		Player.jump();
	}
	else {
		Player.hasPlayedJumpSound = false;
	}


	Player.isClimbing = false;
	if (Player.isOnStair) {
		bool climbUpRequested = isKeyPressed('w') || isKeyPressed('W') || isSpecialKeyPressed(GLUT_KEY_UP) || GameController.isUp();
		bool climbDownRequested = isKeyPressed('s') || isKeyPressed('S') || isSpecialKeyPressed(GLUT_KEY_DOWN) || GameController.isDown();


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


		{
			int midX = Player.x + (playerSize / 2);
			int feetY = Player.y;
			int centerY = Player.y + (playerSize / 2);
			if (reversedGravity) {
				int headY = Player.y + playerSize;
				Player.isOnStair = (mat[centerY][midX] == 2 || mat[headY][midX] == 2);
			} else {
				Player.isOnStair = (mat[centerY][midX] == 2 || mat[feetY][midX] == 2);
			}
		}
	}


	Player.updatePhysics(groundY);
	Player.updateAnimationFrame();
}

 
void playerMovements() {
	int imageToDraw;
	

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
	

	if (reversedGravity) {
		glPushMatrix();
		glTranslatef(Player.x + (playerSize - 8) / 2, Player.y + playerSize / 2, 0);
		glRotatef(180, 1, 0, 0);
		glTranslatef(-(Player.x + (playerSize - 8) / 2), -(Player.y + playerSize / 2), 0);
		iShowImage(Player.x, Player.y, playerSize - 8, playerSize, imageToDraw);
		glPopMatrix();
	} else {
		iShowImage(Player.x, Player.y, playerSize - 8, playerSize, imageToDraw);
	}
}

#endif