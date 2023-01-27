#pragma once
#include "Core/Input.h"
#include "Core/Game.h"
#include <zdk.h>

namespace ZuneCraft {
	Input::Input() {
		ZDKInput_Initialize();
		QueryPerformanceFrequency((LARGE_INTEGER*)&m_TimerFrequency);
	}

	Input::~Input() {
		ZDKInput_Shutdown();
	}

	double Input::GetTime() {
		uint64_t time = 0;
		QueryPerformanceCounter((LARGE_INTEGER*)&time);
		return time / m_TimerFrequency;
	}

	bool Input::GetActionDebugWireframe() {
		return false;
	}

	void Input::CheckToggleFocus() { }

	bool Input::GetActionDebugGizmos() {
		ZDK_INPUT_STATE input;
		ZDKInput_GetState(&input);

		static bool debug = false;

		if(input.TouchState.Count == 3) {
			debug = !debug;
		}

		return debug;
	}

	bool Input::GetActionForward() {
		ZDK_INPUT_STATE input;
		ZDKInput_GetState(&input);

		return input.TouchState.Count > 1;
	}

	bool Input::GetActionBackward() {
		return false;
	}

	bool Input::GetActionLeft() {
		return false;
	}

	bool Input::GetActionRight() {
		return false;
	}

	glm::vec2 Input::GetRotationAxies() {
		static float lastX = 0;
		static float lastY = 0;
		static bool isFirstTouch = true;

		ZDK_INPUT_STATE input;
		ZDKInput_GetState(&input);

		if(input.TouchState.Count < 1) {
			isFirstTouch = true;
			return glm::vec2(0,0);
		}
		
		float x = input.TouchState.Locations[0].X;
		float y = input.TouchState.Locations[0].Y;
		
		if (isFirstTouch) {
			lastX = x;
			lastY = y;
			isFirstTouch = false;
		}

		//Swapped because rendering in landscape mode
		glm::vec2 offsets((y - lastY) * 0.5f, (x - lastX) * 0.5f);

		lastX = x;
		lastY = y;

		return offsets;
	}
}