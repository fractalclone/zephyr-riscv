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

#include <nanokernel.h>
#include <arch/cpu.h>
#include <kernel_structs.h>
#include <inttypes.h>

#ifdef CONFIG_PRINTK
#include <misc/printk.h>
#define PRINTK(...) printk(__VA_ARGS__)
#else
#define PRINTK(...)
#endif

const NANO_ESF _default_esf = {
	0xdeadbaad,
	0xdeadbaad,
	0xdeadbaad,
	0xdeadbaad,
	0xdeadbaad,
	0xdeadbaad,
	0xdeadbaad,
	0xdeadbaad,
	0xdeadbaad,
	0xdeadbaad,
	0xdeadbaad,
	0xdeadbaad,
	0xdeadbaad,
	0xdeadbaad,
	0xdeadbaad,
	0xdeadbaad,
	0xdeadbaad,
	0xdeadbaad,
	0xdeadbaad,
	0xdeadbaad,
	0xdeadbaad,
	0xdeadbaad,
	0xdeadbaad,
	0xdeadbaad,
	0xdeadbaad,
	0xdeadbaad,
	0xdeadbaad,
	0xdeadbaad,
	0xdeadbaad,
	0xdeadbaad,
	0xdeadbaad,
	0xdeadbaad,
#if defined(CONFIG_SOC_RISCV32_PULPINO)
	0xdeadbaad,
	0xdeadbaad,
	0xdeadbaad,
	0xdeadbaad,
	0xdeadbaad,
	0xdeadbaad,
#endif
};


/**
 *
 * @brief Nanokernel fatal error handler
 *
 * This routine is called when a fatal error condition is detected by either
 * hardware or software.
 *
 * The caller is expected to always provide a usable ESF.  In the event that the
 * fatal error does not have a hardware generated ESF, the caller should either
 * create its own or call _Fault instead.
 *
 * @param reason the reason that the handler was called
 * @param esf pointer to the exception stack frame
 *
 * @return This function does not return.
 */
FUNC_NORETURN void _NanoFatalErrorHandler(unsigned int reason,
					  const NANO_ESF *esf)
{
	switch (reason) {
	case _NANO_ERR_CPU_EXCEPTION:
	case _NANO_ERR_SPURIOUS_INT:
		break;

	case _NANO_ERR_INVALID_TASK_EXIT:
		PRINTK("***** Invalid Exit Software Error! *****\n");
		break;

#if defined(CONFIG_STACK_CANARIES)
	case _NANO_ERR_STACK_CHK_FAIL:
		PRINTK("***** Stack Check Fail! *****\n");
		break;
#endif /* CONFIG_STACK_CANARIES */

	case _NANO_ERR_ALLOCATION_FAIL:
		PRINTK("**** Kernel Allocation Failure! ****\n");
		break;

	default:
		PRINTK("**** Unknown Fatal Error %d! ****\n", reason);
		break;
	}

	PRINTK("Current thread ID = %p\n"
	       "Faulting instruction address = 0x%" PRIx32 "\n"
	       "  ra: 0x%" PRIx32 "  x3: 0x%" PRIx32
	       "  x4: 0x%" PRIx32 "  x5: 0x%" PRIx32 "\n"
	       "  x6: 0x%" PRIx32 "  x7: 0x%" PRIx32
	       "  x8: 0x%" PRIx32 "  x9: 0x%" PRIx32 "\n"
	       " x10: 0x%" PRIx32 " x11: 0x%" PRIx32
	       " x12: 0x%" PRIx32 " x13: 0x%" PRIx32 "\n"
	       " x14: 0x%" PRIx32 " x15: 0x%" PRIx32
	       " x16: 0x%" PRIx32 " x17: 0x%" PRIx32 "\n"
	       " x18: 0x%" PRIx32 " x19: 0x%" PRIx32
	       " x20: 0x%" PRIx32 " x21: 0x%" PRIx32 "\n"
	       " x22: 0x%" PRIx32 " x23: 0x%" PRIx32
	       " x24: 0x%" PRIx32 " x25: 0x%" PRIx32 "\n"
	       " x26: 0x%" PRIx32 " x27: 0x%" PRIx32
	       " x28: 0x%" PRIx32 " x29: 0x%" PRIx32 "\n"
	       " x30: 0x%" PRIx32 " x31: 0x%" PRIx32 "\n",
	       k_current_get(),
	       (esf->mepc == 0xdeadbaad) ? 0xdeadbaad : esf->mepc - 4,
	       esf->x1, esf->x3, esf->x4, esf->x5,
	       esf->x6, esf->x7, esf->x8, esf->x9,
	       esf->x10, esf->x11, esf->x12, esf->x13,
	       esf->x14, esf->x15, esf->x16, esf->x17,
	       esf->x18, esf->x19, esf->x20, esf->x21,
	       esf->x22, esf->x23, esf->x24, esf->x25,
	       esf->x26, esf->x27, esf->x28, esf->x29,
	       esf->x30, esf->x31);

	_SysFatalErrorHandler(reason, esf);
	/* spin forever */
	for (;;)
		__asm__ volatile("nop");
}


/**
 *
 * @brief Fatal error handler
 *
 * This routine implements the corrective action to be taken when the system
 * detects a fatal error.
 *
 * This sample implementation attempts to abort the current thread and allow
 * the system to continue executing, which may permit the system to continue
 * functioning with degraded capabilities.
 *
 * System designers may wish to enhance or substitute this sample
 * implementation to take other actions, such as logging error (or debug)
 * information to a persistent repository and/or rebooting the system.
 *
 * @param reason fatal error reason
 * @param esf pointer to exception stack frame
 *
 * @return N/A
 */
void _SysFatalErrorHandler(unsigned int reason, const NANO_ESF *esf)
{
	ARG_UNUSED(reason);
	ARG_UNUSED(esf);

	if (k_is_in_isr() || _is_thread_essential()) {
		printk("Fatal fault in %s! Spinning...\n",
		       k_is_in_isr() ? "ISR" : "essential thread");
		/* spin forever */
		for (;;)
			__asm__ volatile("nop");
	}
	printk("Fatal fault in thread! Aborting.\n");
	k_thread_abort(_current);

	CODE_UNREACHABLE;
}


static char *cause_str(uint32_t cause)
{
	switch (cause) {
	case 0:
		return "Instruction address misaligned";
	case 1:
		return "Instruction Access fault";
	case 2:
		return "Illegal instruction";
	case 3:
		return "Breakpoint";
	case 4:
		return "Load address misaligned";
	case 5:
		return "Load access fault";
	default:
		return "unknown";
	}
}


FUNC_NORETURN void _Fault(const NANO_ESF *esf)
{
	uint32_t mcause;

	__asm__ volatile("csrr %0, mcause" : "=r" (mcause));

	mcause &= SOC_MCAUSE_IRQ_MASK;

	printk("Exception cause %s (%d)\n", cause_str(mcause), mcause);

	_NanoFatalErrorHandler(_NANO_ERR_CPU_EXCEPTION, esf);
}
