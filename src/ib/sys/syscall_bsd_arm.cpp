// SPDX-FileCopyrightText: 2026 iFarbod
//
// SPDX-License-Identifier: LicenseRef-Proprietary

// Shared BSD raw syscall primitive, ARMv7.

export module ib.sys:syscall_bsd_arm;

import ib.core;

#if (defined(IB_OS_FREEBSD) || defined(IB_OS_OPENBSD) || defined(IB_OS_NETBSD) || defined(IB_OS_DRAGONFLY)) && \
    defined(IB_ARCH_ARM)

namespace ib::sys
{
export [[nodiscard]]
auto syscall3(usize n, usize a1, usize a2, usize a3) -> isize
{
    register isize r7 __asm__("r7") = static_cast<isize>(n);
    register isize r0 __asm__("r0") = static_cast<isize>(a1);
    register isize r1 __asm__("r1") = static_cast<isize>(a2);
    register isize r2 __asm__("r2") = static_cast<isize>(a3);

    __asm__ volatile("swi #0" : "+r"(r0) : "r"(r7), "r"(r1), "r"(r2) : "memory", "cc");

    return r0;
}

export [[nodiscard]]
auto syscall6(usize n, usize a1, usize a2, usize a3, usize a4, usize a5, usize a6) -> isize
{
    register isize r7 __asm__("r7") = static_cast<isize>(n);
    register isize r0 __asm__("r0") = static_cast<isize>(a1);
    register isize r1 __asm__("r1") = static_cast<isize>(a2);
    register isize r2 __asm__("r2") = static_cast<isize>(a3);
    register isize r3 __asm__("r3") = static_cast<isize>(a4);
    register isize r4 __asm__("r4") = static_cast<isize>(a5);
    register isize r5 __asm__("r5") = static_cast<isize>(a6);

    __asm__ volatile("swi #0" : "+r"(r0) : "r"(r7), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) : "memory", "cc");

    return r0;
}

#if defined(IB_OS_NETBSD) || defined(IB_OS_OPENBSD)
export [[nodiscard]]
auto syscall7(usize n, usize a1, usize a2, usize a3, usize a4, usize a5, usize a6, usize a7) -> isize
{
    register isize r7 __asm__("r7") = static_cast<isize>(n);
    register isize r0 __asm__("r0") = static_cast<isize>(a1);
    register isize r1 __asm__("r1") = static_cast<isize>(a2);
    register isize r2 __asm__("r2") = static_cast<isize>(a3);
    register isize r3 __asm__("r3") = static_cast<isize>(a4);
    register isize r4 __asm__("r4") = static_cast<isize>(a5);
    register isize r5 __asm__("r5") = static_cast<isize>(a6);
    register isize r6 __asm__("r6") = static_cast<isize>(a7);

    __asm__ volatile("swi #0" : "+r"(r0) : "r"(r7), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5), "r"(r6) : "memory", "cc");

    return r0;
}
#endif  // IB_OS_NETBSD || IB_OS_OPENBSD
}  // namespace ib::sys

#endif  // any BSD && IB_ARCH_ARM
