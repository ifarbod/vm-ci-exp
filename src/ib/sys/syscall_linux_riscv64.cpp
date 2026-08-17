// SPDX-FileCopyrightText: 2026 iFarbod
//
// SPDX-License-Identifier: LicenseRef-Proprietary

// Linux/riscv64: write=64, exit=93, mmap=222.

export module ib.sys:syscall_linux_riscv64;

import ib.core;

#if OS(LINUX) && ARCH(RISCV64)

namespace ib::sys
{
export constexpr usize SYS_WRITE = 64;
export constexpr usize SYS_EXIT = 93;
export constexpr usize SYS_MMAP = 222;

export [[nodiscard]]
auto syscall3(usize n, usize a1, usize a2, usize a3) -> isize
{
    register isize a0 __asm__("a0") = static_cast<isize>(a1);
    register isize a1r __asm__("a1") = static_cast<isize>(a2);
    register isize a2r __asm__("a2") = static_cast<isize>(a3);
    register isize a7 __asm__("a7") = static_cast<isize>(n);

    __asm__ volatile("ecall" : "+r"(a0) : "r"(a1r), "r"(a2r), "r"(a7) : "memory");

    return a0;
}

export [[nodiscard]]
auto syscall6(usize n, usize a1, usize a2, usize a3, usize a4, usize a5, usize a6) -> isize
{
    register isize a0 __asm__("a0") = static_cast<isize>(a1);
    register isize a1r __asm__("a1") = static_cast<isize>(a2);
    register isize a2r __asm__("a2") = static_cast<isize>(a3);
    register isize a3r __asm__("a3") = static_cast<isize>(a4);
    register isize a4r __asm__("a4") = static_cast<isize>(a5);
    register isize a5r __asm__("a5") = static_cast<isize>(a6);
    register isize a7 __asm__("a7") = static_cast<isize>(n);

    __asm__ volatile("ecall" : "+r"(a0) : "r"(a1r), "r"(a2r), "r"(a3r), "r"(a4r), "r"(a5r), "r"(a7) : "memory");

    return a0;
}
}  // namespace ib::sys

#endif  // OS(LINUX) && ARCH(RISCV64)
