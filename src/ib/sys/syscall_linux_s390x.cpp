// SPDX-FileCopyrightText: 2026 iFarbod
//
// SPDX-License-Identifier: LicenseRef-Proprietary

// Linux/s390x (IBM Z): write=4, exit=1, mmap=90.

export module ib.sys:syscall_linux_s390x;

import ib.core;

#if OS(LINUX) && ARCH(S390X)

namespace ib::sys
{
export constexpr usize SYS_WRITE = 4;
export constexpr usize SYS_EXIT = 1;
export constexpr usize SYS_MMAP = 90;

export [[nodiscard]]
auto syscall3(usize n, usize a1, usize a2, usize a3) -> isize
{
    register isize r1 __asm__("r1") = static_cast<isize>(n);
    register isize r2 __asm__("r2") = static_cast<isize>(a1);
    register isize r3 __asm__("r3") = static_cast<isize>(a2);
    register isize r4 __asm__("r4") = static_cast<isize>(a3);

    __asm__ volatile("svc 0" : "+r"(r2) : "r"(r1), "r"(r3), "r"(r4) : "memory", "cc");

    return r2;
}

export [[nodiscard]]
auto syscall6(usize n, usize a1, usize a2, usize a3, usize a4, usize a5, usize a6) -> isize
{
    register isize r1 __asm__("r1") = static_cast<isize>(n);
    register isize r2 __asm__("r2") = static_cast<isize>(a1);
    register isize r3 __asm__("r3") = static_cast<isize>(a2);
    register isize r4 __asm__("r4") = static_cast<isize>(a3);
    register isize r5 __asm__("r5") = static_cast<isize>(a4);
    register isize r6 __asm__("r6") = static_cast<isize>(a5);
    register isize r7 __asm__("r7") = static_cast<isize>(a6);

    __asm__ volatile("svc 0" : "+r"(r2) : "r"(r1), "r"(r3), "r"(r4), "r"(r5), "r"(r6), "r"(r7) : "memory", "cc");

    return r2;
}
}  // namespace ib::sys

#endif  // OS(LINUX) && ARCH(S390X)
