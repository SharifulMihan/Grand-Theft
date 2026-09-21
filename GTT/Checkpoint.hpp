#ifndef CHECKPOINT_HPP
#define CHECKPOINT_HPP

struct Checkpoint {
	bool isActive = false;
	int x = 15;
	int y = 160;
	int levelID = 1;
	bool inSubLevel = false;
	void setCheckpoint(int newX, int newY, int level, bool subLevel = false) {
		isActive = true;
		x = newX;
		y = newY;
		levelID = level;
		inSubLevel = subLevel;
	}


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


	bool getSubLevelState() {
		if (isActive && levelID == 3) {
			return inSubLevel;
		}
		return false;
	}


	void clear() {
		isActive = false;
		x = 15;
		y = 160;
		levelID = 1;
		inSubLevel = false;
	}
};

static Checkpoint GameCheckpoint;

#endif // CHECKPOINT_HPP