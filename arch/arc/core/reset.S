/*
 * Copyright (c) 2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Reset handler
 *
 * Reset handler that prepares the system for running C code.
 */

// #include <board.h>
#include <toolchain.h>
#include <linker/sections.h>
#include <arch/cpu.h>

GDATA(_interrupt_stack)
GDATA(_firq_stack)
GDATA(_main_stack)

/* use one of the available interrupt stacks during init */

/* FIRQ only ? */
#if CONFIG_NUM_IRQ_PRIO_LEVELS == 1

	/* FIRQ, but uses _interrupt_stack ? */
	#if CONFIG_RGF_NUM_BANKS == 1
		#define INIT_STACK _interrupt_stack
		#define INIT_STACK_SIZE CONFIG_ISR_STACK_SIZE
	#else
		#define INIT_STACK _firq_stack
		#define INIT_STACK_SIZE CONFIG_FIRQ_STACK_SIZE
	#endif
#else
	#define INIT_STACK _interrupt_stack
	#define INIT_STACK_SIZE CONFIG_ISR_STACK_SIZE
#endif

GTEXT(__reset)
GTEXT(__start)

/**
 *
 * @brief Reset vector
 *
 * Ran when the system comes out of reset. The processor is at supervisor level.
 *
 * Locking interrupts prevents anything from interrupting the CPU.
 *
 * When these steps are completed, jump to _PrepC(), which will finish setting
 * up the system for running C code.
 *
 * @return N/A
 */

SECTION_FUNC(TEXT,__reset)
SECTION_FUNC(TEXT,__start)

	/* lock interrupts: will get unlocked when switch to main task */
	clri

	mov r1, 1

invalidate_and_disable_icache:

	lr r0, [_ARC_V2_I_CACHE_BUILD]
	and.f r0, r0, 0xff
	bz.nd invalidate_dcache

	mov_s r2, 0
	sr r2, [_ARC_V2_IC_IVIC]
	/* writing to IC_IVIC needs 3 NOPs */
	nop
	nop
	nop
	sr r1, [_ARC_V2_IC_CTRL]

invalidate_dcache:

	lr r3, [_ARC_V2_D_CACHE_BUILD]
	and.f r3, r3, 0xff
	bz.nd done_cache_invalidate

	sr r1, [_ARC_V2_DC_IVDC]

done_cache_invalidate:

#if defined(CONFIG_SYS_POWER_DEEP_SLEEP) && \
	!defined(CONFIG_BOOTLOADER_CONTEXT_RESTORE)
        jl @_sys_soc_resume_from_deep_sleep
#endif

#ifdef CONFIG_INIT_STACKS
	/*
	 * use the main stack to call memset on the interrupt stack and the
	 * FIRQ stack when CONFIG_INIT_STACKS is enabled before switching to
	 * one of them for the rest of the early boot
	 */
	mov sp, _main_stack
	add sp, sp, CONFIG_MAIN_STACK_SIZE

	mov_s r0, _interrupt_stack
	mov_s r1, 0xaa
	mov_s r2, CONFIG_ISR_STACK_SIZE
	jl memset

#if CONFIG_RGF_NUM_BANKS != 1
	mov_s r0, _firq_stack
	mov_s r1, 0xaa
	mov_s r2, CONFIG_FIRQ_STACK_SIZE
	jl memset
#endif

#endif /* CONFIG_INIT_STACKS */

	mov sp, INIT_STACK
	add sp, sp, INIT_STACK_SIZE

	j @_PrepC
