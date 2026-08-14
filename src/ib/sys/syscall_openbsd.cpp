// SPDX-FileCopyrightText: 2026 iFarbod
//
// SPDX-License-Identifier: LicenseRef-Proprietary

// OpenBSD backend. write/exit numbers and the raw syscallN primitives come from
// the shared syscall_bsd base (which also provides syscall7 here, since OpenBSD's
// mmap takes a padding argument); only the mmap number is OpenBSD-specific.

export module ib.sys:syscall_openbsd;

export import :syscall_bsd;

#ifdef IB_OS_OPENBSD

import ib.core;

namespace ib::sys
{
export constexpr usize SYS_MMAP = 49;
}  // namespace ib::sys

#endif  // IB_OS_OPENBSD
