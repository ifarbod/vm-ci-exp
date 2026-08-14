// SPDX-FileCopyrightText: 2026 iFarbod
//
// SPDX-License-Identifier: LicenseRef-Proprietary

// Shared POSIX-like layer: Linux, FreeBSD, OpenBSD, NetBSD, DragonFly, macOS,
// Solaris. Provides the mmap/write/print/exit wrappers on top of the raw
// syscallN primitives and SYS_* numbers supplied by the active OS partition.
//
// This file is compiled for every target; on non-POSIX platforms (Windows) the
// platform guard below leaves the partition empty.

export module ib.sys:posix;

import ib.core;
import :detail;

// The raw syscall3/6/7 primitives and SYS_* numbers live in the per-OS and
// per-arch partitions. Only the partition matching the target exports any
// symbols, so importing all of them is unambiguous.
import :syscall_linux;
import :syscall_bsd;
import :syscall_freebsd;
import :syscall_openbsd;
import :syscall_netbsd;
import :syscall_dragonfly;
import :syscall_darwin;
import :syscall_solaris;

#if defined(IB_OS_LINUX) || defined(IB_OS_FREEBSD) || defined(IB_OS_OPENBSD) || defined(IB_OS_NETBSD) || \
    defined(IB_OS_DRAGONFLY) || defined(IB_OS_MACOS) || defined(IB_OS_SOLARIS)

namespace ib::sys
{
// -------------------------------------------------------------------------
// Memory mapping
// -------------------------------------------------------------------------

/// Sentinel returned by mmap() on failure (the POSIX MAP_FAILED value).
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
export inline void* const MAP_FAILED = reinterpret_cast<void*>(static_cast<usize>(-1));

/// Maps files or devices into memory.
/// \return the mapped address, or MAP_FAILED on error.
export [[nodiscard]]
auto mmap(void* addr, usize length, i32 prot, i32 flags, i32 fd, isize offset) -> void*
{
    isize result = 0;
#if defined(IB_OS_LINUX) && (defined(IB_ARCH_I386) || defined(IB_ARCH_ARM))
    // 32-bit Linux exposes mmap2, whose offset is measured in 4096-byte units.
    constexpr isize page_size = 4096;
    if ((offset & (page_size - 1)) != 0)
    {
        return MAP_FAILED;
    }
    result = syscall6(SYS_MMAP2, reinterpret_cast<usize>(addr), length, static_cast<usize>(prot),
        static_cast<usize>(flags), static_cast<usize>(fd), static_cast<usize>(offset / page_size));
#elif defined(IB_OS_NETBSD) || defined(IB_OS_OPENBSD) || defined(IB_OS_MACOS)
    // NetBSD's, OpenBSD's, and macOS's mmap take a padding argument between fd
    // and offset.
    result = syscall7(SYS_MMAP, reinterpret_cast<usize>(addr), length, static_cast<usize>(prot),
        static_cast<usize>(flags), static_cast<usize>(fd), 0, static_cast<usize>(offset));
#else
    result = syscall6(SYS_MMAP, reinterpret_cast<usize>(addr), length, static_cast<usize>(prot),
        static_cast<usize>(flags), static_cast<usize>(fd), static_cast<usize>(offset));
#endif

    // On failure the kernel returns a negative errno (a value in (-4096, 0]).
    if (result < 0 && result > -4096)
    {
        return MAP_FAILED;
    }
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    return reinterpret_cast<void*>(result);
}

// -------------------------------------------------------------------------
// Standard file descriptors
// -------------------------------------------------------------------------
export constexpr isize STDIN = 0;
export constexpr isize STDOUT = 1;
export constexpr isize STDERR = 2;

// -------------------------------------------------------------------------
// I/O
// -------------------------------------------------------------------------

/// Writes up to `size` bytes to the given file descriptor.
/// Returns the number of bytes written, or a negative error code on failure.
export [[nodiscard]]
auto write(isize fd, const void* data, usize size) -> isize
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    return syscall3(SYS_WRITE, static_cast<usize>(fd), reinterpret_cast<usize>(data), size);
}

/// Writes a NUL-terminated string to the given file descriptor.
/// Returns the number of bytes written, or a negative error code on failure.
export [[nodiscard]]
auto write_string(isize fd, const char* string) -> isize
{
    return write(fd, string, detail::str_length(string));
}

/// Writes raw bytes to the standard output stream.
/// Returns the number of bytes written, or a negative error code on failure.
export [[nodiscard]]
auto print(const void* data, usize size) -> isize
{
    return write(STDOUT, data, size);
}

/// Writes a NUL-terminated string to the standard output stream.
/// Returns the number of bytes written, or a negative error code on failure.
export [[nodiscard]]
auto print(const char* string) -> isize
{
    return write_string(STDOUT, string);
}

// -------------------------------------------------------------------------
// Process
// -------------------------------------------------------------------------

/// Terminates the process with the given exit status.
export [[noreturn]]
void exit(isize status)
{
    static_cast<void>(syscall3(SYS_EXIT, static_cast<usize>(status), 0, 0));
    __builtin_unreachable();
}
}  // namespace ib::sys

#endif  // POSIX-like platforms
