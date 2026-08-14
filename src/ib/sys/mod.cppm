// SPDX-FileCopyrightText: 2026 iFarbod
//
// SPDX-License-Identifier: LicenseRef-Proprietary

export module ib.sys;

// Shared helpers and the POSIX wrapper layer.
export import :detail;
export import :posix;

// Per-OS backends. Each partition is compiled for every target but only exports
// symbols on its matching OS/arch, so importing all of them is unambiguous and
// exactly one backend is active per build.
export import :syscall_windows;
export import :syscall_linux;
export import :syscall_bsd;
export import :syscall_freebsd;
export import :syscall_openbsd;
export import :syscall_netbsd;
export import :syscall_dragonfly;
export import :syscall_darwin;
export import :syscall_solaris;
