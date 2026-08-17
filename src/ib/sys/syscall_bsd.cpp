// SPDX-FileCopyrightText: 2026 iFarbod
//
// SPDX-License-Identifier: LicenseRef-Proprietary

// Shared BSD base. FreeBSD, OpenBSD, NetBSD, and DragonFly share the same
// write/exit numbers and the same register/instruction conventions, so the raw
// syscallN primitives live in the shared syscall_bsd_<arch> partitions. Only
// the mmap number differs per OS — that lives in syscall_<os>.cpp.

export module ib.sys:syscall_bsd;

export import :syscall_bsd_amd64;
export import :syscall_bsd_i386;
export import :syscall_bsd_aarch64;
export import :syscall_bsd_arm;
export import :syscall_bsd_ppc64;
export import :syscall_bsd_riscv64;

#if OS(FREEBSD) || OS(OPENBSD) || OS(NETBSD) || OS(DRAGONFLY)

import ib.core;

namespace ib::sys
{
export constexpr usize SYS_WRITE = 4;
export constexpr usize SYS_EXIT = 1;
}  // namespace ib::sys

#endif  // any BSD
