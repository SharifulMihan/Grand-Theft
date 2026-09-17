#ifndef EFFECTS_HPP
#define EFFECTS_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include "iGraphics.h"

using namespace std;

// ============================================================================
// 1. PARTICLE SYSTEM
// ============================================================================
// Represents a single visual particle (dust, sparkle, coin, smoke, spark).
struct Particle {
	double x, y;          // Current position in screen coordinates
	double vx, vy;        // Velocity (movement per frame)
	double gravity;       // Downward acceleration per frame
	int life;             // Remaining frames to live
	int maxLife;          // Total lifetime in frames (used for fade-out calculation)
	double size;          // Particle radius / size
	double sizeDelta;     // Size change per frame (e.g. shrinking or expanding)
	int r, g, b;          // Color RGB (0-255)
	float alpha;          // Opacity (0.0f = invisible, 1.0f = fully opaque)
	bool isCoin;          // Special flag for gold coin shape rendering
	double rotation;      // Rotation angle for coins/sparkles
	double rotSpeed;      // Rotation speed per frame
};

const int MAX_PARTICLES = 300;
static Particle particles[MAX_PARTICLES];
static int particleCount = 0;

// Update all active particles (physics, lifetime, fading)
inline void updateParticles() {
	for (int i = 0; i < particleCount; i++) {
		particles[i].x += particles[i].vx;
		particles[i].y += particles[i].vy;
		particles[i].vy += particles[i].gravity;
		particles[i].size += particles[i].sizeDelta;
		if (particles[i].size < 0.5) particles[i].size = 0.5;

		particles[i].rotation += particles[i].rotSpeed;
		particles[i].life--;

		// Calculate opacity fade out based on remaining life
		particles[i].alpha = (float)particles[i].life / (float)particles[i].maxLife;

		// Remove dead particles by swapping with the last particle in array
		if (particles[i].life <= 0) {
			particles[i] = particles[particleCount - 1];
			particleCount--;
			i--;
		}
	}
}

// Render all active particles on screen using OpenGL alpha blending
inline void drawParticles() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (int i = 0; i < particleCount; i++) {
		Particle &p = particles[i];

		if (p.isCoin) {
			// Draw glowing gold coin particle
			glColor4f(1.0f, 0.85f, 0.0f, p.alpha);
			iFilledCircle(p.x, p.y, p.size, 12);
			glColor4f(1.0f, 1.0f, 0.6f, p.alpha);
			iFilledCircle(p.x, p.y, p.size * 0.5, 8);
		}
		else {
			// Standard smooth circular particle
			glColor4f((float)p.r / 255.0f, (float)p.g / 255.0f, (float)p.b / 255.0f, p.alpha);
			iFilledCircle(p.x, p.y, p.size, 10);
		}
	}

	glDisable(GL_BLEND);
}

// Clear all active particles (reset level)
inline void clearParticles() {
	particleCount = 0;
}

// Helper to add a particle if space allows
inline void addParticle(Particle p) {
	if (particleCount < MAX_PARTICLES) {
		particles[particleCount++] = p;
	}
}

// --- EMITTER PRESETS ---

// 1. Dust Particles (spawned when player lands or runs)
inline void spawnDustParticles(double x, double y, int count = 6) {
	for (int i = 0; i < count; i++) {
		Particle p;
		p.x = x + (rand() % 20 - 10);
		p.y = y + (rand() % 4);
		p.vx = ((rand() % 100) / 50.0 - 1.0) * 1.5;
		p.vy = ((rand() % 100) / 100.0) * 1.5 + 0.5;
		p.gravity = -0.05;
		p.life = 15 + rand() % 10;
		p.maxLife = p.life;
		p.size = 3.0 + (rand() % 3);
		p.sizeDelta = 0.1; // grows slightly like dust cloud
		p.r = 180 + rand() % 40;
		p.g = 170 + rand() % 30;
		p.b = 150 + rand() % 30;
		p.alpha = 0.8f;
		p.isCoin = false;
		p.rotation = 0;
		p.rotSpeed = 0;
		addParticle(p);
	}
}

// 2. Key Sparkles (sparkles floating around the uncollected key)
inline void spawnKeySparkles(double x, double y) {
	if (rand() % 3 != 0) return; // limit density
	Particle p;
	p.x = x + (rand() % 24 - 4);
	p.y = y + (rand() % 24 - 4);
	p.vx = ((rand() % 100) / 100.0 - 0.5) * 0.6;
	p.vy = ((rand() % 100) / 100.0) * 0.8 + 0.2;
	p.gravity = 0.01;
	p.life = 20 + rand() % 15;
	p.maxLife = p.life;
	p.size = 2.0 + (rand() % 2);
	p.sizeDelta = -0.05;
	p.r = 255;
	p.g = 220 + rand() % 35;
	p.b = 50;
	p.alpha = 1.0f;
	p.isCoin = false;
	p.rotation = 0;
	p.rotSpeed = 0;
	addParticle(p);
}

// 3. Coin Burst (burst of shiny coins when lootbox is opened)
inline void spawnCoinBurst(double x, double y, int count = 18) {
	for (int i = 0; i < count; i++) {
		Particle p;
		p.x = x;
		p.y = y;
		double angle = ((rand() % 360) * 3.14159) / 180.0;
		double speed = 3.0 + (rand() % 40) / 10.0;
		p.vx = cos(angle) * speed;
		p.vy = sin(angle) * speed + 2.0; // upward bias
		p.gravity = -0.25;
		p.life = 35 + rand() % 20;
		p.maxLife = p.life;
		p.size = 4.0 + (rand() % 3);
		p.sizeDelta = 0;
		p.r = 255;
		p.g = 215;
		p.b = 0;
		p.alpha = 1.0f;
		p.isCoin = true;
		p.rotation = rand() % 360;
		p.rotSpeed = 5.0 + rand() % 10;
		addParticle(p);
	}
}

// 4. Explosion Smoke (when player hits bomb or laser)
inline void spawnExplosionSmoke(double x, double y, int count = 25) {
	for (int i = 0; i < count; i++) {
		Particle p;
		p.x = x;
		p.y = y;
		double angle = ((rand() % 360) * 3.14159) / 180.0;
		double speed = 1.0 + (rand() % 50) / 10.0;
		p.vx = cos(angle) * speed;
		p.vy = sin(angle) * speed;
		p.gravity = 0.05;
		p.life = 25 + rand() % 20;
		p.maxLife = p.life;
		p.size = 5.0 + (rand() % 6);
		p.sizeDelta = 0.2; // smoke expands
		if (rand() % 2 == 0) {
			p.r = 255; p.g = 100 + rand() % 100; p.b = 20; // fire flame
		}
		else {
			p.r = 80 + rand() % 40; p.g = p.r; p.b = p.r;  // dark smoke
		}
		p.alpha = 0.9f;
		p.isCoin = false;
		p.rotation = 0;
		p.rotSpeed = 0;
		addParticle(p);
	}
}


// ============================================================================
// 2. SCREEN SHAKE SYSTEM
// ============================================================================
// Adds impact & feedback when player dies, triggers alarm, or explodes.
static int shakeTimer = 0;
static int shakeIntensity = 0;
static int shakeOffsetX = 0;
static int shakeOffsetY = 0;

// Trigger screen shake with a specific intensity and frame duration
inline void triggerShake(int intensity = 8, int durationFrames = 12) {
	shakeIntensity = intensity;
	shakeTimer = durationFrames;
}

// Fully clear shake state (call when a level is (re)initialized so a death's
// leftover shake never bleeds into the next attempt)
inline void resetShake() {
	shakeTimer = 0;
	shakeIntensity = 0;
	shakeOffsetX = 0;
	shakeOffsetY = 0;
}

// Update shake offset per frame
inline void updateShake() {
	if (shakeTimer > 0) {
		shakeTimer--;
		shakeOffsetX = (rand() % (shakeIntensity * 2 + 1)) - shakeIntensity;
		shakeOffsetY = (rand() % (shakeIntensity * 2 + 1)) - shakeIntensity;
	}
	else {
		shakeOffsetX = 0;
		shakeOffsetY = 0;
	}
}

// Apply shake translation before drawing world objects
inline void applyScreenShake() {
	if (shakeOffsetX != 0 || shakeOffsetY != 0) {
		glPushMatrix();
		glTranslatef((float)shakeOffsetX, (float)shakeOffsetY, 0.0f);
	}
}

// Reset shake matrix after drawing world objects
inline void resetScreenShake() {
	if (shakeOffsetX != 0 || shakeOffsetY != 0) {
		glPopMatrix();
	}
}


// ============================================================================
// 3. FLOATING TEXT SYSTEM
// ============================================================================
// Renders dynamic pop-up messages like "+500 GEMS!", "KEY ACQUIRED!"
struct FloatingText {
	char text[64];
	double x, y;
	double vy;
	int life;
	int maxLife;
	int r, g, b;
	bool active;
};

const int MAX_FLOATING_TEXTS = 10;
static FloatingText floatingTexts[MAX_FLOATING_TEXTS];

inline void initFloatingTexts() {
	for (int i = 0; i < MAX_FLOATING_TEXTS; i++) {
		floatingTexts[i].active = false;
	}
}

inline void spawnFloatingText(double x, double y, const char* str, int r = 255, int g = 215, int b = 0) {
	for (int i = 0; i < MAX_FLOATING_TEXTS; i++) {
		if (!floatingTexts[i].active) {
			strcpy_s(floatingTexts[i].text, sizeof(floatingTexts[i].text), str);
			floatingTexts[i].x = x;
			floatingTexts[i].y = y;
			floatingTexts[i].vy = 1.2;
			floatingTexts[i].life = 40;
			floatingTexts[i].maxLife = 40;
			floatingTexts[i].r = r;
			floatingTexts[i].g = g;
			floatingTexts[i].b = b;
			floatingTexts[i].active = true;
			break;
		}
	}
}

inline void updateFloatingTexts() {
	for (int i = 0; i < MAX_FLOATING_TEXTS; i++) {
		if (floatingTexts[i].active) {
			floatingTexts[i].y += floatingTexts[i].vy;
			floatingTexts[i].life--;
			if (floatingTexts[i].life <= 0) {
				floatingTexts[i].active = false;
			}
		}
	}
}

inline void drawFloatingTexts() {
	for (int i = 0; i < MAX_FLOATING_TEXTS; i++) {
		if (floatingTexts[i].active) {
			FloatingText &ft = floatingTexts[i];
			iSetColor(ft.r, ft.g, ft.b);
			iText((int)ft.x, (int)ft.y, ft.text, GLUT_BITMAP_HELVETICA_18);
		}
	}
}

#endif // EFFECTS_HPP