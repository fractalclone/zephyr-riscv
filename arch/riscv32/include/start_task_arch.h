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
 * @brief RISCV32 microkernel declarations to start a task
 *
 * RISCV32 parts of start_task().
 *
 * Currently empty, only here for abstraction.
 */

#ifndef _START_TASK_ARCH__H_
#define _START_TASK_ARCH__H_

#include <toolchain.h>
#include <sections.h>

#include <micro_private.h>
#include <kernel_structs.h>
#include <microkernel/task.h>

#ifdef __cplusplus
extern "C" {
#endif

#define _START_TASK_ARCH(task, opt_ptr) \
	do {/* nothing */              \
	} while ((0))

#ifdef __cplusplus
}
#endif

#endif /* _START_TASK_ARCH__H_ */
