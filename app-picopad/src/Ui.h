
#pragma once

#include <stdint.h>

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

struct Color {
	uint8_t red;
	uint8_t green;
	uint8_t blue;

	static Color White() { return { 255, 255, 255 }; }
	static Color Gray() { return { 128, 128, 128 }; }
	static Color Red() { return { 255, 0, 0 }; }
	static Color Green() { return { 0, 255, 0 }; }
	static Color Blue() { return { 0, 0, 255 }; }
};

bool IsButtonPressed(Button button);

void UpdateInputs();

void BeginWidget();
void EndWidget();

void SetColor(Color color);

void Text(const char* format, ...);
void TextUnformatted(const char* text);
void Separator();

void Init();
void Deinit();

}