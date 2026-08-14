// SPDX-FileCopyrightText: 2026 iFarbod
//
// SPDX-License-Identifier: LicenseRef-Proprietary

// Linux/ARMv7 (EABI): write=4, exit=1, mmap2=192 (offset in 4096-byte units).

export module ib.sys:syscall_linux_arm;

import ib.core;

#if defined(IB_OS_LINUX) && defined(IB_ARCH_ARM)

namespace ib::sys
{
export constexpr usize SYS_WRITE = 4;
export constexpr usize SYS_EXIT = 1;
export constexpr usize SYS_MMAP2 = 192;

export [[nodiscard]]
auto syscall3(usize n, usize a1, usize a2, usize a3) -> isize
{
    register isize r0 __asm__("r0") = static_cast<isize>(a1);
    register isize r1 __asm__("r1") = static_cast<isize>(a2);
    register isize r2 __asm__("r2") = static_cast<isize>(a3);
    register isize r7 __asm__("r7") = static_cast<isize>(n);

    __asm__ volatile("svc 0" : "+r"(r0) : "r"(r1), "r"(r2), "r"(r7) : "memory", "cc");

    return r0;
}

export [[nodiscard]]
auto syscall6(usize n, usize a1, usize a2, usize a3, usize a4, usize a5, usize a6) -> isize
{
    register isize r0 __asm__("r0") = static_cast<isize>(a1);
    register isize r1 __asm__("r1") = static_cast<isize>(a2);
    register isize r2 __asm__("r2") = static_cast<isize>(a3);
    register isize r3 __asm__("r3") = static_cast<isize>(a4);
    register isize r4 __asm__("r4") = static_cast<isize>(a5);
    register isize r5 __asm__("r5") = static_cast<isize>(a6);
    register isize r7 __asm__("r7") = static_cast<isize>(n);

    __asm__ volatile("svc 0" : "+r"(r0) : "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5), "r"(r7) : "memory", "cc");

    return r0;
}
}  // namespace ib::sys

#endif  // IB_OS_LINUX && IB_ARCH_ARM
