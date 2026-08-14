// SPDX-FileCopyrightText: 2026 iFarbod
//
// SPDX-License-Identifier: LicenseRef-Proprietary

// Linux backend. The syscall numbers and the raw syscall3/6 primitives are
// per-architecture and live in the syscall_linux_<arch> partitions; this file
// just groups them.

export module ib.sys:syscall_linux;

export import :syscall_linux_amd64;
export import :syscall_linux_i386;
export import :syscall_linux_arm;
export import :syscall_linux_aarch64;
export import :syscall_linux_mips64;
export import :syscall_linux_ppc64;
export import :syscall_linux_riscv64;
export import :syscall_linux_s390x;
export import :syscall_linux_loongarch64;
