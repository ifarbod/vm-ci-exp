// SPDX-FileCopyrightText: 2026 iFarbod
//
// SPDX-License-Identifier: LicenseRef-Proprietary

// Solaris/amd64 raw syscall primitive (SysV syscall ABI).

export module ib.sys:syscall_solaris_amd64;

import ib.core;

#if OS(SOLARIS) && ARCH(AMD64)

namespace ib::sys
{
export [[nodiscard]]
auto syscall3(usize n, usize a1, usize a2, usize a3) -> isize
{
    register isize rax __asm__("rax") = static_cast<isize>(n);
    register isize rdi __asm__("rdi") = static_cast<isize>(a1);
    register isize rsi __asm__("rsi") = static_cast<isize>(a2);
    register isize rdx __asm__("rdx") = static_cast<isize>(a3);

    __asm__ volatile("syscall" : "+r"(rax) : "r"(rdi), "r"(rsi), "r"(rdx) : "rcx", "r11", "memory");

    return rax;
}

export [[nodiscard]]
auto syscall6(usize n, usize a1, usize a2, usize a3, usize a4, usize a5, usize a6) -> isize
{
    register isize rax __asm__("rax") = static_cast<isize>(n);
    register isize rdi __asm__("rdi") = static_cast<isize>(a1);
    register isize rsi __asm__("rsi") = static_cast<isize>(a2);
    register isize rdx __asm__("rdx") = static_cast<isize>(a3);
    register isize r10 __asm__("r10") = static_cast<isize>(a4);
    register isize r8 __asm__("r8") = static_cast<isize>(a5);
    register isize r9 __asm__("r9") = static_cast<isize>(a6);

    __asm__ volatile("syscall"
        : "+r"(rax)
        : "r"(rdi), "r"(rsi), "r"(rdx), "r"(r10), "r"(r8), "r"(r9)
        : "rcx", "r11", "memory");

    return rax;
}
}  // namespace ib::sys

#endif  // OS(SOLARIS) && ARCH(AMD64)
