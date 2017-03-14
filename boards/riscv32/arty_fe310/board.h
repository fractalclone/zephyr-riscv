/*
 * Copyright (c) 2017 Jean-Paul Etienne <fractalclone@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __INC_BOARD_H
#define __INC_BOARD_H

#include <soc.h>

/*
 * UART clock configurations
 *
 * Define them here so that it can be replaced by global variables
 * on other boards where the uart clock is determined dynamically
 * following the PLL configuration
 */
#define uart_fe310_port_0_clk_freq    65000000
#define uart_fe310_port_1_clk_freq    65000000

/* LEDS configuration */
#define LED0_GPIO_PORT        "gpio0"
#define LED1_GPIO_PORT        "gpio0"
#define LED2_GPIO_PORT        "gpio0"
#define LED3_GPIO_PORT        "gpio0"

#define LED0_GPIO_PIN         14
#define LED1_GPIO_PIN         2
#define LED2_GPIO_PIN         21
#define LED3_GPIO_PIN         12

/* Push Buttons configurations */
#define SW0_GPIO_NAME         "gpio0"
#define SW1_GPIO_NAME         "gpio0"
#define SW2_GPIO_NAME         "gpio0"

#define SW0_GPIO_PIN	      15
#define SW1_GPIO_PIN	      30
#define SW2_GPIO_PIN	      31

/*
 * GPIO PULL-UP configs
 */
#define SW0_GPIO_PULL_UP      0
#define SW1_GPIO_PULL_UP      0
#define SW2_GPIO_PULL_UP      0

/* Interrupt config for push buttons */
#define SW0_GPIO_INT_CONF     (GPIO_INT_EDGE | GPIO_INT_ACTIVE_HIGH)
#define SW1_GPIO_INT_CONF     SW0_GPIO_INT_CONF
#define SW2_GPIO_INT_CONF     SW0_GPIO_INT_CONF

#endif /* __INC_BOARD_H */
