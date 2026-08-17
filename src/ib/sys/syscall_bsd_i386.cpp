// SPDX-FileCopyrightText: 2026 iFarbod
//
// SPDX-License-Identifier: LicenseRef-Proprietary

// Shared BSD raw syscall primitive, i386. Args are passed on the stack (like a
// normal C call), the number goes in eax, and the trap is `int $0x80`.

export module ib.sys:syscall_bsd_i386;

import ib.core;

#if (OS(FREEBSD) || OS(OPENBSD) || OS(NETBSD) || OS(DRAGONFLY)) && ARCH(I386)

namespace ib::sys
{
export [[nodiscard]]
auto syscall3(usize n, usize a1, usize a2, usize a3) -> isize
{
    isize ret = 0;
    __asm__ volatile("pushl %4\n\t"
                     "pushl %3\n\t"
                     "pushl %2\n\t"
                     "pushl %%eax\n\t"
                     "int   $0x80\n\t"
                     "addl  $16, %%esp\n\t"
        : "=a"(ret)
        : "0"(static_cast<isize>(n)), "g"(static_cast<isize>(a1)), "g"(static_cast<isize>(a2)),
        "g"(static_cast<isize>(a3))
        : "memory", "cc");
    return ret;
}

export [[nodiscard]]
auto syscall6(usize n, usize a1, usize a2, usize a3, usize a4, usize a5, usize a6) -> isize
{
    isize ret = 0;
    __asm__ volatile("pushl %7\n\t"
                     "pushl %6\n\t"
                     "pushl %5\n\t"
                     "pushl %4\n\t"
                     "pushl %3\n\t"
                     "pushl %2\n\t"
                     "pushl %%eax\n\t"
                     "int   $0x80\n\t"
                     "addl  $28, %%esp\n\t"
        : "=a"(ret)
        : "0"(static_cast<isize>(n)), "g"(static_cast<isize>(a1)), "g"(static_cast<isize>(a2)),
        "g"(static_cast<isize>(a3)), "g"(static_cast<isize>(a4)), "g"(static_cast<isize>(a5)),
        "g"(static_cast<isize>(a6))
        : "memory", "cc");
    return ret;
}

#if OS(NETBSD) || OS(OPENBSD)
export [[nodiscard]]
auto syscall7(usize n, usize a1, usize a2, usize a3, usize a4, usize a5, usize a6, usize a7) -> isize
{
    isize ret = 0;
    __asm__ volatile("pushl %8\n\t"
                     "pushl %7\n\t"
                     "pushl %6\n\t"
                     "pushl %5\n\t"
                     "pushl %4\n\t"
                     "pushl %3\n\t"
                     "pushl %2\n\t"
                     "pushl %%eax\n\t"
                     "int   $0x80\n\t"
                     "addl  $32, %%esp\n\t"
        : "=a"(ret)
        : "0"(static_cast<isize>(n)), "g"(static_cast<isize>(a1)), "g"(static_cast<isize>(a2)),
        "g"(static_cast<isize>(a3)), "g"(static_cast<isize>(a4)), "g"(static_cast<isize>(a5)),
        "g"(static_cast<isize>(a6)), "g"(static_cast<isize>(a7))
        : "memory", "cc");
    return ret;
}
#endif  // OS(NETBSD) || OS(OPENBSD)
}  // namespace ib::sys

#endif  // any BSD && ARCH(I386)
