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

#ifndef _ASM_INLINE_PUBLIC_H
#define _ASM_INLINE_PUBLIC_H

/*
 * The file must not be included directly
 * Include nanokernel/cpu.h instead
 */

#if defined(__GNUC__)
#include <arch/riscv32/pulpino/asm_inline_gcc.h>
#else
#include <arch/riscv32/pulpino/asm_inline_other.h>
#endif

#endif /* _ASM_INLINE_PUBLIC_H */
