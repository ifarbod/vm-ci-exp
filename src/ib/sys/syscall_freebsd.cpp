// SPDX-FileCopyrightText: 2026 iFarbod
//
// SPDX-License-Identifier: LicenseRef-Proprietary

// FreeBSD backend. write/exit numbers and the raw syscallN primitives come from
// the shared syscall_bsd base; only the mmap number is FreeBSD-specific.

export module ib.sys:syscall_freebsd;

export import :syscall_bsd;

#ifdef IB_OS_FREEBSD

import ib.core;

namespace ib::sys
{
export constexpr usize SYS_MMAP = 477;
}  // namespace ib::sys

#endif  // IB_OS_FREEBSD
