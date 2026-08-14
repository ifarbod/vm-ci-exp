// SPDX-FileCopyrightText: 2026 iFarbod
//
// SPDX-License-Identifier: LicenseRef-Proprietary

// DragonFly BSD backend. write/exit numbers and the raw syscallN primitives come
// from the shared syscall_bsd base; only the mmap number is DragonFly-specific.
// DragonFly ships amd64 only.

export module ib.sys:syscall_dragonfly;

export import :syscall_bsd;

#ifdef IB_OS_DRAGONFLY

import ib.core;

namespace ib::sys
{
export constexpr usize SYS_MMAP = 197;
}  // namespace ib::sys

#endif  // IB_OS_DRAGONFLY
