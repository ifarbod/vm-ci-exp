// SPDX-FileCopyrightText: 2026 iFarbod
//
// SPDX-License-Identifier: LicenseRef-Proprietary

// Shared BSD raw syscall primitive, amd64. syscall7 is only needed by the
// NetBSD/OpenBSD mmap (which take a padding argument).

export module ib.sys:syscall_bsd_amd64;

import ib.core;

#if (defined(IB_OS_FREEBSD) || defined(IB_OS_OPENBSD) || defined(IB_OS_NETBSD) || defined(IB_OS_DRAGONFLY)) && \
    defined(IB_ARCH_AMD64)

namespace ib::sys
{
export [[nodiscard]]
auto syscall3(usize n, usize a1, usize a2, usize a3) -> isize
{
    isize ret = 0;
    register isize r10 __asm__("r10") = 0;
    register isize r8 __asm__("r8") = 0;
    register isize r9 __asm__("r9") = 0;
    __asm__ volatile("syscall"
        : "=a"(ret)
        : "0"(static_cast<isize>(n)), "D"(static_cast<isize>(a1)), "S"(static_cast<isize>(a2)),
        "d"(static_cast<isize>(a3)), "r"(r10), "r"(r8), "r"(r9)
        : "rcx", "r11", "memory");
    return ret;
}

export [[nodiscard]]
auto syscall6(usize n, usize a1, usize a2, usize a3, usize a4, usize a5, usize a6) -> isize
{
    isize ret = 0;
    register isize r10 __asm__("r10") = static_cast<isize>(a4);
    register isize r8 __asm__("r8") = static_cast<isize>(a5);
    register isize r9 __asm__("r9") = static_cast<isize>(a6);
    __asm__ volatile("syscall"
        : "=a"(ret)
        : "0"(static_cast<isize>(n)), "D"(static_cast<isize>(a1)), "S"(static_cast<isize>(a2)),
        "d"(static_cast<isize>(a3)), "r"(r10), "r"(r8), "r"(r9)
        : "rcx", "r11", "memory");
    return ret;
}

#if defined(IB_OS_NETBSD) || defined(IB_OS_OPENBSD)
export [[nodiscard]]
auto syscall7(usize n, usize a1, usize a2, usize a3, usize a4, usize a5, usize a6, usize a7) -> isize
{
    isize ret = 0;
    register isize r10 __asm__("r10") = static_cast<isize>(a4);
    register isize r8 __asm__("r8") = static_cast<isize>(a5);
    register isize r9 __asm__("r9") = static_cast<isize>(a6);
    __asm__ volatile("subq $16, %%rsp\n\t"
                     "movq %8, 8(%%rsp)\n\t"
                     "syscall\n\t"
                     "addq $16, %%rsp"
        : "=a"(ret)
        : "0"(static_cast<isize>(n)), "D"(static_cast<isize>(a1)), "S"(static_cast<isize>(a2)),
        "d"(static_cast<isize>(a3)), "r"(r10), "r"(r8), "r"(r9), "r"(static_cast<isize>(a7))
        : "rcx", "r11", "memory");
    return ret;
}
#endif  // IB_OS_NETBSD || IB_OS_OPENBSD
}  // namespace ib::sys

#endif  // any BSD && IB_ARCH_AMD64
