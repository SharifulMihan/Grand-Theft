#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <windows.h>
#include <xinput.h>
#include <cmath>

// Dynamic XInput function prototypes
typedef DWORD(WINAPI* PFN_XInputGetState)(DWORD dwUserIndex, XINPUT_STATE* pState);
typedef DWORD(WINAPI* PFN_XInputSetState)(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration);

class ControllerManager {
private:
	HMODULE hXInput;
	PFN_XInputGetState pfnGetState;
	PFN_XInputSetState pfnSetState;

	bool isConnected;
	int activeIndex;
	XINPUT_STATE currentState;
	XINPUT_STATE previousState;

	// Deadzone threshold for analog sticks
	const SHORT DEADZONE = 12000;

	// Menu navigation repeat and debounce timers
	float navCooldown;
	const float NAV_REPEAT_DELAY = 0.22f;

	// Vibration / Haptic state
	float hapticTimer;
	WORD hapticLeftMotor;
	WORD hapticRightMotor;

	// Whistle pulsating vibration state (1s on, 2s off cycle)
	bool whistlePulseActive;
	float whistleCycleTimer;

public:
	ControllerManager() {
		hXInput = NULL;
		pfnGetState = NULL;
		pfnSetState = NULL;
		isConnected = false;
		activeIndex = -1;
		memset(&currentState, 0, sizeof(XINPUT_STATE));
		memset(&previousState, 0, sizeof(XINPUT_STATE));
		navCooldown = 0.0f;
		hapticTimer = 0.0f;
		hapticLeftMotor = 0;
		hapticRightMotor = 0;
		whistlePulseActive = false;
		whistleCycleTimer = 0.0f;

		initXInput();
	}

	~ControllerManager() {
		stopVibration();
		if (hXInput) {
			FreeLibrary(hXInput);
			hXInput = NULL;
		}
	}

	void initXInput() {
		// Try loading XInput 1.4 (Windows 8+), then 1.3 (DirectX SDK), then 9.1.0 (Windows Vista/7)
		const char* dllNames[] = { "xinput1_4.dll", "xinput1_3.dll", "xinput9_1_0.dll" };
		for (int i = 0; i < 3; i++) {
			hXInput = LoadLibraryA(dllNames[i]);
			if (hXInput) {
				pfnGetState = (PFN_XInputGetState)GetProcAddress(hXInput, "XInputGetState");
				pfnSetState = (PFN_XInputSetState)GetProcAddress(hXInput, "XInputSetState");
				if (pfnGetState && pfnSetState) {
					break;
				}
				FreeLibrary(hXInput);
				hXInput = NULL;
			}
		}
	}

	void update(float dt) {
		previousState = currentState;
		isConnected = false;
		activeIndex = -1;

		if (navCooldown > 0.0f) {
			navCooldown -= dt;
			if (navCooldown < 0.0f) navCooldown = 0.0f;
		}

		if (!pfnGetState) return;

		// Query controllers 0 to 3 to find first active connected controller
		for (DWORD i = 0; i < 4; i++) {
			XINPUT_STATE state;
			DWORD result = pfnGetState(i, &state);
			if (result == ERROR_SUCCESS) {
				isConnected = true;
				activeIndex = (int)i;
				currentState = state;
				break;
			}
		}

		if (!isConnected) {
			memset(&currentState, 0, sizeof(XINPUT_STATE));
		}

		updateHaptics(dt);
	}

	// -------------------------------------------------------------
	// Button State Queries
	// -------------------------------------------------------------
	bool isButtonDown(WORD button) const {
		if (!isConnected) return false;
		return (currentState.Gamepad.wButtons & button) != 0;
	}

	bool isButtonPressed(WORD button) const {
		if (!isConnected) return false;
		bool now = (currentState.Gamepad.wButtons & button) != 0;
		bool prev = (previousState.Gamepad.wButtons & button) != 0;
		return now && !prev;
	}

	// Jump: strictly Xbox A / PS4 X (XINPUT_GAMEPAD_A)
	bool isJumpPressed() const {
		return isButtonPressed(XINPUT_GAMEPAD_A);
	}

	bool isJumpDown() const {
		return isButtonDown(XINPUT_GAMEPAD_A);
	}

	// Confirm / Select in menus: Xbox A / PS4 X
	bool isConfirmPressed() const {
		return isButtonPressed(XINPUT_GAMEPAD_A);
	}

	// Back / Cancel in menus: Xbox B / PS4 O (Circle)
	bool isBackPressed() const {
		return isButtonPressed(XINPUT_GAMEPAD_B);
	}

	// Pause / Options: Xbox Start / PS4 Options
	bool isOptionsPressed() const {
		return isButtonPressed(XINPUT_GAMEPAD_START);
	}

	// -------------------------------------------------------------
	// In-Game Directional Controls (D-Pad or Left Stick)
	// -------------------------------------------------------------
	bool isLeft() const {
		if (!isConnected) return false;
		if (currentState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) return true;
		if (currentState.Gamepad.sThumbLX < -DEADZONE) return true;
		return false;
	}

	bool isRight() const {
		if (!isConnected) return false;
		if (currentState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) return true;
		if (currentState.Gamepad.sThumbLX > DEADZONE) return true;
		return false;
	}

	bool isUp() const {
		if (!isConnected) return false;
		if (currentState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) return true;
		if (currentState.Gamepad.sThumbLY > DEADZONE) return true;
		return false;
	}

	bool isDown() const {
		if (!isConnected) return false;
		if (currentState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) return true;
		if (currentState.Gamepad.sThumbLY < -DEADZONE) return true;
		return false;
	}

	// -------------------------------------------------------------
	// Clean Menu Navigation (with anti-skipping debounce)
	// -------------------------------------------------------------
	bool isNavUp() {
		if (!isConnected) return false;
		bool dpadNow = (currentState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0;
		bool dpadPrev = (previousState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0;
		bool stickNow = currentState.Gamepad.sThumbLY > DEADZONE;
		bool stickPrev = previousState.Gamepad.sThumbLY > DEADZONE;

		bool freshPress = (dpadNow && !dpadPrev) || (stickNow && !stickPrev);
		bool heldRepeat = (dpadNow || stickNow) && (navCooldown <= 0.0f);

		if (freshPress || heldRepeat) {
			navCooldown = NAV_REPEAT_DELAY;
			return true;
		}
		return false;
	}

	bool isNavDown() {
		if (!isConnected) return false;
		bool dpadNow = (currentState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0;
		bool dpadPrev = (previousState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0;
		bool stickNow = currentState.Gamepad.sThumbLY < -DEADZONE;
		bool stickPrev = previousState.Gamepad.sThumbLY < -DEADZONE;

		bool freshPress = (dpadNow && !dpadPrev) || (stickNow && !stickPrev);
		bool heldRepeat = (dpadNow || stickNow) && (navCooldown <= 0.0f);

		if (freshPress || heldRepeat) {
			navCooldown = NAV_REPEAT_DELAY;
			return true;
		}
		return false;
	}

	bool isNavLeft() {
		if (!isConnected) return false;
		bool dpadNow = (currentState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0;
		bool dpadPrev = (previousState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0;
		bool stickNow = currentState.Gamepad.sThumbLX < -DEADZONE;
		bool stickPrev = previousState.Gamepad.sThumbLX < -DEADZONE;

		bool freshPress = (dpadNow && !dpadPrev) || (stickNow && !stickPrev);
		bool heldRepeat = (dpadNow || stickNow) && (navCooldown <= 0.0f);

		if (freshPress || heldRepeat) {
			navCooldown = NAV_REPEAT_DELAY;
			return true;
		}
		return false;
	}

	bool isNavRight() {
		if (!isConnected) return false;
		bool dpadNow = (currentState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0;
		bool dpadPrev = (previousState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0;
		bool stickNow = currentState.Gamepad.sThumbLX > DEADZONE;
		bool stickPrev = previousState.Gamepad.sThumbLX > DEADZONE;

		bool freshPress = (dpadNow && !dpadPrev) || (stickNow && !stickPrev);
		bool heldRepeat = (dpadNow || stickNow) && (navCooldown <= 0.0f);

		if (freshPress || heldRepeat) {
			navCooldown = NAV_REPEAT_DELAY;
			return true;
		}
		return false;
	}

	// -------------------------------------------------------------
	// Haptic Vibration System
	// -------------------------------------------------------------
	void triggerHaptic(float duration, WORD leftMotor, WORD rightMotor) {
		hapticTimer = duration;
		hapticLeftMotor = leftMotor;
		hapticRightMotor = rightMotor;
		sendVibration(leftMotor, rightMotor);
	}

	// Cash collected: vibrate for 0.5 second
	void triggerCashCollected() {
		triggerHaptic(0.5f, 32000, 32000);
	}

	// Key collected: vibrate for 0.5 second
	void triggerKeyCollected() {
		triggerHaptic(0.5f, 28000, 28000);
	}

	// Player dies: vibrate for 1.0 second
	void triggerPlayerDeath() {
		whistlePulseActive = false; // Stop alarm pulse on death
		triggerHaptic(1.0f, 65535, 65535);
	}

	// Start / update whistle pulsating vibration (1s vibrate, 2s off, repeat)
	void setWhistlePulse(bool active) {
		if (active) {
			if (!whistlePulseActive) {
				whistlePulseActive = true;
				whistleCycleTimer = 0.0f;
			}
		}
		else {
			if (whistlePulseActive) {
				whistlePulseActive = false;
				whistleCycleTimer = 0.0f;
				if (hapticTimer <= 0.0f) {
					stopVibration();
				}
			}
		}
	}

	void stopVibration() {
		hapticTimer = 0.0f;
		hapticLeftMotor = 0;
		hapticRightMotor = 0;
		whistlePulseActive = false;
		sendVibration(0, 0);
	}

private:
	void sendVibration(WORD left, WORD right) {
		if (!isConnected || !pfnSetState || activeIndex < 0) return;
		XINPUT_VIBRATION vib;
		vib.wLeftMotorSpeed = left;
		vib.wRightMotorSpeed = right;
		pfnSetState((DWORD)activeIndex, &vib);
	}

	void updateHaptics(float dt) {
		// Priority 1: One-shot event vibrations (Cash or Death)
		if (hapticTimer > 0.0f) {
			hapticTimer -= dt;
			if (hapticTimer <= 0.0f) {
				hapticTimer = 0.0f;
				hapticLeftMotor = 0;
				hapticRightMotor = 0;
				if (!whistlePulseActive) {
					sendVibration(0, 0);
				}
			}
			else {
				sendVibration(hapticLeftMotor, hapticRightMotor);
				return;
			}
		}

		// Priority 2: Whistle pulsation (1.0s vibrate, 6.0s off = 7.0s total period)
		if (whistlePulseActive) {
			whistleCycleTimer += dt;
			if (whistleCycleTimer >= 7.0f) {
				whistleCycleTimer -= 7.0f;
			}

			if (whistleCycleTimer < 1.0f) {
				// Vibrate phase (1 second)
				sendVibration(42000, 42000);
			}
			else {
				// Off phase (6 seconds)
				sendVibration(0, 0);
			}
		}
	}
};

extern ControllerManager GameController;

#endif // CONTROLLER_HPP
