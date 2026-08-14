// SPDX-FileCopyrightText: 2026 iFarbod
//
// SPDX-License-Identifier: LicenseRef-Proprietary

// Linux/i386: write=4, exit=1, mmap2=192 (offset in 4096-byte units).

export module ib.sys:syscall_linux_i386;

import ib.core;

#if defined(IB_OS_LINUX) && defined(IB_ARCH_I386)

namespace ib::sys
{
export constexpr usize SYS_WRITE = 4;
export constexpr usize SYS_EXIT = 1;
export constexpr usize SYS_MMAP2 = 192;

export [[nodiscard]]
auto syscall3(usize n, usize a1, usize a2, usize a3) -> isize
{
    register isize eax __asm__("eax") = static_cast<isize>(n);
    register isize ebx __asm__("ebx") = static_cast<isize>(a1);
    register isize ecx __asm__("ecx") = static_cast<isize>(a2);
    register isize edx __asm__("edx") = static_cast<isize>(a3);

    __asm__ volatile("int $0x80" : "+r"(eax) : "r"(ebx), "r"(ecx), "r"(edx) : "memory", "cc");

    return eax;
}

export [[nodiscard]]
auto syscall6(usize n, usize a1, usize a2, usize a3, usize a4, usize a5, usize a6) -> isize
{
    register isize eax __asm__("eax") = static_cast<isize>(n);
    register isize ebx __asm__("ebx") = static_cast<isize>(a1);
    register isize ecx __asm__("ecx") = static_cast<isize>(a2);
    register isize edx __asm__("edx") = static_cast<isize>(a3);
    register isize esi __asm__("esi") = static_cast<isize>(a4);
    register isize edi __asm__("edi") = static_cast<isize>(a5);
    register isize ebp __asm__("ebp") = static_cast<isize>(a6);

    __asm__ volatile("int $0x80"
        : "+r"(eax)
        : "r"(ebx), "r"(ecx), "r"(edx), "r"(esi), "r"(edi), "r"(ebp)
        : "memory", "cc");

    return eax;
}
}  // namespace ib::sys

#endif  // IB_OS_LINUX && IB_ARCH_I386
