/*
 * Copyright (c) 2017 Jean-Paul Etienne <fractalclone@gmail.com>
 * Copyright (c) 2017 Palmer Dabbelt <palmer@dabbelt.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <init.h>
#include <board.h>
#include "prci.h"

/* Selects the 16MHz oscilator on the HiFive1 board, which provides a clock
 * that's accurate enough to actually drive serial ports off of. */
static int hifive1_clock_init(struct device *dev)
{
	ARG_UNUSED(dev);
	uint32_t config_value = 0;
	config_value |= PLL_REFSEL(1);
	config_value |= PLL_BYPASS(1);
	PRCI_REG(PRCI_PLLCFG) = config_value;
	PRCI_REG(PRCI_PLLDIV) = (PLL_FINAL_DIV_BY_1(1) | PLL_FINAL_DIV(0));
	PRCI_REG(PRCI_PLLCFG) |= PLL_SEL(1);
	PRCI_REG(PRCI_HFROSCCFG) &= ~ROSC_EN(1);
	return 0;
}

SYS_INIT(hifive1_clock_init, PRE_KERNEL_1, CONFIG_PINMUX_INIT_PRIORITY);
