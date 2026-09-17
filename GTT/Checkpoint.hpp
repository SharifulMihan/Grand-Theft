#ifndef CHECKPOINT_HPP
#define CHECKPOINT_HPP

struct Checkpoint {
	bool isActive = false;
	int x = 15;
	int y = 160;
	int levelID = 1;

	// Sets checkpoint coordinates for a specific level
	void setCheckpoint(int newX, int newY, int level) {
		isActive = true;
		x = newX;
		y = newY;
		levelID = level;
	}

	// Loads spawn coordinates and key status into level/player variables
	void loadSpawnPoint(int level, int &playerX, int &playerY, bool &keyState, int defaultX = 15, int defaultY = 160) {
		if (isActive && levelID == level) {
			playerX = x;
			playerY = y;
			keyState = true; // Key stays collected when respawning at checkpoint
		}
		else {
			playerX = defaultX;
			playerY = defaultY;
			keyState = false;
		}
	}

	// Clears checkpoint memory
	void clear() {
		isActive = false;
		x = 15;
		y = 160;
		levelID = 1;
	}
};

static Checkpoint GameCheckpoint;

#endif // CHECKPOINT_HPP