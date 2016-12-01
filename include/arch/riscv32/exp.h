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
 * @brief RISCV32 public exception handling
 *
 * RISCV32-specific nanokernel exception handling interface.
 */

#ifndef _ARCH_RISCV32_EXP_H_
#define _ARCH_RISCV32_EXP_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE
#include <stdint.h>
#include <toolchain.h>

struct __esf {
	uint32_t x1;       /* return address */
	uint32_t x3;       /* global pointer */
	uint32_t x4;       /* thread pointer */

	/* temporaries caller-saved registers */
	uint32_t x5;
	uint32_t x6;
	uint32_t x7;

	uint32_t x8;        /* saved register/frame pointer */
	uint32_t x9;        /* saved register */

	uint32_t x10;       /* function argument/return value */
	uint32_t x11;       /* return value */
	uint32_t x12;       /* function argument */
	uint32_t x13;       /* function argument */
	uint32_t x14;       /* function argument */
	uint32_t x15;       /* function argument */
	uint32_t x16;       /* function argument */
	uint32_t x17;       /* function argument */

	uint32_t x18;       /* saved register */
	uint32_t x19;       /* saved register */
	uint32_t x20;       /* saved register */
	uint32_t x21;       /* saved register */
	uint32_t x22;       /* saved register */
	uint32_t x23;       /* saved register */
	uint32_t x24;       /* saved register */
	uint32_t x25;       /* saved register */
	uint32_t x26;       /* saved register */
	uint32_t x27;       /* saved register */

	/* temporaries caller-saved registers */
	uint32_t x28;
	uint32_t x29;
	uint32_t x30;
	uint32_t x31;

	uint32_t mepc;      /* machine exception program counter */
	uint32_t mstatus;   /* machine status register */

#if defined(CONFIG_SOC_RISCV32_PULPINO)
	/* pulpino hardware loop registers */
	uint32_t lpstart0;
	uint32_t lpend0;
	uint32_t lpcount0;
	uint32_t lpstart1;
	uint32_t lpend1;
	uint32_t lpcount1;
#endif
};

typedef struct __esf NANO_ESF;
extern const NANO_ESF _default_esf;

extern FUNC_NORETURN void _NanoFatalErrorHandler(unsigned int reason,
						 const NANO_ESF *esf);
extern void _SysFatalErrorHandler(unsigned int reason,
				  const NANO_ESF *esf);

#endif /* _ASMLANGUAGE */

#define _NANO_ERR_CPU_EXCEPTION (0)      /* Any unhandled exception */
#define _NANO_ERR_INVALID_TASK_EXIT (1)  /* Invalid task exit */
#define _NANO_ERR_STACK_CHK_FAIL (2)     /* Stack corruption detected */
#define _NANO_ERR_ALLOCATION_FAIL (3)    /* Kernel Allocation Failure */
#define _NANO_ERR_SPURIOUS_INT (4)	 /* Spurious interrupt */

#ifdef __cplusplus
}
#endif

#endif /* _ARCH_RISCV32_EXP_H_ */
