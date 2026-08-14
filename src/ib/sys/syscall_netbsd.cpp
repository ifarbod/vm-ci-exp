// SPDX-FileCopyrightText: 2026 iFarbod
//
// SPDX-License-Identifier: LicenseRef-Proprietary

// NetBSD backend. write/exit numbers and the raw syscallN primitives come from
// the shared syscall_bsd base (which also provides syscall7 here, since NetBSD's
// mmap takes a padding argument); only the mmap number is NetBSD-specific.

export module ib.sys:syscall_netbsd;

export import :syscall_bsd;

#ifdef IB_OS_NETBSD

import ib.core;

namespace ib::sys
{
export constexpr usize SYS_MMAP = 197;
}  // namespace ib::sys

#endif  // IB_OS_NETBSD
