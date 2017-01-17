/*
 * Copyright (c) 2016 Jean-Paul Etienne <fractalclone@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <zephyr.h>
#include <device.h>
#include <gpio.h>
#include <board.h>
#include <string.h>

#include <misc/util.h>
#include <misc/printk.h>

/* LED blinking speed configuration */
#define SPEED_STEP        10
#define SPEED_MAX_STEPS   (15 * SPEED_STEP)

#define STACK_SIZE        512

struct disco_thread_led_t {
	const char *port;
	struct device *gpio;
	uint32_t pin;
	uint32_t sleep_ms;

	char __stack stack[STACK_SIZE];
};

struct disco_sw_t {
	const char *port;
	struct gpio_callback cb;
	uint32_t pin;
	uint32_t pull_up;
	uint32_t irq_trig_conf;
};

/*
 * LED configurations
 * Each board is expected to provide at least one LEDx_GPIO_PORT and
 * one LEDx_GPIO_PIN
 * The application handles a maximum of 8 LEDS, ranging from 0 to 7.
 */
struct disco_thread_led_t leds[] = {
#ifdef LED0_GPIO_PIN
	{
		.port     = LED0_GPIO_PORT,
		.pin      = LED0_GPIO_PIN,
		.sleep_ms = 200,
	},
#endif
#ifdef LED1_GPIO_PIN
	{
		.port     = LED1_GPIO_PORT,
		.pin      = LED1_GPIO_PIN,
		.sleep_ms = 500,
	},
#endif
#ifdef LED2_GPIO_PIN
	{
		.port     = LED2_GPIO_PORT,
		.pin      = LED2_GPIO_PIN,
		.sleep_ms = 250,
	},
#endif
#ifdef LED3_GPIO_PIN
	{
		.port     = LED3_GPIO_PORT,
		.pin      = LED3_GPIO_PIN,
		.sleep_ms = 450,
	},
#endif
#ifdef LED4_GPIO_PIN
	{
		.port     = LED4_GPIO_PORT,
		.pin      = LED4_GPIO_PIN,
		.sleep_ms = 240,
	},
#endif
#ifdef LED5_GPIO_PIN
	{
		.port     = LED5_GPIO_PORT,
		.pin      = LED5_GPIO_PIN,
		.sleep_ms = 400,
	},
#endif
#ifdef LED6_GPIO_PIN
	{
		.port     = LED6_GPIO_PORT,
		.pin      = LED6_GPIO_PIN,
		.sleep_ms = 230,
	},
#endif
#ifdef LED7_GPIO_PIN
	{
		.port     = LED7_GPIO_PORT,
		.pin      = LED7_GPIO_PIN,
		.sleep_ms = 550,
	},
#endif
};

/*
 * Speed switch configuration
 * SW0 is used to increase LED blinking speed.
 * SW1 is used to decrease the LED blinkig speed.
 * Speed is increased/decreased by SPEED_STEP each time SW0/SW1 is pressed.
 * Speed can be increased or decreased for a maximum of SPEED_MAX_STEPS.
 */
#ifndef SW0_GPIO_NAME
#error "Board is expected to provide SW0_GPIO_NAME"
#endif

#ifndef SW0_GPIO_PIN
#error "Board is expected to provide SW0_GPIO_PIN"
#endif

#ifndef SW0_GPIO_PULL_UP
#error "Board is expected to provide SW0_GPIO_PULL_UP"
#endif

#ifndef SW0_GPIO_INT_CONF
#error "Board is expected to provide SW0_GPIO_INT_CONF"
#endif

#ifndef SW1_GPIO_NAME
#error "Board is expected to provide SW0_GPIO_NAME"
#endif

#ifndef SW1_GPIO_PIN
#error "Board is expected to provide SW0_GPIO_PIN"
#endif

#ifndef SW1_GPIO_PULL_UP
#error "Board is expected to provide SW1_GPIO_PULL_UP"
#endif

#ifndef SW1_GPIO_INT_CONF
#error "Board is expected to provide SW1_GPIO_INT_CONF"
#endif

static struct disco_sw_t sws[] = {
	{
		.port          = SW0_GPIO_NAME,
		.pin           = SW0_GPIO_PIN,
		.pull_up       = SW0_GPIO_PULL_UP,
		.irq_trig_conf = SW0_GPIO_INT_CONF,
	},
	{
		.port          = SW1_GPIO_NAME,
		.pin           = SW1_GPIO_PIN,
		.pull_up       = SW1_GPIO_PULL_UP,
		.irq_trig_conf = SW1_GPIO_INT_CONF,
	},
};

/*
 * Mutex used by LED blinking threads to ensure
 * exclusive access to the gpio controller
 */
static struct k_mutex gpio_mutex;

static volatile int speed_offset;

static void blinking_thread(void *data, void *unused1, void *unused2)
{
	struct disco_thread_led_t *led = (struct disco_thread_led_t *)data;
	int toggle;

	ARG_UNUSED(unused1);
	ARG_UNUSED(unused2);

	toggle = 0;
	while (1) {
		toggle = !toggle;

		/* Toggle Led with mutex held */
		(void)k_mutex_lock(&gpio_mutex, K_FOREVER);
		gpio_pin_write(led->gpio, led->pin, toggle);
		k_mutex_unlock(&gpio_mutex);

		/* Sleep for sleep_ms + speed_offset */
		k_sleep(led->sleep_ms + speed_offset);
	}
}

static void start_blinking_threads(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(leds); i++) {
		/* Setup gpio device for each thread */
		leds[i].gpio = device_get_binding(leds[i].port);
		if (leds[i].gpio == NULL) {
			printk("error: cannot get binding to device %s\n",
			       leds[i].port);
			return;
		}

		/* Configure gpio pin for each thread */
		gpio_pin_configure(leds[i].gpio, leds[i].pin, GPIO_DIR_OUT);
	}

	for (i = 0; i < ARRAY_SIZE(leds); i++) {
		/* Start each thread with same priority level 0 */
		k_thread_spawn(leds[i].stack, STACK_SIZE, blinking_thread,
			       (void *)&leds[i], NULL, NULL,
			       0, 0, 0);
	}
}

void sw_pressed(struct device *gpio,
		struct gpio_callback *cb,
		uint32_t pins)
{
	if (!strcmp(sws[0].port, gpio->config->name) &&
	    (pins & BIT(sws[0].pin)) &&
	    speed_offset > -SPEED_MAX_STEPS)
		speed_offset -= SPEED_STEP;
	else if (!strcmp(sws[1].port, gpio->config->name) &&
		 (pins & BIT(sws[1].pin)) &&
		 speed_offset < SPEED_MAX_STEPS)
		speed_offset += SPEED_STEP;
	else
		return;

	printk("Blinking speed adjusted by %dms\n", speed_offset);
}

static void configure_sw_buttons(void)
{
	struct device *dev;
	int i;

	for (i = 0; i < ARRAY_SIZE(sws); i++) {
		dev = device_get_binding(sws[i].port);
		if (dev == NULL) {
			printk("error: cannot get binding to device %s\n",
			       sws[i].port);
			return;
		}

		/* Configure gpio pin for each switch */
		gpio_pin_configure(dev, sws[i].pin,
				   GPIO_DIR_IN | GPIO_INT |
				   sws[i].pull_up | sws[i].irq_trig_conf);

		/* Setup callback for each switch */
		gpio_init_callback(&sws[i].cb, sw_pressed, BIT(sws[i].pin));
		gpio_add_callback(dev, &sws[i].cb);
		gpio_pin_enable_callback(dev, sws[i].pin);
	}
}

void main(void)
{
	/* Initialize mutex */
	k_mutex_init(&gpio_mutex);

	/* Configure switch buttons */
	configure_sw_buttons();

	/* Start blinking threads */
	start_blinking_threads();

	printk("Press buttons at pins %d and %d to respectively\n"
	       "increase or decrease LED blinking speed\n",
	       sws[0].pin,
	       sws[1].pin);
}
