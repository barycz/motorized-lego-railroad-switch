
#include "Ui.h"

#include "hagl_hal.h"
#include "hagl.h"
#include "font6x9.h"
#include "fontx.h"

#include "hardware/gpio.h"

#include <stdarg.h>
#include <stdio.h>
#include <array>

namespace Ui {

enum class ButtonState {
	Up = 0,
	Down,
	Pressed, // was up, now is down
	Released, // was down, now is up
};

struct Context {
	hagl_backend_t* Display;
	size_t WidgetStackSize;
	uint16_t X;
	uint16_t Y;

	std::array<ButtonState, static_cast<size_t>(Button::Count)> ButtonStates;
	std::array<uint, static_cast<size_t>(Button::Count)> ButtonMapping;
};

static Context g_Context;

#define UI_CHECK_CONTEXT() \
	if (g_Context.Display == nullptr) { return; }

void Text(const char* format, ...) {
	UI_CHECK_CONTEXT();

	char buffer[32];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	fontx_meta_t meta;
	fontx_meta(&meta, font6x9);

	uint16_t x = g_Context.X;
	uint16_t y = g_Context.Y;

	char* it = buffer;
	do {
		wchar_t temp = *it++;
		if (13 == temp || 10 == temp) {
			x = g_Context.X;
			y += meta.height;
		} else {
			x += hagl_put_char(g_Context.Display, temp, x, y, hagl_color(g_Context.Display, 255, 255, 255), font6x9);
		}
	} while (*it != 0);

	g_Context.Y = y + meta.height;
}

void Separator() {
	UI_CHECK_CONTEXT();
	hagl_draw_hline(g_Context.Display, g_Context.X, g_Context.Y, g_Context.Display->width - 1, hagl_color(g_Context.Display, 255, 255, 255));
	g_Context.Y += 1;
}

void UpdateInputs() {
	UI_CHECK_CONTEXT();
	for (size_t i = 0; i < static_cast<size_t>(Button::Count); ++i) {
		if (g_Context.ButtonMapping[i] == -1) {
			continue;
		}
		if (gpio_get(g_Context.ButtonMapping[i]) == BUTTON_PRESSED_LEVEL) {
			if (g_Context.ButtonStates[i] == ButtonState::Up || g_Context.ButtonStates[i] == ButtonState::Released) {
				g_Context.ButtonStates[i] = ButtonState::Pressed;
			} else {
				g_Context.ButtonStates[i] = ButtonState::Down;
			}
		} else {
			if (g_Context.ButtonStates[i] == ButtonState::Down || g_Context.ButtonStates[i] == ButtonState::Pressed) {
				g_Context.ButtonStates[i] = ButtonState::Released;
			} else {
				g_Context.ButtonStates[i] = ButtonState::Up;
			}
		}
	}
}

bool IsButtonPressed(Button button) {
	return g_Context.ButtonStates[static_cast<size_t>(button)] == ButtonState::Pressed;
}

void Init() {
	g_Context.ButtonMapping.fill(-1);
	g_Context.ButtonStates.fill(ButtonState::Up);
	auto setupInput = [](uint gpio, Button button) {
		gpio_init(gpio);
		gpio_set_dir(gpio, GPIO_IN);
#if BUTTON_PRESSED_LEVEL == 0
		gpio_pull_up(gpio);
#else
		gpio_pull_down(gpio);
#endif
		g_Context.ButtonMapping[static_cast<size_t>(button)] = gpio;
	};

#ifdef BUTTON_A_PIN
	setupInput(BUTTON_A_PIN, Button::A);
#endif
#ifdef BUTTON_B_PIN
	setupInput(BUTTON_B_PIN, Button::B);
#endif
#ifdef BUTTON_X_PIN
	setupInput(BUTTON_X_PIN, Button::X);
#endif
#ifdef BUTTON_Y_PIN
	setupInput(BUTTON_Y_PIN, Button::Y);
#endif
#ifdef BUTTON_UP_PIN
	setupInput(BUTTON_UP_PIN, Button::Up);
#endif
#ifdef BUTTON_DOWN_PIN
	setupInput(BUTTON_DOWN_PIN, Button::Down);
#endif
#ifdef BUTTON_LEFT_PIN
	setupInput(BUTTON_LEFT_PIN, Button::Left);
#endif
#ifdef BUTTON_RIGHT_PIN
	setupInput(BUTTON_RIGHT_PIN, Button::Right);
#endif

	g_Context.Display = hagl_init();
	g_Context.WidgetStackSize = 0;
	g_Context.X = 0;
	g_Context.Y = 0;
	hagl_clear(g_Context.Display);
}

void BeginWidget() {
	UI_CHECK_CONTEXT();
	if (g_Context.WidgetStackSize == 0) {
		hagl_clear(g_Context.Display);
		g_Context.X = 0;
		g_Context.Y = 0;
	}
	g_Context.WidgetStackSize++;
}

void EndWidget() {
	UI_CHECK_CONTEXT();
	if (g_Context.WidgetStackSize == 0) {
		printf("Ui::EndWidget() stack underflow");
		return;
	}
	g_Context.WidgetStackSize--;

	if (g_Context.WidgetStackSize == 0) {
		hagl_flush(g_Context.Display);
	}
}

void Deinit() {
	UI_CHECK_CONTEXT();
	hagl_close(g_Context.Display);
	g_Context.Display = nullptr;
}

}
