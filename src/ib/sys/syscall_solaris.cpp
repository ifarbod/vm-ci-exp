// SPDX-FileCopyrightText: 2026 iFarbod
//
// SPDX-License-Identifier: LicenseRef-Proprietary

// Solaris / Illumos backend. Syscall numbers from <sys/syscall.h>. The raw
// syscall3/6 primitives are per-architecture and live in the
// syscall_solaris_<arch> partitions.

export module ib.sys:syscall_solaris;

export import :syscall_solaris_amd64;
export import :syscall_solaris_sparc64;

#if OS(SOLARIS)

import ib.core;

namespace ib::sys
{
export constexpr usize SYS_WRITE = 4;
export constexpr usize SYS_EXIT = 1;
export constexpr usize SYS_MMAP = 115;
}  // namespace ib::sys

#endif  // OS(SOLARIS)
