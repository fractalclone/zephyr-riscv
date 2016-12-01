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

#ifndef _ASM_INLINE_GCC_H
#define _ASM_INLINE_GCC_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The file must not be included directly
 * Include arch/cpu.h instead
 * NOTE: only pulpino-specific NOT RISCV32
 */

#ifndef _ASMLANGUAGE

#include <toolchain.h>

/*
 * Account for pulpino-specific bit manipulation opcodes only when
 * CONFIG_RISCV_GENERIC_TOOLCHAIN is not set
 */

#ifndef CONFIG_RISCV_GENERIC_TOOLCHAIN
/**
 *
 * @brief find least significant bit set in a 32-bit word
 *
 * This routine finds the first bit set starting from the least significant bit
 * in the argument passed in and returns the index of that bit.  Bits are
 * numbered starting at 1 from the least significant bit.  A return value of
 * zero indicates that the value passed is zero.
 *
 * @return least significant bit set, 0 if @a op is 0
 */
static ALWAYS_INLINE unsigned int find_lsb_set(uint32_t op)
{
	unsigned int ret;

	if (!op)
		return 0;
	__asm__ volatile ("p.ff1 %[d], %[a]"
			  : [d] "=r" (ret)
			  : [a] "r" (op));
	return ret + 1;
}

/**
 *
 * @brief find most significant bit set in a 32-bit word
 *
 * This routine finds the first bit set starting from the most significant bit
 * in the argument passed in and returns the index of that bit.  Bits are
 * numbered starting at 1 from the least significant bit.  A return value of
 * zero indicates that the value passed is zero.
 *
 * @return most significant bit set, 0 if @a op is 0
 */
static ALWAYS_INLINE unsigned int find_msb_set(uint32_t op)
{
	unsigned int ret;

	if (!op)
		return 0;
	__asm__ volatile ("p.fl1 %[d], %[a]"
			  : [d] "=r" (ret)
			  : [a] "r" (op));
	return ret + 1;
}

#else /* CONFIG_RISCV_GENERIC_TOOLCHAIN */

/*
 * When compilied with a riscv32 generic toolchain, provide
 * C implementations of find_lsb_set and find_msb_set functions
 */
static ALWAYS_INLINE unsigned int find_lsb_set(uint32_t op)
{
	int i;

	if (!op)
		return 0;

	for (i = 0; i < 32; i++) {
		if (op & (1 << i))
			return i + 1;
	}

	return 0;
}

static ALWAYS_INLINE unsigned int find_msb_set(uint32_t op)
{
	int i;

	if (!op)
		return 0;

	for (i = 31; i >= 0; i--) {
		if (op & (1 << i))
			return i + 1;
	}

	return 0;
}

#endif /* CONFIG_RISCV_GENERIC_TOOLCHAIN */

#endif /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* _ASM_INLINE_GCC_PUBLIC_GCC_H */
