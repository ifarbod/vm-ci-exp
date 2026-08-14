// SPDX-FileCopyrightText: 2026 iFarbod
//
// SPDX-License-Identifier: LicenseRef-Proprietary

// Shared BSD raw syscall primitive, aarch64.

export module ib.sys:syscall_bsd_aarch64;

import ib.core;

#if (defined(IB_OS_FREEBSD) || defined(IB_OS_OPENBSD) || defined(IB_OS_NETBSD) || defined(IB_OS_DRAGONFLY)) && \
    defined(IB_ARCH_AARCH64)

namespace ib::sys
{
export [[nodiscard]]
auto syscall3(usize n, usize a1, usize a2, usize a3) -> isize
{
    register isize x8 __asm__("x8") = static_cast<isize>(n);
    register isize x0 __asm__("x0") = static_cast<isize>(a1);
    register isize x1 __asm__("x1") = static_cast<isize>(a2);
    register isize x2 __asm__("x2") = static_cast<isize>(a3);

    __asm__ volatile("svc #0" : "+r"(x0) : "r"(x8), "r"(x1), "r"(x2) : "memory", "cc");

    return x0;
}

export [[nodiscard]]
auto syscall6(usize n, usize a1, usize a2, usize a3, usize a4, usize a5, usize a6) -> isize
{
    register isize x8 __asm__("x8") = static_cast<isize>(n);
    register isize x0 __asm__("x0") = static_cast<isize>(a1);
    register isize x1 __asm__("x1") = static_cast<isize>(a2);
    register isize x2 __asm__("x2") = static_cast<isize>(a3);
    register isize x3 __asm__("x3") = static_cast<isize>(a4);
    register isize x4 __asm__("x4") = static_cast<isize>(a5);
    register isize x5 __asm__("x5") = static_cast<isize>(a6);

    __asm__ volatile("svc #0" : "+r"(x0) : "r"(x8), "r"(x1), "r"(x2), "r"(x3), "r"(x4), "r"(x5) : "memory", "cc");

    return x0;
}

#if defined(IB_OS_NETBSD) || defined(IB_OS_OPENBSD)
export [[nodiscard]]
auto syscall7(usize n, usize a1, usize a2, usize a3, usize a4, usize a5, usize a6, usize a7) -> isize
{
    register isize x8 __asm__("x8") = static_cast<isize>(n);
    register isize x0 __asm__("x0") = static_cast<isize>(a1);
    register isize x1 __asm__("x1") = static_cast<isize>(a2);
    register isize x2 __asm__("x2") = static_cast<isize>(a3);
    register isize x3 __asm__("x3") = static_cast<isize>(a4);
    register isize x4 __asm__("x4") = static_cast<isize>(a5);
    register isize x5 __asm__("x5") = static_cast<isize>(a6);
    register isize x6 __asm__("x6") = static_cast<isize>(a7);

    __asm__ volatile("svc #0" : "+r"(x0) : "r"(x8), "r"(x1), "r"(x2), "r"(x3), "r"(x4), "r"(x5), "r"(x6) : "memory", "cc");

    return x0;
}
#endif  // IB_OS_NETBSD || IB_OS_OPENBSD
}  // namespace ib::sys

#endif  // any BSD && IB_ARCH_AARCH64
