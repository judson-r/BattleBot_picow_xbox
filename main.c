// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2023 Brian Starkey <stark3y@gmail.com>
// Gustavo Diaz <intgusd@gmail.com>

#include <stdio.h>
#include <string.h>

#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "bt_hid.h"

// This is an example of how to parse button information

#define BUTTON_A 1
#define BUTTON_B 2
#define BUTTON_X 4
#define BUTTON_Y 8

#define BUTTON_LEFT_BUMPER 11
#define BUTTON_RIGHT_BUMPER 12

#define DRIVE_LEFT_MOTOR
#define DRIVE_RIGHT_MOTOR
#define WEAPON_SPIN_MOTOR

void main(void) {
	stdio_init_all();

	gpio_init(DRIVE_LEFT_MOTOR);
	gpio_set_dir(DRIVE_LEFT_MOTOR, OUT);
	gpio_set_function(DRIVE_LEFT_MOTOR, GPIO_FUNC_PWM);

	gpio_init(DRIVE_RIGHT_MOTOR);
	gpio_set_dir(DRIVE_RIGHT_MOTOR, OUT);
	gpio_set_function(DRIVE_RIGHT_MOTOR, GPIO_FUNC_PWM);

	gpio_init(WEAPON_SPIN_MOTOR);
	gpio_set_dir(WEAPON_SPIN_MOTOR, OUT);
	gpio_set_function(WEAPON_SPIN_MOTOR, GPIO_FUNC_PWM);

	uint slice_left = pwm_gpio_to_slice_num(DRIVE_LEFT_MOTOR);
	uint slice_right = pwm_gpio_to_slice_num(DRIVE_RIGHT_MOTOR);
	uint slice_weapon = pwm_gpio_to_slice_num(WEAPON_SPIN_MOTOR);

	pwm_set_wrap(slice_left, 1000);
	pwm_set_wrap(slice_right, 1000);
	pwm_set_wrap(slice_weapon, 1000);

	sleep_ms(1000);

	multicore_launch_core1(bt_main);
	// Wait for init (should do a handshake with the fifo here?)
	sleep_ms(1000);

	struct bt_hid_state state;
	for ( ;; ) {
		sleep_ms(20);
		bt_hid_get_latest(&state);
		// Use the state of the xbox controller here
	
		if (state.ly > 10){
			pwm_set_chan_level(slice_left, PWM_CHAN_A, 1000 * (state.ly/127));
			pwm_set_chan_level(slice_left, PWM_CHAN_B, 0);
		}
		else{
			pwm_set_chan_level(slice_left, PWM_CHAN_A, 0);
			pwm_set_chan_level(slice_left, PWM_CHAN_B, 0);
		}

		if (state.ry > 10){
			pwm_set_chan_level(slice_right, PWM_CHAN_A, 1000 * (state.ry/127));
			pwm_set_chan_level(slice_right, PWM_CHAN_B, 0);
		}
		else{
			pwm_set_chan_level(slice_right, PWM_CHAN_A, 0);
			pwm_set_chan_level(slice_right, PWM_CHAN_B, 0);
		}

		if ((state.buttons & (BUTTON_RIGHT_BUMPER | BUTTON_LEFT_BUMPER)) == (BUTTON_RIGHT_BUMPER | BUTTON_LEFT_BUMPER)) {
			// Combination of buttons RIGHT_BUMPER and LEFT_BUMPER is pressed
			pwm_set_chan_level(slice_weapon, PWM_CHAN_A, 1000);
			pwm_set_chan_level(slice_weapon, PWM_CHAN_B, 0);
		}
		else{
			pwm_set_chan_level(slice_weapon, PWM_CHAN_A, 0);
			pwm_set_chan_level(slice_weapon, PWM_CHAN_B, 0);
		}

		if (state.buttons & BUTTON_A) {
			// Button A is pressed
			
		}

		if (state.buttons & BUTTON_B) {
			// Button B is pressed
			
		}

		if (state.buttons & BUTTON_X) {
			// Button X is pressed
			
		}

		if (state.buttons & BUTTON_Y) {
			// Button Y is pressed
			
		}

		// Check combinations of buttons
		if ((state.buttons & (BUTTON_A | BUTTON_X)) == (BUTTON_A | BUTTON_X)) {
			// Combination of buttons A and X is pressed
			
		}
	
		// Print for debugging purposes to USB serial monitor
		// printf("buttons: %d, l: %d,%d, r: %d,%d, l2,r2: %d,%d hat: %d\n",
		// 		state.buttons, state.lx, state.ly, state.rx, state.ry,
		// 		state.l2, state.r2, state.hat);

	}
}
