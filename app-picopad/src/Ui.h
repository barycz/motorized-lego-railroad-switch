
#pragma once

namespace Ui {

enum class Button {
	A,
	B,
	X,
	Y,
	Up,
	Down,
	Left,
	Right,
	Count
};

bool IsButtonPressed(Button button);

void UpdateInputs();

void BeginWidget();
void EndWidget();

void Text(const char* format, ...);
void Separator();

void Init();
void Deinit();

}