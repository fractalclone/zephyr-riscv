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

/**
 * @file
 * @brief RISCV32 specific nanokernel interface header
 * This header contains the RISCV32 specific nanokernel interface.  It is
 * included by the generic nanokernel interface header (nanokernel.h)
 */

#ifndef _RISCV32_ARCH_H_
#define _RISCV32_ARCH_H_

#include "exp.h"
#include "sys_io.h"

#include <irq.h>
#include <sw_isr_table.h>
#include <soc.h>

#ifdef __cplusplus
extern "C" {
#endif

/* stacks, for RISCV architecture stack should be 16byte-aligned */
#define STACK_ALIGN  16

#ifndef _ASMLANGUAGE
#include <misc/util.h>

#define STACK_ROUND_UP(x) ROUND_UP(x, STACK_ALIGN)
#define STACK_ROUND_DOWN(x) ROUND_DOWN(x, STACK_ALIGN)

/* APIs need to support non-byte addressable architectures */

#define OCTET_TO_SIZEOFUNIT(X) (X)
#define SIZEOFUNIT_TO_OCTET(X) (X)

/* macros convert value of it's argument to a string */
#define DO_TOSTR(s) #s
#define TOSTR(s) DO_TOSTR(s)

/* concatenate the values of the arguments into one */
#define DO_CONCAT(x, y) x ## y
#define CONCAT(x, y) DO_CONCAT(x, y)

/*
 * SOC-specific function to get the IRQ number generating the interrupt.
 * __soc_get_irq returns a bitfield of pending IRQs.
 */
extern uint32_t __soc_get_irq(void);

void _arch_irq_enable(unsigned int irq);
void _arch_irq_disable(unsigned int irq);
int _arch_irq_is_enabled(unsigned int irq);


/**
 * Configure a static interrupt.
 *
 * All arguments must be computable by the compiler at build time.
 *
 * Internally this function does a few things:
 *
 * 1. The enum statement has no effect but forces the compiler to only
 * accept constant values for the irq_p parameter, very important as the
 * numerical IRQ line is used to create a named section.
 *
 * 2. An instance of _IsrTableEntry is created containing the ISR and its
 * parameter. If you look at how _sw_isr_table is created, each entry in the
 * array is in its own section named by the IRQ line number. What we are doing
 * here is to override one of the default entries (which points to the
 * spurious IRQ handler) with what was supplied here.
 *
 * 3. interrupt priority is not supported by pulpino core
 *
 * @param irq_p IRQ line number
 * @param priority_p Interrupt priority
 * @param isr_p Interrupt service routine
 * @param isr_param_p ISR parameter
 * @param flags_p IRQ options
 *
 * @return The vector assigned to this interrupt
 */
#define _ARCH_IRQ_CONNECT(irq_p, priority_p, isr_p, isr_param_p, flags_p) \
({ \
	enum { IRQ = irq_p }; \
	static struct _IsrTableEntry _CONCAT(_isr_irq, irq_p) \
		__attribute__ ((used)) \
		__attribute__ ((section(STRINGIFY(_CONCAT(.gnu.linkonce.isr_irq, irq_p))))) = \
			{isr_param_p, isr_p}; \
	irq_p; \
})

/*
 * use atomic instruction csrrc to lock global irq
 * csrrc: atomic read and clear bits in CSR register
 */
static ALWAYS_INLINE unsigned int _arch_irq_lock(void)
{
	unsigned int key, mstatus;

	__asm__ volatile ("csrrc %0, mstatus, %1"
			  : "=r" (mstatus)
			  : "r" (SOC_MSTATUS_IEN));

	key = (mstatus & SOC_MSTATUS_IEN);
	return key;
}

/*
 * use atomic instruction csrrs to unlock global irq
 * csrrs: atomic read and set bits in CSR register
 */
static ALWAYS_INLINE void _arch_irq_unlock(unsigned int key)
{
	unsigned int mstatus;

	__asm__ volatile ("csrrs %0, mstatus, %1"
			  : "=r" (mstatus)
			  : "r" (key & SOC_MSTATUS_IEN));
}

#endif /*_ASMLANGUAGE */

#if defined(CONFIG_SOC_RISCV32_PULPINO)
#include <arch/riscv32/pulpino/asm_inline.h>
#elif defined(CONFIG_SOC_RISCV32_QEMU)
#include <arch/riscv32/riscv32-qemu/asm_inline.h>
#endif

#ifdef __cplusplus
}
#endif

#endif
