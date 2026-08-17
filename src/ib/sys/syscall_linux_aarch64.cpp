// SPDX-FileCopyrightText: 2026 iFarbod
//
// SPDX-License-Identifier: LicenseRef-Proprietary

// Linux/aarch64: write=64, exit=93, mmap=222.

export module ib.sys:syscall_linux_aarch64;

import ib.core;

#if OS(LINUX) && ARCH(AARCH64)

namespace ib::sys
{
export constexpr usize SYS_WRITE = 64;
export constexpr usize SYS_EXIT = 93;
export constexpr usize SYS_MMAP = 222;

export [[nodiscard]]
auto syscall3(usize n, usize a1, usize a2, usize a3) -> isize
{
    register isize x0 __asm__("x0") = static_cast<isize>(a1);
    register isize x1 __asm__("x1") = static_cast<isize>(a2);
    register isize x2 __asm__("x2") = static_cast<isize>(a3);
    register isize x8 __asm__("x8") = static_cast<isize>(n);

    __asm__ volatile("svc #0" : "+r"(x0) : "r"(x1), "r"(x2), "r"(x8) : "memory");

    return x0;
}

export [[nodiscard]]
auto syscall6(usize n, usize a1, usize a2, usize a3, usize a4, usize a5, usize a6) -> isize
{
    register isize x0 __asm__("x0") = static_cast<isize>(a1);
    register isize x1 __asm__("x1") = static_cast<isize>(a2);
    register isize x2 __asm__("x2") = static_cast<isize>(a3);
    register isize x3 __asm__("x3") = static_cast<isize>(a4);
    register isize x4 __asm__("x4") = static_cast<isize>(a5);
    register isize x5 __asm__("x5") = static_cast<isize>(a6);
    register isize x8 __asm__("x8") = static_cast<isize>(n);

    __asm__ volatile("svc #0" : "+r"(x0) : "r"(x1), "r"(x2), "r"(x3), "r"(x4), "r"(x5), "r"(x8) : "memory");

    return x0;
}
}  // namespace ib::sys

#endif  // OS(LINUX) && ARCH(AARCH64)
