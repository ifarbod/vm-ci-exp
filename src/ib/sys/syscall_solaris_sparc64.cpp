// SPDX-FileCopyrightText: 2026 iFarbod
//
// SPDX-License-Identifier: LicenseRef-Proprietary

// Solaris/SPARC64 (sparcv9) raw syscall primitive: the number goes in g1, args
// in o0-o5, trap is `ta 0x40`. macros.hpp has no SPARC arch macro yet, so raw
// __sparc* checks are used.

export module ib.sys:syscall_solaris_sparc64;

import ib.core;

#if OS(SOLARIS) && (defined(__sparcv9) || defined(__sparc64__) || (defined(__sparc__) && defined(__arch64__)))

namespace ib::sys
{
export [[nodiscard]]
auto syscall3(usize n, usize a1, usize a2, usize a3) -> isize
{
    register isize o0 __asm__("o0") = static_cast<isize>(a1);
    register isize o1 __asm__("o1") = static_cast<isize>(a2);
    register isize o2 __asm__("o2") = static_cast<isize>(a3);
    register isize g1 __asm__("g1") = static_cast<isize>(n);

    __asm__ volatile("ta 0x40" : "+r"(o0) : "r"(o1), "r"(o2), "r"(g1) : "cc", "memory");

    return o0;
}

export [[nodiscard]]
auto syscall6(usize n, usize a1, usize a2, usize a3, usize a4, usize a5, usize a6) -> isize
{
    register isize o0 __asm__("o0") = static_cast<isize>(a1);
    register isize o1 __asm__("o1") = static_cast<isize>(a2);
    register isize o2 __asm__("o2") = static_cast<isize>(a3);
    register isize o3 __asm__("o3") = static_cast<isize>(a4);
    register isize o4 __asm__("o4") = static_cast<isize>(a5);
    register isize o5 __asm__("o5") = static_cast<isize>(a6);
    register isize g1 __asm__("g1") = static_cast<isize>(n);

    __asm__ volatile("ta 0x40" : "+r"(o0) : "r"(o1), "r"(o2), "r"(o3), "r"(o4), "r"(o5), "r"(g1) : "cc", "memory");

    return o0;
}
}  // namespace ib::sys

#endif  // OS(SOLARIS) && sparc64
