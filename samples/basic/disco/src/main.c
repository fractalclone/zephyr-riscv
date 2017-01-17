/*
 * Copyright (c) 2016 Open-RnD Sp. z o.o.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <device.h>
#include <gpio.h>
#include <board.h>

/**
  * If LED1_GPIO_PORT, LED1_GPIO_PIN and LED2_GPIO_PIN are not provided
  * within board.h, the demo assumes use of nucleo_f103rb board by default.
  */

/* Change this if you have a LED connected to a custom port */
#ifdef LED1_GPIO_PORT
#define PORT    LED1_GPIO_PORT
#else
#define PORT    "GPIOB"
#endif

/* Change this if you have an LED connected to a custom pin */
#ifdef LED1_GPIO_PIN
#define LED1    LED1_GPIO_PIN
#else
#define LED1    5
#endif

#ifdef LED2_GPIO_PIN
#define LED2    LED2_GPIO_PIN
#else
#define LED2    8
#endif

#define SLEEP_TIME	500

void main(void)
{
	int cnt = 0;
	struct device *gpiob;

	gpiob = device_get_binding(PORT);

	gpio_pin_configure(gpiob, LED1, GPIO_DIR_OUT);
	gpio_pin_configure(gpiob, LED2, GPIO_DIR_OUT);

	while (1) {
		gpio_pin_write(gpiob, LED1, cnt % 2);
		gpio_pin_write(gpiob, LED2, (cnt + 1) % 2);
		k_sleep(SLEEP_TIME);
		cnt++;
	}
}
