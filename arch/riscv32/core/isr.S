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

#define _ASMLANGUAGE

#include <toolchain.h>
#include <sections.h>
#include <kernel_structs.h>
#include <offsets_short.h>

/* imports */
GDATA(_sw_isr_table)
GTEXT(__soc_save_context)
GTEXT(__soc_restore_context)
GTEXT(__soc_is_irq)
GTEXT(__soc_handle_irq)
GTEXT(_Fault)

GTEXT(_k_neg_eagain)
GTEXT(_is_next_thread_current)
GTEXT(_get_next_ready_thread)

#ifdef CONFIG_KERNEL_EVENT_LOGGER_CONTEXT_SWITCH
GTEXT(_sys_k_event_logger_context_switch)
#endif

#ifdef CONFIG_KERNEL_EVENT_LOGGER_SLEEP
GTEXT(_sys_k_event_logger_exit_sleep)
#endif

#ifdef CONFIG_KERNEL_EVENT_LOGGER_INTERRUPT
GTEXT(_sys_k_event_logger_interrupt)
#endif

#ifdef CONFIG_IRQ_OFFLOAD
GTEXT(_offload_routine)
#endif

/* exports */
GTEXT(__irq_wrapper)

/* use ABI name of registers for the sake of simplicity */

/*
 * ISR is handled at both ARCH and SOC levels.
 * At the ARCH level, ISR handles basic context saving/restore of registers
 * onto/from the thread stack and calls corresponding IRQ function registered
 * at driver level

 * At SOC level, ISR handles saving/restoring of SOC-specific registers
 * onto/from the thread stack (handled via __soc_save_context and
 * __soc_restore_context functions). SOC level save/restore context
 * is accounted for only if CONFIG_RISCV_SOC_CONTEXT_SAVE variable is set
 *
 * Moreover, given that RISC-V architecture does not provide a clear ISA
 * specification about interrupt handling, each RISC-V SOC handles it in
 * its own way. Hence, the generic RISC-V ISR handler expects the following
 * functions to be provided at the SOC level:
 * __soc_is_irq: to check if the exception is the result of an interrupt or not.
 * __soc_handle_irq: handle pending IRQ at SOC level (ex: clear pending IRQ in
 * SOC-specific IRQ register)
 */

/*
 * Handler called upon each exception/interrupt/fault
 * In this architecture, system call (ECALL) is used to perform context
 * switching or IRQ offloading (when enabled).
 */
SECTION_FUNC(exception.entry, __irq_wrapper)
	/* Allocate space on thread stack to save registers */
	addi sp, sp, -__NANO_ESF_SIZEOF

	/*
	 * Save all registers on current thread stack.
	 * NOTE: need to be updated to account for floating-point registers
	 * floating-point registers should be accounted for when corresponding
	 * config variable is set
	 */
	sw x1, __NANO_ESF_x1_OFFSET(sp)
	sw x3, __NANO_ESF_x3_OFFSET(sp)
	sw x4, __NANO_ESF_x4_OFFSET(sp)
	sw x5, __NANO_ESF_x5_OFFSET(sp)
	sw x6, __NANO_ESF_x6_OFFSET(sp)
	sw x7, __NANO_ESF_x7_OFFSET(sp)
	sw x8, __NANO_ESF_x8_OFFSET(sp)
	sw x9, __NANO_ESF_x9_OFFSET(sp)
	sw x10, __NANO_ESF_x10_OFFSET(sp)
	sw x11, __NANO_ESF_x11_OFFSET(sp)
	sw x12, __NANO_ESF_x12_OFFSET(sp)
	sw x13, __NANO_ESF_x13_OFFSET(sp)
	sw x14, __NANO_ESF_x14_OFFSET(sp)
	sw x15, __NANO_ESF_x15_OFFSET(sp)
	sw x16, __NANO_ESF_x16_OFFSET(sp)
	sw x17, __NANO_ESF_x17_OFFSET(sp)
	sw x18, __NANO_ESF_x18_OFFSET(sp)
	sw x19, __NANO_ESF_x19_OFFSET(sp)
	sw x20, __NANO_ESF_x20_OFFSET(sp)
	sw x21, __NANO_ESF_x21_OFFSET(sp)
	sw x22, __NANO_ESF_x22_OFFSET(sp)
	sw x23, __NANO_ESF_x23_OFFSET(sp)
	sw x24, __NANO_ESF_x24_OFFSET(sp)
	sw x25, __NANO_ESF_x25_OFFSET(sp)
	sw x26, __NANO_ESF_x26_OFFSET(sp)
	sw x27, __NANO_ESF_x27_OFFSET(sp)
	sw x28, __NANO_ESF_x28_OFFSET(sp)
	sw x29, __NANO_ESF_x29_OFFSET(sp)
	sw x30, __NANO_ESF_x30_OFFSET(sp)
	sw x31, __NANO_ESF_x31_OFFSET(sp)

	/* Save MEPC register */
	csrr t0, mepc
	sw t0, __NANO_ESF_mepc_OFFSET(sp)

	/* Save SOC-specific MSTATUS register */
	csrr t0, SOC_MSTATUS_REG
	sw t0, __NANO_ESF_mstatus_OFFSET(sp)

#ifdef CONFIG_RISCV_SOC_CONTEXT_SAVE
	/* Handle context saving at SOC level. */
	jal ra, __soc_save_context
#endif /* CONFIG_RISCV_SOC_CONTEXT_SAVE */

	/*
	 * Check if exception is the result of an interrupt or not.
	 * (SOC dependent). Following the RISC-V architecture spec, the MSB
	 * of the mcause register is used to indicate whether an exception
	 * is the result of an interrupt or an exception/fault. But for some
	 * SOCs (like pulpino or riscv-qemu), the MSB is never set to indicate
	 * interrupt. Hence, check for interrupt/exception via the __soc_is_irq
	 * function (that needs to be implemented by each SOC). The result is
	 * returned via register a0 (1: interrupt, 0 exception)
	 */
	jal ra, __soc_is_irq

	/* If a0 != 0, jump to is_interrupt */
	addi t1, x0, 0
	bnez a0, is_interrupt

	/*
	 * If exception is not an interrupt, MEPC will contain
	 * the instruction address, which has caused the exception.
	 * Increment saved MEPC by 4 to prevent running into the
	 * exception again, upon exiting the ISR.
	 */
	lw t0, __NANO_ESF_mepc_OFFSET(sp)
	addi t0, t0, 4
	sw t0, __NANO_ESF_mepc_OFFSET(sp)

	/*
	 * If the exception is the result of an ECALL, check whether to
	 * perform a context switch or an IRQ offload. Otherwise call _Fault
	 * to report the exception.
	 */
	csrr t0, mcause
	li t2, SOC_MCAUSE_IRQ_MASK
	and t0, t0, t2
	li t1, SOC_MCAUSE_ECALL_EXP

	/*
	 * If mcause == SOC_MCAUSE_ECALL_EXP, handle system call,
	 * otherwise handle fault
	 */
#ifdef CONFIG_IRQ_OFFLOAD
	/* If not system call, jump to is_fault */
	bne t0, t1, is_fault

	/*
	 * Determine if the system call is the result of an IRQ offloading.
	 * Done by checking if _offload_routine is not pointing to NULL.
	 * If NULL, jump to reschedule to perform a context switch, otherwise,
	 * jump to is_interrupt to handle the IRQ offload.
	 */
	la t0, _offload_routine
	lw t1, 0x00(t0)
	beqz t1, reschedule
	bnez t1, is_interrupt

is_fault:
#else
	/*
	 * Go to reschedule to handle context-switch if system call,
	 * otherwise call _Fault to handle exception
	 */
	beq t0, t1, reschedule
#endif

	/*
	 * Call _Fault to handle exception.
	 * Stack pointer is pointing to a NANO_ESF structure, pass it
	 * to _Fault (via register a0).
	 * If _Fault shall return, set return address to no_reschedule
	 * to restore stack.
	 */
	addi a0, sp, 0
	la ra, no_reschedule
	tail _Fault

is_interrupt:
	/*
	 * Save current thread stack pointer and switch
	 * stack pointer to interrupt stack.
	 */

	/* Save thread stack pointer to temp register t0 */
	addi t0, sp, 0

	/* Switch to interrupt stack */
	la t2, _kernel
	lw sp, _kernel_offset_to_irq_stack(t2)

	/*
	 * Save thread stack pointer on interrupt stack
	 * In RISC-V, stack pointer needs to be 16-byte aligned
	 */
	addi sp, sp, -16
	sw t0, 0x00(sp)

on_irq_stack:
	/* Increment _kernel.nested variable */
	lw t3, _kernel_offset_to_nested(t2)
	addi t3, t3, 1
	sw t3, _kernel_offset_to_nested(t2)

	/*
	 * If we are here due to a system call, t1 register should != 0.
	 * In this case, perform IRQ offloading, otherwise jump to call_irq
	 */
	beqz t1, call_irq

	/*
	 * Call _irq_do_offload to handle IRQ offloading.
	 * Set return address to on_thread_stack in order to jump there
	 * upon returning from _irq_do_offload
	 */
	la ra, on_thread_stack
	tail _irq_do_offload

call_irq:
#ifdef CONFIG_KERNEL_EVENT_LOGGER_SLEEP
	call _sys_k_event_logger_exit_sleep
#endif

#ifdef CONFIG_KERNEL_EVENT_LOGGER_INTERRUPT
	call _sys_k_event_logger_interrupt
#endif

	/* Get IRQ causing interrupt */
	csrr a0, mcause
	li t0, SOC_MCAUSE_IRQ_MASK
	and a0, a0, t0

	/*
	 * Clear pending IRQ generating the interrupt at SOC level
	 * Pass IRQ number to __soc_handle_irq via register a0
	 */
	jal ra, __soc_handle_irq

	/*
	 * Call corresponding registered function in _sw_isr_table.
	 * (table is 8-bytes wide, we should shift index by 3)
	 */
	la t0, _sw_isr_table
	slli a0, a0, 3
	add t0, t0, a0

	/* Load argument in a0 register */
	lw a0, 0x00(t0)

	/* Load ISR function address in register t1 */
	lw t1, 0x04(t0)

	/* Call ISR function */
	jalr ra, t1

on_thread_stack:
	/* Get reference to _kernel */
	la t1, _kernel

	/* Decrement _kernel.nested variable */
	lw t2, _kernel_offset_to_nested(t1)
	addi t2, t2, -1
	sw t2, _kernel_offset_to_nested(t1)

	/* Restore thread stack pointer */
	lw t0, 0x00(sp)
	addi sp, t0, 0

	/*
	 * Check if we need to perform a reschedule
	 */

	/* Get pointer to _kernel.current */
	lw t2, _kernel_offset_to_current(t1)

	/*
	 * Check if current thread is a coop thread (prio < 0).
	 * If yes, do not perform a reschedule
	 */
	lw t3, _thread_offset_to_prio(t2)
	bltz t3, no_reschedule

	/*
	 * Check if scheduler is locked, (sched_locked > 0).
	 * If yes, do not perform a reschedule
	 */
	lw t3, _thread_offset_to_sched_locked(t2)
	bgtz t3, no_reschedule

	/*
	 * Check if next_thread is current thread.
	 * If yes do not perform a reschedule
	 */
	jal ra,  _is_next_thread_current
	bnez a0, no_reschedule

reschedule:
#if CONFIG_KERNEL_EVENT_LOGGER_CONTEXT_SWITCH
	call _sys_k_event_logger_context_switch
#endif /* CONFIG_KERNEL_EVENT_LOGGER_CONTEXT_SWITCH */

	/* Get reference to _kernel */
	la t0, _kernel

	/* Get pointer to _kernel.current */
	lw t1, _kernel_offset_to_current(t0)

	/*
	 * Save stack pointer of current thread and set the default return value
	 * of _Swap to _k_neg_eagain for the thread.
	 */
	sw sp, _thread_offset_to_sp(t1)
	la t2, _k_neg_eagain
	lw t3, 0x00(t2)
	sw t3, _thread_offset_to_retval(t1)

	/* Call _get_next_ready_thread to get next thread to schedule. */
	call _get_next_ready_thread

	/*
	 * Get reference to _kernel again
	 * (_get_next_ready_thread might have tampered previous value of t0)
	 */
	la t0, _kernel

	/*
	 * After a call to _get_next_ready_thread, a0 register contains a
	 * pointer to the new thread. Set _kernel.current to new thread.
	 */
	sw a0, _kernel_offset_to_current(t0)

	/* Switch to new thread stack */
	lw sp, _thread_offset_to_sp(a0)

no_reschedule:
#ifdef CONFIG_RISCV_SOC_CONTEXT_SAVE
	/* Restore context at SOC level */
	jal ra, __soc_restore_context
#endif /* CONFIG_RISCV_SOC_CONTEXT_SAVE */

	/* Restore all registers from thread stack */
	lw x1, __NANO_ESF_x1_OFFSET(sp)
	lw x3, __NANO_ESF_x3_OFFSET(sp)
	lw x4, __NANO_ESF_x4_OFFSET(sp)
	lw x5, __NANO_ESF_x5_OFFSET(sp)
	lw x6, __NANO_ESF_x6_OFFSET(sp)
	lw x7, __NANO_ESF_x7_OFFSET(sp)
	lw x8, __NANO_ESF_x8_OFFSET(sp)
	lw x9, __NANO_ESF_x9_OFFSET(sp)
	lw x10, __NANO_ESF_x10_OFFSET(sp)
	lw x11, __NANO_ESF_x11_OFFSET(sp)
	lw x12, __NANO_ESF_x12_OFFSET(sp)
	lw x13, __NANO_ESF_x13_OFFSET(sp)
	lw x14, __NANO_ESF_x14_OFFSET(sp)
	lw x15, __NANO_ESF_x15_OFFSET(sp)
	lw x16, __NANO_ESF_x16_OFFSET(sp)
	lw x17, __NANO_ESF_x17_OFFSET(sp)
	lw x18, __NANO_ESF_x18_OFFSET(sp)
	lw x19, __NANO_ESF_x19_OFFSET(sp)
	lw x20, __NANO_ESF_x20_OFFSET(sp)
	lw x21, __NANO_ESF_x21_OFFSET(sp)
	lw x22, __NANO_ESF_x22_OFFSET(sp)
	lw x23, __NANO_ESF_x23_OFFSET(sp)
	lw x24, __NANO_ESF_x24_OFFSET(sp)
	lw x25, __NANO_ESF_x25_OFFSET(sp)
	lw x26, __NANO_ESF_x26_OFFSET(sp)
	lw x27, __NANO_ESF_x27_OFFSET(sp)
	lw x28, __NANO_ESF_x28_OFFSET(sp)
	lw x29, __NANO_ESF_x29_OFFSET(sp)
	lw x30, __NANO_ESF_x30_OFFSET(sp)
	lw x31, __NANO_ESF_x31_OFFSET(sp)

	/* Restore MEPC register */
	lw t0, __NANO_ESF_mepc_OFFSET(sp)
	csrw mepc, t0

	/* Restore SOC-specific MSTATUS register */
	lw t0, __NANO_ESF_mstatus_OFFSET(sp)
	csrw SOC_MSTATUS_REG, t0

	/* Release stack space */
	addi sp, sp, __NANO_ESF_SIZEOF

	/* Call SOC_ERET to exit ISR */
	SOC_ERET
