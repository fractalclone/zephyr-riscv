/*
 * Copyright (c) 2017 Jean-Paul Etienne <fractalclone@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <init.h>
#include <pinmux.h>
#include <board.h>

static int arty_fe310_pinmux_init(struct device *dev)
{
	ARG_UNUSED(dev);

	struct device *p = device_get_binding(CONFIG_PINMUX_FE310_0_NAME);

	/* UART0 RX */
	pinmux_pin_set(p, 16, FE310_PINMUX_IOF0);

	/* UART0 TX */
	pinmux_pin_set(p, 17, FE310_PINMUX_IOF0);

	return 0;
}

SYS_INIT(arty_fe310_pinmux_init, PRE_KERNEL_1, CONFIG_PINMUX_INIT_PRIORITY);
