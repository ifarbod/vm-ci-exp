// SPDX-FileCopyrightText: 2026 iFarbod
//
// SPDX-License-Identifier: LicenseRef-Proprietary

// Shared BSD raw syscall primitive, RISC-V 64.

export module ib.sys:syscall_bsd_riscv64;

import ib.core;

#if (defined(IB_OS_FREEBSD) || defined(IB_OS_OPENBSD) || defined(IB_OS_NETBSD) || defined(IB_OS_DRAGONFLY)) && \
    defined(IB_ARCH_RISCV64)

namespace ib::sys
{
export [[nodiscard]]
auto syscall3(usize n, usize a1, usize a2, usize a3) -> isize
{
    register isize a7 __asm__("a7") = static_cast<isize>(n);
    register isize a0 __asm__("a0") = static_cast<isize>(a1);
    register isize a1r __asm__("a1") = static_cast<isize>(a2);
    register isize a2r __asm__("a2") = static_cast<isize>(a3);

    __asm__ volatile("ecall" : "+r"(a0) : "r"(a7), "r"(a1r), "r"(a2r) : "memory");

    return a0;
}

export [[nodiscard]]
auto syscall6(usize n, usize a1, usize a2, usize a3, usize a4, usize a5, usize a6) -> isize
{
    register isize a7 __asm__("a7") = static_cast<isize>(n);
    register isize a0 __asm__("a0") = static_cast<isize>(a1);
    register isize a1r __asm__("a1") = static_cast<isize>(a2);
    register isize a2r __asm__("a2") = static_cast<isize>(a3);
    register isize a3r __asm__("a3") = static_cast<isize>(a4);
    register isize a4r __asm__("a4") = static_cast<isize>(a5);
    register isize a5r __asm__("a5") = static_cast<isize>(a6);

    __asm__ volatile("ecall" : "+r"(a0) : "r"(a7), "r"(a1r), "r"(a2r), "r"(a3r), "r"(a4r), "r"(a5r) : "memory");

    return a0;
}

#if defined(IB_OS_NETBSD) || defined(IB_OS_OPENBSD)
export [[nodiscard]]
auto syscall7(usize n, usize a1, usize a2, usize a3, usize a4, usize a5, usize a6, usize a7) -> isize
{
    register isize a7n __asm__("a7") = static_cast<isize>(n);
    register isize a0 __asm__("a0") = static_cast<isize>(a1);
    register isize a1r __asm__("a1") = static_cast<isize>(a2);
    register isize a2r __asm__("a2") = static_cast<isize>(a3);
    register isize a3r __asm__("a3") = static_cast<isize>(a4);
    register isize a4r __asm__("a4") = static_cast<isize>(a5);
    register isize a5r __asm__("a5") = static_cast<isize>(a6);
    register isize a6r __asm__("a6") = static_cast<isize>(a7);

    __asm__ volatile("ecall" : "+r"(a0) : "r"(a7n), "r"(a1r), "r"(a2r), "r"(a3r), "r"(a4r), "r"(a5r), "r"(a6r) : "memory");

    return a0;
}
#endif  // IB_OS_NETBSD || IB_OS_OPENBSD
}  // namespace ib::sys

#endif  // any BSD && IB_ARCH_RISCV64
