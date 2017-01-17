Title: Disco fever Demo :-)

Description:

A multi-threaded disco demo application, in which each thread toggles a LED
for a given period in milliseconds. The blinking speed can be increased
or decreased via two push buttons.

The application can handle up the 8 LEDS, ranging from 0 to 7 and expects a
board to provide at least one LEDx_GPIO_PORT and one LEDx_GPIO_PIN.

A board is required to provide the following variable definitions for the two
push buttons:
SW0_GPIO_NAME: GPIO controller used by SW0
SW0_GPIO_PIN: GPIO pin of SW0
SW0_GPIO_PULL_UP: GPIO Pull-up config for SW0
SW0_GPIO_INT_CONF: GPIO Interrupt config for SW0

SW1_GPIO_NAME: GPIO controller used by SW1
SW1_GPIO_PIN: GPIO pin of SW1
SW1_GPIO_PULL_UP: GPIO Pull-up config for SW1
SW1_GPIO_INT_CONF: GPIO Interrupt config for SW1

After startup, the program shall:

1) configure the two push buttons, as well as their corresponding gpio
   callbacks. The callbacks shall be called upon each GPIO interrupt to
   increase/decrease the sleep time of the threads by a 'speed_offset'.

2) configure the LEDS defined by the board and launch their associated threads.
   The threads shall toggle their LED every (thread_sleep_ms + speed_offset).
   When toggling the GPIO pin, the GPIO driver is accessed in mutual exclusion
   (via the gpio_mutex) to avoid race-conditions over the GPIO registers.
--------------------------------------------------------------------------------

Building and Running Project:

It can be built for any board as follows:

    make BOARD=board_name

--------------------------------------------------------------------------------

Troubleshooting:

Problems caused by out-dated project information can be addressed by
issuing one of the following commands then rebuilding the project:

    make clean          # discard results of previous builds
			# but keep existing configuration info
or
    make pristine       # discard results of previous builds
			# and restore pre-defined configuration info

--------------------------------------------------------------------------------
