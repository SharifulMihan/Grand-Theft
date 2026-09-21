#ifndef SCOREBOARD_HPP
#define SCOREBOARD_HPP

#include <stdio.h>
#include <string.h>

#define MAX_PLAYERS 100
#define MAX_USERNAME_LEN 50

typedef struct {
	char username[MAX_USERNAME_LEN];
	int level1Deaths;
	int level2Deaths;
	int level3Deaths;
} PlayerScore;

PlayerScore leaderboard[MAX_PLAYERS];
int leaderboardCount = 0;
char currentUsername[MAX_USERNAME_LEN] = "";
int currentLevelDeaths = 0;

void loadScoreboard() {
	leaderboardCount = 0;
	FILE* file;
	errno_t err = fopen_s(&file, "ScoreBoard.txt", "r");
	
	if (err != 0 || file == NULL) {
		FILE* createFile;
		errno_t createErr = fopen_s(&createFile, "ScoreBoard.txt", "w");
		if (createErr == 0 && createFile != NULL) {
			fclose(createFile);
		}
		return;
	}

	char line[256];
	while (fgets(line, sizeof(line), file) != NULL) {
		if (strlen(line) == 0 || line[0] == '\n') continue;
		
		char name[MAX_USERNAME_LEN];
		int l1, l2, l3;
		
		if (sscanf_s(line, "%s %d %d %d", name, (unsigned int)sizeof(name), &l1, &l2, &l3) == 4) {
			if (leaderboardCount < MAX_PLAYERS) {
				strcpy_s(leaderboard[leaderboardCount].username, sizeof(leaderboard[leaderboardCount].username), name);
				leaderboard[leaderboardCount].level1Deaths = l1;
				leaderboard[leaderboardCount].level2Deaths = l2;
				leaderboard[leaderboardCount].level3Deaths = l3;
				leaderboardCount++;
			}
		}
	}
	
	fclose(file);
}

void saveScoreboard() {
	FILE* file;
	errno_t err = fopen_s(&file, "ScoreBoard.txt", "w");
	
	if (err != 0 || file == NULL) {
		return;
	}

	for (int i = 0; i < leaderboardCount; i++) {
		fprintf(file, "%s %d %d %d\n", 
				leaderboard[i].username, 
				leaderboard[i].level1Deaths, 
				leaderboard[i].level2Deaths, 
				leaderboard[i].level3Deaths);
	}
	
	fclose(file);
}

void updatePlayerScore(int levelNum, int deaths) {
	int found = 0;
	int foundIndex = -1;
	
	for (int i = 0; i < leaderboardCount; i++) {
		if (strcmp(leaderboard[i].username, currentUsername) == 0) {
			found = 1;
			foundIndex = i;
			break;
		}
	}
	
	if (found) {
		if (levelNum == 1) {
			leaderboard[foundIndex].level1Deaths = deaths;
		} else if (levelNum == 2) {
			leaderboard[foundIndex].level2Deaths = deaths;
		} else if (levelNum == 3) {
			leaderboard[foundIndex].level3Deaths = deaths;
		}
	} else {
		if (leaderboardCount < MAX_PLAYERS) {
			strcpy_s(leaderboard[leaderboardCount].username, sizeof(leaderboard[leaderboardCount].username), currentUsername);
			leaderboard[leaderboardCount].level1Deaths = 0;
			leaderboard[leaderboardCount].level2Deaths = 0;
			leaderboard[leaderboardCount].level3Deaths = 0;
			
			if (levelNum == 1) {
				leaderboard[leaderboardCount].level1Deaths = deaths;
			} else if (levelNum == 2) {
				leaderboard[leaderboardCount].level2Deaths = deaths;
			} else if (levelNum == 3) {
				leaderboard[leaderboardCount].level3Deaths = deaths;
			}
			leaderboardCount++;
		}
	}
	
	saveScoreboard();
}

void incrementDeathCount() {
	currentLevelDeaths++;
}

void resetDeathCount() {
	currentLevelDeaths = 0;
}

int getCurrentDeathCount() {
	return currentLevelDeaths;
}

void sortLeaderboard(PlayerScore* arr, int count, int levelNum) {
	for (int i = 0; i < count - 1; i++) {
		for (int j = 0; j < count - i - 1; j++) {
			int aDeaths, bDeaths;
			
			if (levelNum == 1) {
				aDeaths = arr[j].level1Deaths;
				bDeaths = arr[j + 1].level1Deaths;
			} else if (levelNum == 2) {
				aDeaths = arr[j].level2Deaths;
				bDeaths = arr[j + 1].level2Deaths;
			} else {
				aDeaths = arr[j].level3Deaths;
				bDeaths = arr[j + 1].level3Deaths;
			}
			
			if (aDeaths > bDeaths) {
				PlayerScore temp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = temp;
			}
		}
	}
}

void getLevel1Rankings(PlayerScore* result, int* count) {
	for (int i = 0; i < leaderboardCount; i++) {
		result[i] = leaderboard[i];
	}
	*count = leaderboardCount;
	sortLeaderboard(result, *count, 1);
}

void getLevel2Rankings(PlayerScore* result, int* count) {
	for (int i = 0; i < leaderboardCount; i++) {
		result[i] = leaderboard[i];
	}
	*count = leaderboardCount;
	sortLeaderboard(result, *count, 2);
}

void getLevel3Rankings(PlayerScore* result, int* count) {
	for (int i = 0; i < leaderboardCount; i++) {
		result[i] = leaderboard[i];
	}
	*count = leaderboardCount;
	sortLeaderboard(result, *count, 3);
}

#endif
