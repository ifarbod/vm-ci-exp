// SPDX-FileCopyrightText: 2026 iFarbod
//
// SPDX-License-Identifier: LicenseRef-Proprietary

// macOS (Darwin) backend. Syscalls live in the BSD class (class 2):
// 0x2000000 + number. The raw syscall3/6/7 primitives are per-architecture and
// live in the syscall_darwin_<arch> partitions.

export module ib.sys:syscall_darwin;

export import :syscall_darwin_amd64;
export import :syscall_darwin_aarch64;

#ifdef IB_OS_MACOS

import ib.core;

namespace ib::sys
{
export constexpr usize SYS_WRITE = 0x2000004;
export constexpr usize SYS_EXIT = 0x2000001;
export constexpr usize SYS_MMAP = 0x20000c5;
}  // namespace ib::sys

#endif  // IB_OS_MACOS
