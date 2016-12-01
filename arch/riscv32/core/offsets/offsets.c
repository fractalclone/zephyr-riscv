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
 * @brief RISCV32 kernel structure member offset definition file
 *
 * This module is responsible for the generation of the absolute symbols whose
 * value represents the member offsets for various RISCV32 kernel
 * structures.
 */

#include <gen_offset.h>
#include <kernel_structs.h>
#include <kernel_offsets.h>

/* struct coop member offsets */
GEN_OFFSET_SYM(_callee_saved_t, sp);
GEN_OFFSET_SYM(_callee_saved_t, retval);

GEN_OFFSET_SYM(NANO_ESF, x1);
GEN_OFFSET_SYM(NANO_ESF, x3);
GEN_OFFSET_SYM(NANO_ESF, x4);
GEN_OFFSET_SYM(NANO_ESF, x5);
GEN_OFFSET_SYM(NANO_ESF, x6);
GEN_OFFSET_SYM(NANO_ESF, x7);
GEN_OFFSET_SYM(NANO_ESF, x8);
GEN_OFFSET_SYM(NANO_ESF, x9);
GEN_OFFSET_SYM(NANO_ESF, x10);
GEN_OFFSET_SYM(NANO_ESF, x11);
GEN_OFFSET_SYM(NANO_ESF, x12);
GEN_OFFSET_SYM(NANO_ESF, x13);
GEN_OFFSET_SYM(NANO_ESF, x14);
GEN_OFFSET_SYM(NANO_ESF, x15);
GEN_OFFSET_SYM(NANO_ESF, x16);
GEN_OFFSET_SYM(NANO_ESF, x17);
GEN_OFFSET_SYM(NANO_ESF, x18);
GEN_OFFSET_SYM(NANO_ESF, x19);
GEN_OFFSET_SYM(NANO_ESF, x20);
GEN_OFFSET_SYM(NANO_ESF, x21);
GEN_OFFSET_SYM(NANO_ESF, x22);
GEN_OFFSET_SYM(NANO_ESF, x23);
GEN_OFFSET_SYM(NANO_ESF, x24);
GEN_OFFSET_SYM(NANO_ESF, x25);
GEN_OFFSET_SYM(NANO_ESF, x26);
GEN_OFFSET_SYM(NANO_ESF, x27);
GEN_OFFSET_SYM(NANO_ESF, x28);
GEN_OFFSET_SYM(NANO_ESF, x29);
GEN_OFFSET_SYM(NANO_ESF, x30);
GEN_OFFSET_SYM(NANO_ESF, x31);
GEN_OFFSET_SYM(NANO_ESF, mepc);
GEN_OFFSET_SYM(NANO_ESF, mstatus);

#if defined(CONFIG_SOC_RISCV32_PULPINO)
GEN_OFFSET_SYM(NANO_ESF, lpstart0);
GEN_OFFSET_SYM(NANO_ESF, lpend0);
GEN_OFFSET_SYM(NANO_ESF, lpcount0);
GEN_OFFSET_SYM(NANO_ESF, lpstart1);
GEN_OFFSET_SYM(NANO_ESF, lpend1);
GEN_OFFSET_SYM(NANO_ESF, lpcount1);
#endif

/*
 * RISC-V requires the stack to be 16-bytes aligned, hence SP needs to grow or
 * shrink by a size, which follows the RISC-V stack alignment requirements
 * Hence, ensure that __tTCS_NOFLOAT_SIZEOF and __tTCS_NOFLOAT_SIZEOF sizes
 * are aligned accordingly.
 */
GEN_ABSOLUTE_SYM(__NANO_ESF_SIZEOF, STACK_ROUND_UP(sizeof(NANO_ESF)));

/* size of the struct tcs structure sans save area for floating point regs */
GEN_ABSOLUTE_SYM(_K_THREAD_NO_FLOAT_SIZEOF,
		 STACK_ROUND_UP(sizeof(struct k_thread)));

GEN_ABS_SYM_END
