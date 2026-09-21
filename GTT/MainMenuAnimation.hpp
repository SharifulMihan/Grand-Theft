#ifndef MAINMENUANIMATION_HPP
#define MAINMENUANIMATION_HPP

#include <stdio.h>
#include <string.h>

#define ANIMATION_FRAMES 36
#define ANIMATION_SPEED 10

int menuCharacterFrames[ANIMATION_FRAMES];
int currentAnimationFrame = 0;
int animationTimer = 0;
int characterX = 550;
int characterY = 90;
int characterWidth = 220;
int characterHeight = 350;

void loadMenuCharacterAnimation() {
	for (int i = 0; i < ANIMATION_FRAMES; i++) {
		char imagePath[64];
		sprintf_s(imagePath, sizeof(imagePath), "Images/MenuPageCharacter/frame_%02d.png", i + 1);
		menuCharacterFrames[i] = iLoadImage(imagePath);
	}
}

void updateMenuCharacterAnimation() {
	animationTimer++;
	if (animationTimer >= ANIMATION_SPEED) {
		animationTimer = 0;
		currentAnimationFrame++;
		if (currentAnimationFrame >= ANIMATION_FRAMES) {
			currentAnimationFrame = 0;
		}
	}
}

void drawMenuCharacterAnimation() {
	if (menuCharacterFrames[currentAnimationFrame] > 0) {
		iShowImage(characterX, characterY, characterWidth, characterHeight, menuCharacterFrames[currentAnimationFrame]);
	}
}

#endif