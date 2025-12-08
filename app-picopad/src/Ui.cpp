
#include "Ui.h"

#include "hagl_hal.h"
#include "hagl.h"
#include "font6x9.h"
#include "X11/include/font8x13.h"
#include "fontx.h"

#include "hardware/gpio.h"

#include <stdarg.h>
#include <stdio.h>
#include <array>

namespace Ui {

static_assert(HAGL_CHAR_BUFFER_SIZE >= 8 * 13 * 2);
static const uint8_t* g_Font = font8x13;

enum class ButtonState {
	Up = 0,
	Down,
	Pressed, // was up, now is down
	Released, // was down, now is up
};

struct WidgetState {
	Color CurrentColor;
};

struct Context {
	hagl_backend_t* Display;
	size_t WidgetStackSize;
	Color DefaultColor;
	uint16_t X;
	uint16_t Y;

	std::array<ButtonState, static_cast<size_t>(Button::Count)> ButtonStates;
	std::array<uint, static_cast<size_t>(Button::Count)> ButtonMapping;
	std::array<WidgetState, 10> WidgetStack;
};

static Context g_Context;

#define UI_CHECK_VALID_DISPLAY() \
	assert(g_Context.Display != nullptr);

#define UI_CHECK_NONEMPTY_STACK() \
	assert(g_Context.WidgetStackSize > 0);

hagl_color_t GetCurrentColor() {
	UI_CHECK_VALID_DISPLAY();
	UI_CHECK_NONEMPTY_STACK();
	return hagl_color(g_Context.Display,
		g_Context.WidgetStack[g_Context.WidgetStackSize - 1].CurrentColor.red,
		g_Context.WidgetStack[g_Context.WidgetStackSize - 1].CurrentColor.green,
		g_Context.WidgetStack[g_Context.WidgetStackSize - 1].CurrentColor.blue);
}

void Text(const char* format, ...) {
	char buffer[32];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	TextUnformatted(buffer);
}

void TextUnformatted(const char* text) {
	UI_CHECK_VALID_DISPLAY();
	UI_CHECK_NONEMPTY_STACK();

	fontx_meta_t meta;
	fontx_meta(&meta, g_Font);

	const hagl_color_t color = GetCurrentColor();

	uint16_t x = g_Context.X;
	uint16_t y = g_Context.Y;

	const char* it = text;
	do {
		wchar_t temp = *it++;
		if (13 == temp || 10 == temp) {
			x = g_Context.X;
			y += meta.height;
		} else {
			x += hagl_put_char(g_Context.Display, temp, x, y, color, g_Font);
		}
	} while (*it != 0);

	g_Context.Y = y + meta.height;
}

void Separator() {
	UI_CHECK_VALID_DISPLAY();
	UI_CHECK_NONEMPTY_STACK();
	hagl_draw_hline(g_Context.Display, g_Context.X, g_Context.Y, g_Context.Display->width - 1, GetCurrentColor());
	g_Context.Y += 1;
}

void UpdateInputs() {
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

	g_Context.DefaultColor = Color::White();
	g_Context.WidgetStack.fill(WidgetState{g_Context.DefaultColor});
	g_Context.WidgetStackSize = 0;

	g_Context.Display = hagl_init();
	g_Context.X = 0;
	g_Context.Y = 0;
	hagl_clear(g_Context.Display);
}

void BeginWidget() {
	UI_CHECK_VALID_DISPLAY();
	if (g_Context.WidgetStackSize == 0) {
		hagl_clear(g_Context.Display);
		g_Context.X = 0;
		g_Context.Y = 0;
	}
	g_Context.WidgetStack[g_Context.WidgetStackSize].CurrentColor = g_Context.DefaultColor;
	assert(g_Context.WidgetStackSize < Context::WidgetStack::size());
	g_Context.WidgetStackSize++;
}

void EndWidget() {
	UI_CHECK_VALID_DISPLAY();
	UI_CHECK_NONEMPTY_STACK();
	g_Context.WidgetStackSize--;

	if (g_Context.WidgetStackSize == 0) {
		hagl_flush(g_Context.Display);
	}
}

void SetColor(Color color) {
	UI_CHECK_NONEMPTY_STACK();
	g_Context.WidgetStack[g_Context.WidgetStackSize - 1].CurrentColor = color;
}

void Deinit() {
	UI_CHECK_VALID_DISPLAY();
	hagl_close(g_Context.Display);
	g_Context.Display = nullptr;
}

}
