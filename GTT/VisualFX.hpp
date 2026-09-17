#ifndef VISUALFX_HPP
#define VISUALFX_HPP

#include <iostream>
#include <cmath>
#include "iGraphics.h"

using namespace std;

static double globalTimer = 0.0;

inline void updateGlobalTimer() {
    globalTimer += 0.08;
    if (globalTimer > 10000.0) globalTimer = 0.0;
}

// ============================================================================
// 1. PULSING LASER GLOW
// ============================================================================
// Renders a high-tech glowing laser beam with multi-layered translucency
inline void drawGlowingLaser(int x, int y, int w, int h, bool isRed = true) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    double pulse = sin(globalTimer * 3.0) * 2.0;

    // Layer 1: Wide outer ambient aura glow
    if (isRed) {
        glColor4f(1.0f, 0.0f, 0.2f, 0.25f);
    } else {
        glColor4f(0.0f, 0.6f, 1.0f, 0.25f);
    }
    iFilledRectangle(x - 2, y - 3 + pulse, w + 4, h + 6 - pulse);

    // Layer 2: Medium semi-bright core glow
    if (isRed) {
        glColor4f(1.0f, 0.2f, 0.2f, 0.65f);
    } else {
        glColor4f(0.2f, 0.8f, 1.0f, 0.65f);
    }
    iFilledRectangle(x, y - 1, w, h + 2);

    // Layer 3: Hot white center beam
    glColor4f(1.0f, 1.0f, 1.0f, 0.95f);
    iFilledRectangle(x + 2, y + (h / 4), w - 4, h / 2);

    glDisable(GL_BLEND);
}


// ============================================================================
// 2. RED ALARM SCREEN OVERLAY
// ============================================================================
// Emergency red warning light overlay during loot escape phase
inline void drawAlarmOverlay(bool isActive) {
    if (!isActive) return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Oscillating opacity between 0.05 and 0.25 for flashing alarm effect
    float alpha = (float)(0.15 + 0.10 * sin(globalTimer * 4.0));
    glColor4f(1.0f, 0.0f, 0.0f, alpha);
    iFilledRectangle(0, 0, 1200, 860);

    glDisable(GL_BLEND);
}


// ============================================================================
// 3. PARALLAX BACKGROUND SILHOUETTE
// ============================================================================
// Multi-layered cityscape background with glowing building windows
inline void drawParallaxBackground(int playerX) {
    double parallaxOffset = playerX * 0.03;

    // Far Layer: Dark city building silhouettes
    iSetColor(12, 16, 28);
    for (int i = 0; i < 10; i++) {
        int bx = (int)(i * 140 - parallaxOffset);
        int bw = 90;
        int bh = 220 + (i % 3) * 60;
        iFilledRectangle(bx, 150, bw, bh);

        // Lit building windows
        iSetColor(220, 200, 100);
        for (int wy = 170; wy < 150 + bh - 20; wy += 25) {
            for (int wx = bx + 12; wx < bx + bw - 15; wx += 20) {
                if ((i + wy + wx) % 7 != 0) { // Random pattern of lit windows
                    iFilledRectangle(wx, wy, 10, 12);
                }
            }
        }
        iSetColor(12, 16, 28);
    }
}


// ============================================================================
// 4. VIGNETTE CINEMATIC BORDER
// ============================================================================
// Subtle dark gradient edges for immersive spy atmosphere
inline void drawVignette() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Top border shadow
    glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
    iFilledRectangle(0, 840, 1200, 20);
    glColor4f(0.0f, 0.0f, 0.0f, 0.2f);
    iFilledRectangle(0, 820, 1200, 20);

    // Bottom border shadow
    glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
    iFilledRectangle(0, 0, 1200, 20);

    // Side shadows
    glColor4f(0.0f, 0.0f, 0.0f, 0.3f);
    iFilledRectangle(0, 0, 20, 860);
    iFilledRectangle(1180, 0, 20, 860);

    glDisable(GL_BLEND);
}


// ============================================================================
// 5. SECURITY CAMERA / SPOTLIGHT CONE
// ============================================================================
// Sweeping security spotlight that detects the thief
struct SecurityCamera {
    double pivotX, pivotY; // Mounting location on ceiling/wall
    double angle;          // Current sweep angle in degrees
    double sweepMin;       // Min angle boundary
    double sweepMax;       // Max angle boundary
    double speed;          // Sweep speed
    bool sweepingRight;
    double coneLength;     // Length of light beam
    double coneSpread;     // Angular spread of light cone

    SecurityCamera(double px = 300, double py = 520, double minA = -35.0, double maxA = 35.0, double spd = 0.8, double len = 220.0) {
        this->pivotX = px;
        this->pivotY = py;
        this->angle = minA;
        this->sweepMin = minA;
        this->sweepMax = maxA;
        this->speed = spd;
        this->sweepingRight = true;
        this->coneLength = len;
        this->coneSpread = 22.0; // degrees
    }

    void update() {
        if (sweepingRight) {
            angle += speed;
            if (angle >= sweepMax) {
                angle = sweepMax;
                sweepingRight = false;
            }
        } else {
            angle -= speed;
            if (angle <= sweepMin) {
                angle = sweepMin;
                sweepingRight = true;
            }
        }
    }

    void draw() {
        // Draw camera housing bracket
        iSetColor(60, 65, 80);
        iFilledCircle(pivotX, pivotY, 8, 12);

        // Calculate cone polygon vertices (triangle beam extending downward)
        double radMid = (angle - 90.0) * (3.14159 / 180.0);
        double radLeft = (angle - 90.0 - coneSpread / 2.0) * (3.14159 / 180.0);
        double radRight = (angle - 90.0 + coneSpread / 2.0) * (3.14159 / 180.0);

        double x1 = pivotX;
        double y1 = pivotY;
        double x2 = pivotX + cos(radLeft) * coneLength;
        double y2 = pivotY + sin(radLeft) * coneLength;
        double x3 = pivotX + cos(radRight) * coneLength;
        double y3 = pivotY + sin(radRight) * coneLength;

        double cx[3] = { x1, x2, x3 };
        double cy[3] = { y1, y2, y3 };

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Yellow translucent spotlight beam
        glColor4f(1.0f, 0.95f, 0.4f, 0.22f);
        iFilledPolygon(cx, cy, 3);

        // Core bright center line
        glColor4f(1.0f, 1.0f, 0.8f, 0.5f);
        iLine(pivotX, pivotY, pivotX + cos(radMid) * coneLength, pivotY + sin(radMid) * coneLength);

        glDisable(GL_BLEND);
    }

    // Check if player (px, py) intersects with spotlight cone
    bool checkDetection(int px, int py, int pSize) {
        int playerCenterX = px + pSize / 2;
        int playerCenterY = py + pSize / 2;

        double dx = playerCenterX - pivotX;
        double dy = playerCenterY - pivotY;
        double dist = sqrt(dx * dx + dy * dy);

        if (dist > coneLength) return false;

        // Angle from camera to player
        double playerAngle = atan2(dy, dx) * (180.0 / 3.14159);
        double targetAngle = angle - 90.0;

        // Wrap angle difference
        double diff = fabs(playerAngle - targetAngle);
        while (diff > 180.0) diff -= 360.0;
        diff = fabs(diff);

        return diff <= (coneSpread / 2.0 + 5.0);
    }
};

#endif 
