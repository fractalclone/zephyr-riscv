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

#include <toolchain.h>
#include <irq.h>
#include <soc.h>

#include <misc/kernel_event_logger.h>

static ALWAYS_INLINE void pulpino_idle(unsigned int key)
{
#ifdef CONFIG_KERNEL_EVENT_LOGGER_SLEEP
	_sys_k_event_logger_enter_sleep();
#endif
	/* unlock interrupts */
	irq_unlock(key);

	/* Put CPU core to sleep via SCR register */
	PULP_SCR = 0x01;

	/* Wait for interrupt */
	__asm__ volatile("wfi");
}

/**
 *
 * @brief Power save idle routine
 *
 * This function will be called by the nanokernel idle loop or possibly within
 * an implementation of _sys_power_save_idle in the microkernel when the
 * '_sys_power_save_flag' variable is non-zero.
 *
 * @return N/A
 */
void nano_cpu_idle(void)
{
	pulpino_idle(SOC_MSTATUS_IEN);
}

/**
 *
 * @brief Atomically re-enable interrupts and enter low power mode
 *
 * This function is utilized by the nanokernel object "wait" APIs for tasks,
 * e.g. nano_task_lifo_get(), nano_task_sem_take(),
 * nano_task_stack_pop(), and nano_task_fifo_get().
 *
 * INTERNAL
 * The requirements for nano_cpu_atomic_idle() are as follows:
 * 1) The enablement of interrupts and entering a low-power mode needs to be
 *    atomic, i.e. there should be no period of time where interrupts are
 *    enabled before the processor enters a low-power mode.  See the comments
 *    in nano_task_lifo_get(), for example, of the race condition that
 *    occurs if this requirement is not met.
 *
 * 2) After waking up from the low-power mode, the interrupt lockout state
 *    must be restored as indicated in the 'imask' input parameter.
 *
 * @return N/A
 */
void nano_cpu_atomic_idle(unsigned int key)
{
	pulpino_idle(key);
}
