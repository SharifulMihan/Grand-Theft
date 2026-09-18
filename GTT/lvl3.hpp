#ifndef LEVEL3_HPP
#define LEVEL3_HPP

#include <cstdlib>
#include <cmath>

// Level 3 is self-contained: it does not use the platforming Character state
// used by Levels 1 and 2.
extern int level3RoadImage;
extern int level3PlayerCarImage;
extern int level3PoliceBikeImage;
extern int level3TrafficImages[6];
extern bool isGameOver;
extern bool isLevelComplete;

const int LEVEL3_WIDTH = 1200;
const int LEVEL3_HEIGHT = 720;
const int LEVEL3_ROAD_LEFT = 320;
const int LEVEL3_ROAD_RIGHT = 880;
const int LEVEL3_LANE_WIDTH = (LEVEL3_ROAD_RIGHT - LEVEL3_ROAD_LEFT) / 4;
const int LEVEL3_MAX_TRAFFIC = 14;

struct TrafficCar {
	float x;
	float y;
	float speed;
	int lane;
	int direction;       // 1 = top to bottom, -1 = bottom to top
	int imageIndex;
	bool active;
	int w;
	int h;
};

struct Level3PoliceBike {
	float x;
	float y;
	float sideOffset;
	int w;
	int h;
};

static TrafficCar level3Traffic[LEVEL3_MAX_TRAFFIC];
static Level3PoliceBike level3PoliceBikes[2];
static float level3RoadY[2] = { 0.0f, (float)LEVEL3_HEIGHT };
static float level3PlayerX = 0.0f;
static float level3PlayerY = 145.0f;
static const int level3PlayerW = 62;
static const int level3PlayerH = 102;
static int level3ElapsedTicks = 0;
static int level3SpawnTicks = 0;
static float level3PursuitPressure = 0.0f;

inline float level3Clamp(float value, float low, float high) {
	if (value < low) return low;
	if (value > high) return high;
	return value;
}

inline float level3LaneX(int lane, int vehicleWidth) {
	return LEVEL3_ROAD_LEFT + lane * LEVEL3_LANE_WIDTH +
		(LEVEL3_LANE_WIDTH - vehicleWidth) / 2.0f;
}

inline bool level3RectCollision(float x1, float y1, float w1, float h1,
	float x2, float y2, float w2, float h2) {
	return x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2;
}

inline void level3VehicleSize(int imageIndex, int& width, int& height) {
	// The source artwork is tall; these dimensions keep every vehicle within a lane.
	if (imageIndex == 4) {
		width = 42; height = 74; // blue bike
	}
	else if (imageIndex == 2 || imageIndex == 3) {
		width = 70; height = 128; // minibuses
	}
	else {
		width = 60; height = 106; // cars
	}
}

inline bool level3SpawnIsClear(int lane, float spawnY, int height) {
	for (int i = 0; i < LEVEL3_MAX_TRAFFIC; ++i) {
		if (!level3Traffic[i].active || level3Traffic[i].lane != lane) continue;
		if (fabs(level3Traffic[i].y - spawnY) < level3Traffic[i].h + height + 90) {
			return false;
		}
	}
	return true;
}

inline bool level3SpawnTraffic() {
	int freeSlot = -1;
	for (int i = 0; i < LEVEL3_MAX_TRAFFIC; ++i) {
		if (!level3Traffic[i].active) {
			freeSlot = i;
			break;
		}
	}
	if (freeSlot < 0) return false;

	int direction = (rand() % 100 < 62) ? 1 : -1;
	int imageIndex = rand() % 6;
	int width, height;
	level3VehicleSize(imageIndex, width, height);
	float spawnY = (direction == 1) ? (float)LEVEL3_HEIGHT + 20.0f : -(float)height - 20.0f;

	// Try several random lanes so traffic never enters on top of an existing car.
	for (int attempt = 0; attempt < 8; ++attempt) {
		int lane = rand() % 4;
		if (!level3SpawnIsClear(lane, spawnY, height)) continue;
		TrafficCar& car = level3Traffic[freeSlot];
		car.lane = lane;
		car.direction = direction;
		car.imageIndex = imageIndex;
		car.w = width;
		car.h = height;
		car.x = level3LaneX(lane, width);
		car.y = spawnY;
		float difficulty = (level3ElapsedTicks > 0) ? (float)level3ElapsedTicks / 1800.0f : 0.0f;
		if (difficulty > 2.0f) difficulty = 2.0f;
		car.speed = 2.4f + (rand() % 25) / 10.0f + difficulty;
		if (direction == -1) car.speed *= 0.72f;
		car.active = true;
		return true;
	}
	return false;
}

inline void level3ResetTraffic() {
	for (int i = 0; i < LEVEL3_MAX_TRAFFIC; ++i) {
		level3Traffic[i].active = false;
	}

	// A readable opening layout makes the first seconds fair.
	level3Traffic[0] = { level3LaneX(0, 60), 485.0f, 3.4f, 0, 1, 0, true, 60, 106 };
	level3Traffic[1] = { level3LaneX(3, 70), 650.0f, 3.0f, 3, 1, 2, true, 70, 128 };
	level3Traffic[2] = { level3LaneX(1, 60), -120.0f, 2.1f, 1, -1, 1, true, 60, 106 };
}

inline void level3InitializeGame() {
	level3RoadY[0] = 0.0f;
	level3RoadY[1] = (float)LEVEL3_HEIGHT;
	level3PlayerX = level3LaneX(2, level3PlayerW);
	level3PlayerY = 145.0f;
	level3ElapsedTicks = 0;
	level3SpawnTicks = 45;
	level3PursuitPressure = 0.0f;
	level3ResetTraffic();

	level3PoliceBikes[0] = { level3PlayerX - 95.0f, 12.0f, -92.0f, 46, 84 };
	level3PoliceBikes[1] = { level3PlayerX + 105.0f, 12.0f, 92.0f, 46, 84 };
	isGameOver = false;
	isLevelComplete = false;
}

inline void level3UpdatePlayerInput() {
	const float horizontalSpeed = 6.0f;
	const float verticalSpeed = 3.0f;
	bool left = isKeyPressed('a') || isKeyPressed('A') || isSpecialKeyPressed(GLUT_KEY_LEFT);
	bool right = isKeyPressed('d') || isKeyPressed('D') || isSpecialKeyPressed(GLUT_KEY_RIGHT);
	bool up = isKeyPressed('w') || isKeyPressed('W') || isSpecialKeyPressed(GLUT_KEY_UP);
	bool down = isKeyPressed('s') || isKeyPressed('S') || isSpecialKeyPressed(GLUT_KEY_DOWN);

	if (left && !right) level3PlayerX -= horizontalSpeed;
	if (right && !left) level3PlayerX += horizontalSpeed;
	if (up && !down) level3PlayerY += verticalSpeed;
	if (down && !up) level3PlayerY -= verticalSpeed;

	level3PlayerX = level3Clamp(level3PlayerX, LEVEL3_ROAD_LEFT + 8.0f,
		LEVEL3_ROAD_RIGHT - level3PlayerW - 8.0f);
	level3PlayerY = level3Clamp(level3PlayerY, 100.0f, 270.0f);
}

inline void level3UpdateRoad() {
	float roadSpeed = 5.4f + ((level3ElapsedTicks > 1800) ? 1.0f : 0.0f);
	for (int i = 0; i < 2; ++i) {
		level3RoadY[i] -= roadSpeed;
		if (level3RoadY[i] <= -LEVEL3_HEIGHT) level3RoadY[i] += LEVEL3_HEIGHT * 2.0f;
	}
}

inline void level3UpdatePoliceBikes() {
	level3PursuitPressure += 0.006f;
	if (level3PursuitPressure > 58.0f) level3PursuitPressure = 58.0f;
	for (int i = 0; i < 2; ++i) {
		Level3PoliceBike& bike = level3PoliceBikes[i];
		float targetX = level3PlayerX + bike.sideOffset;
		targetX = level3Clamp(targetX, LEVEL3_ROAD_LEFT + 8.0f, LEVEL3_ROAD_RIGHT - bike.w - 8.0f);
		bike.x += (targetX - bike.x) * 0.075f;
		float targetY = level3PlayerY - 148.0f + level3PursuitPressure;
		bike.y += (targetY - bike.y) * 0.06f;
	}
}

inline bool level3UpdateTraffic() {
	++level3SpawnTicks;
	int spawnInterval = 78 - level3ElapsedTicks / 95;
	if (spawnInterval < 28) spawnInterval = 28;
	if (level3SpawnTicks >= spawnInterval) {
		level3SpawnTraffic();
		level3SpawnTicks = 0;
	}

	for (int i = 0; i < LEVEL3_MAX_TRAFFIC; ++i) {
		TrafficCar& car = level3Traffic[i];
		if (!car.active) continue;
		car.y += (car.direction == 1) ? -car.speed : car.speed;
		if (car.y < -car.h - 30 || car.y > LEVEL3_HEIGHT + 30) {
			car.active = false;
			continue;
		}
		if (level3RectCollision(level3PlayerX, level3PlayerY, level3PlayerW, level3PlayerH,
			car.x, car.y, car.w, car.h)) {
			return true;
		}
		for (int bike = 0; bike < 2; ++bike) {
			Level3PoliceBike& pursuer = level3PoliceBikes[bike];
			if (level3RectCollision(pursuer.x, pursuer.y, pursuer.w, pursuer.h,
				car.x, car.y, car.w, car.h)) {
				car.active = false;
				level3PursuitPressure += 1.5f;
			}
		}
	}
	return false;
}

inline bool updateLevel3Logic() {
	++level3ElapsedTicks;
	level3UpdatePlayerInput();
	level3UpdateRoad();
	level3UpdatePoliceBikes();
	if (level3UpdateTraffic()) return true;
	for (int i = 0; i < 2; ++i) {
		if (level3RectCollision(level3PlayerX, level3PlayerY, level3PlayerW, level3PlayerH,
			level3PoliceBikes[i].x, level3PoliceBikes[i].y,
			level3PoliceBikes[i].w, level3PoliceBikes[i].h)) {
			return true;
		}
	}
	return false;
}

inline void level3DrawVehicle(float x, float y, int w, int h, int image, bool facesDown) {
	if (facesDown) {
		iRotate(x + w / 2.0f, y + h / 2.0f, 180.0);
		iShowImage((int)x, (int)y, w, h, image);
		iUnRotate();
	}
	else {
		iShowImage((int)x, (int)y, w, h, image);
	}
}

inline void drawLevel3HUD() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.02f, 0.05f, 0.10f, 0.88f);
	iFilledRectangle(18, 672, 385, 38);
	glDisable(GL_BLEND);
	iSetColor(105, 220, 255);
	iRectangle(18, 672, 385, 38);
	iSetColor(255, 255, 255);
	iText(32, 695, "LEVEL 3  //  HIGHWAY PURSUIT", GLUT_BITMAP_HELVETICA_18);
	iSetColor(255, 220, 110);
	iText(850, 695, "A/D: STEER   W/S: ADJUST", GLUT_BITMAP_HELVETICA_12);
}

inline void drawLevel3() {
	// Two adjacent road segments move down together. Resetting a segment only
	// after it is fully off-screen makes the loop continuous with no gap.
	iShowImage(0, (int)level3RoadY[0], LEVEL3_WIDTH, LEVEL3_HEIGHT, level3RoadImage);
	iShowImage(0, (int)level3RoadY[1], LEVEL3_WIDTH, LEVEL3_HEIGHT, level3RoadImage);

	for (int i = 0; i < LEVEL3_MAX_TRAFFIC; ++i) {
		TrafficCar& car = level3Traffic[i];
		if (car.active) level3DrawVehicle(car.x, car.y, car.w, car.h,
			level3TrafficImages[car.imageIndex], car.direction == 1);
	}
	for (int i = 0; i < 2; ++i) {
		Level3PoliceBike& bike = level3PoliceBikes[i];
		iShowImage((int)bike.x, (int)bike.y, bike.w, bike.h, level3PoliceBikeImage);
	}
	iShowImage((int)level3PlayerX, (int)level3PlayerY, level3PlayerW, level3PlayerH, level3PlayerCarImage);
	drawLevel3HUD();
}

#endif
