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
 * @brief Private kernel definitions
 *
 * This file contains private kernel structures definitions and various
 * other definitions for the RISCV32 processor architecture.
 */

#ifndef _kernel_arch_data_h_
#define _kernel_arch_data_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <toolchain.h>
#include <sections.h>
#include <arch/cpu.h>

#ifndef _ASMLANGUAGE
#include <kernel.h>
#include <stdint.h>
#include <misc/util.h>
#include <misc/dlist.h>
#include <nano_internal.h>

/* Bitmask definitions for the struct k_thread->flags bit field */
#define INT_ACTIVE    (1 << 1) /* 1 = executing context is interrupt handler */
#define EXC_ACTIVE    (1 << 2) /* 1 = executing context is exception handler */

/*
 * The following structure defines the list of registers that need to be
 * saved/restored when a cooperative context switch occurs.
 */
struct _callee_saved {
	uint32_t sp;     /* Stack pointer, (x2 register) */
	uint32_t retval; /* Return value of _Swap() */
};
typedef struct _callee_saved _callee_saved_t;

struct _caller_saved {
	/*
	 * Nothing here, the exception code puts all the caller-saved
	 * registers onto the stack.
	 */
};

typedef struct _caller_saved _caller_saved_t;

struct _thread_arch {
	/* nothing for now */
};

typedef struct _thread_arch _thread_arch_t;

struct _kernel_arch {
	/* nothing for now */
};

typedef struct _kernel_arch _kernel_arch_t;

extern char _interrupt_stack[CONFIG_ISR_STACK_SIZE];

#endif /* _ASMLANGUAGE */

#endif /* _kernel_arch_data_h_ */
