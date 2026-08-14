// SPDX-FileCopyrightText: 2026 iFarbod
//
// SPDX-License-Identifier: LicenseRef-Proprietary

// Linux/MIPS64 little-endian, n64 ABI: __NR_Linux=5000, write=5001, exit=5058,
// mmap=5009.

export module ib.sys:syscall_linux_mips64;

import ib.core;

#if defined(IB_OS_LINUX) && defined(IB_ARCH_MIPS64)

namespace ib::sys
{
export constexpr usize SYS_WRITE = 5001;
export constexpr usize SYS_EXIT = 5058;
export constexpr usize SYS_MMAP = 5009;

export [[nodiscard]]
auto syscall3(usize n, usize a1, usize a2, usize a3) -> isize
{
    register usize v0 __asm__("$2") = n;
    register usize a0 __asm__("$4") = a1;
    register usize a1r __asm__("$5") = a2;
    register usize a2r __asm__("$6") = a3;

    __asm__ volatile("syscall" : "+r"(v0) : "r"(a0), "r"(a1r), "r"(a2r) : "$1", "$3", "$7", "memory");

    return static_cast<isize>(v0);
}

export [[nodiscard]]
auto syscall6(usize n, usize a1, usize a2, usize a3, usize a4, usize a5, usize a6) -> isize
{
    register usize v0 __asm__("$2") = n;
    register usize a0 __asm__("$4") = a1;
    register usize a1r __asm__("$5") = a2;
    register usize a2r __asm__("$6") = a3;
    register usize a3r __asm__("$7") = a4;
    register usize a4r __asm__("$8") = a5;
    register usize a5r __asm__("$9") = a6;

    __asm__ volatile("syscall"
        : "+r"(v0)
        : "r"(a0), "r"(a1r), "r"(a2r), "r"(a3r), "r"(a4r), "r"(a5r)
        : "$1", "$3", "$10", "$11", "memory");

    return static_cast<isize>(v0);
}
}  // namespace ib::sys

#endif  // IB_OS_LINUX && IB_ARCH_MIPS64
