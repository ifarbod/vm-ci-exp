// SPDX-FileCopyrightText: 2026 iFarbod
//
// SPDX-License-Identifier: LicenseRef-Proprietary

// Linux/ppc64 (little-endian): write=4, exit=1, mmap=90.

export module ib.sys:syscall_linux_ppc64;

import ib.core;

#if defined(IB_OS_LINUX) && defined(IB_ARCH_PPC64)

namespace ib::sys
{
export constexpr usize SYS_WRITE = 4;
export constexpr usize SYS_EXIT = 1;
export constexpr usize SYS_MMAP = 90;

export [[nodiscard]]
auto syscall3(usize n, usize a1, usize a2, usize a3) -> isize
{
    register isize r0 __asm__("r0") = static_cast<isize>(n);
    register isize r3 __asm__("r3") = static_cast<isize>(a1);
    register isize r4 __asm__("r4") = static_cast<isize>(a2);
    register isize r5 __asm__("r5") = static_cast<isize>(a3);
    __asm__ volatile("sc"
        : "+r"(r3)
        : "r"(r0), "r"(r4), "r"(r5)
        : "memory", "cc", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "ctr", "xer");

    return r3;
}

export [[nodiscard]]
auto syscall6(usize n, usize a1, usize a2, usize a3, usize a4, usize a5, usize a6) -> isize
{
    register isize r0 __asm__("r0") = static_cast<isize>(n);
    register isize r3 __asm__("r3") = static_cast<isize>(a1);
    register isize r4 __asm__("r4") = static_cast<isize>(a2);
    register isize r5 __asm__("r5") = static_cast<isize>(a3);
    register isize r6 __asm__("r6") = static_cast<isize>(a4);
    register isize r7 __asm__("r7") = static_cast<isize>(a5);
    register isize r8 __asm__("r8") = static_cast<isize>(a6);
    __asm__ volatile("sc"
        : "+r"(r3)
        : "r"(r0), "r"(r4), "r"(r5), "r"(r6), "r"(r7), "r"(r8)
        : "memory", "cc", "r9", "r10", "r11", "r12", "ctr", "xer");

    return r3;
}
}  // namespace ib::sys

#endif  // IB_OS_LINUX && IB_ARCH_PPC64
