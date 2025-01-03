#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <memory>

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/uart.h"
#include "hardware/timer.h"
#include "hardware/pwm.h"

const uint LedYellow = 1;

const uint ServoPwmOut = 0;
const uint ServoPeriodUs = 20000;
const uint ServoDutyMinUs = 1300;
const uint ServoDutyMaxUs = 1700;
const uint ServoDutyMidUs = (ServoDutyMaxUs + ServoDutyMinUs) / 2;
const uint ServoClkDivider = SYS_CLK_KHZ / 1000;

static uint ServoPwmSlice = 0;

void setServoDuty(uint dutyUs) {
	pwm_set_chan_level(ServoPwmSlice, 0, dutyUs);
}

void init() {
	stdio_init_all();

	gpio_init(LedYellow);
	gpio_set_dir(LedYellow, GPIO_OUT);

	gpio_set_function(ServoPwmOut, GPIO_FUNC_PWM);
	ServoPwmSlice = pwm_gpio_to_slice_num(ServoPwmOut);
	pwm_set_clkdiv_int_frac(ServoPwmSlice, ServoClkDivider, 0);
	pwm_set_wrap(ServoPwmSlice, ServoPeriodUs);
	pwm_set_chan_level(ServoPwmSlice, 0, ServoDutyMidUs);
	pwm_set_enabled(ServoPwmSlice, true);
}

void update() {
	gpio_put(LedYellow, 1);
	setServoDuty(ServoDutyMinUs);
	sleep_ms(1000);

	gpio_put(LedYellow, 0);
	setServoDuty(ServoDutyMaxUs);
	sleep_ms(1000);
}

int main() {
	init();

	while(true) {
		update();
	}
}
