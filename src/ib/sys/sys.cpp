// SPDX-FileCopyrightText: 2026 iFarbod
//
// SPDX-License-Identifier: LicenseRef-Proprietary

export module ib.sys:syscalls;

import ib.core;

// -----------------------------------------------------------------------------
// Internal helpers (not exported)
// -----------------------------------------------------------------------------

/// Returns the length of a NUL-terminated string, not counting the terminator.
///
/// clang::no_builtin keeps the optimizer from turning the loop into a `strlen`
/// libcall (there is no libc here) when the call is not constant-folded.
[[nodiscard, clang::no_builtin("strlen")]]
constexpr auto str_length(const char* string) noexcept -> usize
{
    usize length = 0;
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    while (string[length] != '\0')
    {
        ++length;
    }
    return length;
}

// =============================================================================
// Windows
// =============================================================================
#ifdef IB_OS_WINDOWS

namespace ib::sys
{
/// Windows API handle (HANDLE), as returned by get_std_handle().
export using Handle = void*;

/// Standard device handles, as passed to get_std_handle().
export enum class StdHandle : u8 {
    Input,   // STD_INPUT_HANDLE
    Output,  // STD_OUTPUT_HANDLE
    Error,   // STD_ERROR_HANDLE
};

static_assert(sizeof(wchar_t) == 2, "Windows wchar_t must be UTF-16.");

#ifdef IB_ARCH_I386
#define IB_WINAPI __stdcall
#else
#define IB_WINAPI
#endif

// Direct imports from kernel32.dll — no CRT involved. The Win32 ABI types
// (DWORD is `unsigned long`, BOOL is `int`) are ABI-identical to u32/i32 on
// both Win32 and Win64, so the aliases below are interchangeable.
// NOLINTBEGIN(readability-identifier-naming)
extern "C"
{
    __declspec(dllimport) auto IB_WINAPI GetStdHandle(u32 n_std_handle) -> Handle;
    __declspec(dllimport) auto IB_WINAPI GetConsoleMode(Handle h_console_handle, u32* lp_mode) -> i32;
    __declspec(dllimport) auto IB_WINAPI WriteConsoleW(Handle h_console_output, const void* lp_buffer,
        u32 n_number_of_chars_to_write, u32* lp_number_of_chars_written, void* lp_reserved) -> i32;
    __declspec(dllimport) auto IB_WINAPI WriteFile(Handle h_file, const void* lp_buffer, u32 n_number_of_bytes_to_write,
        u32* lp_number_of_bytes_written, void* lp_overlapped) -> i32;
    __declspec(dllimport) __declspec(noreturn) void IB_WINAPI ExitProcess(u32 u_exit_code);
}
// NOLINTEND(readability-identifier-naming)

/// Returns the standard input, output, or error handle.
/// Returns INVALID_HANDLE_VALUE (-1) when the handle cannot be obtained.
export [[nodiscard]]
auto get_std_handle(StdHandle which) noexcept -> Handle
{
    constexpr u32 std_input_handle = static_cast<u32>(-10);
    constexpr u32 std_output_handle = static_cast<u32>(-11);
    constexpr u32 std_error_handle = static_cast<u32>(-12);

    u32 n_std_handle = std_output_handle;
    switch (which)
    {
        case StdHandle::Input:
            n_std_handle = std_input_handle;
            break;
        case StdHandle::Error:
            n_std_handle = std_error_handle;
            break;
        case StdHandle::Output:
            break;
    }
    return GetStdHandle(n_std_handle);
}

/// Returns whether the given handle refers to a console.
export [[nodiscard]]
auto get_console_mode(Handle console, u32* mode) noexcept -> bool
{
    return GetConsoleMode(console, mode) != 0;
}

/// Writes UTF-16 text to a console output handle.
export [[nodiscard]]
auto write_console_w(Handle output, const wchar_t* text, u32 chars, u32* written) noexcept -> bool
{
    return WriteConsoleW(output, text, chars, written, nullptr) != 0;
}

/// Writes raw bytes to any file-like handle (console or redirected).
export [[nodiscard]]
auto write_file(Handle file, const void* data, u32 bytes, u32* written) noexcept -> bool
{
    return WriteFile(file, data, bytes, written, nullptr) != 0;
}

/// Writes raw bytes to the standard output stream.
/// \return the number of bytes written, or a negative value on failure.
export [[nodiscard]]
auto print(const void* data, usize size) noexcept -> isize
{
    // GetStdHandle() returns INVALID_HANDLE_VALUE (-1) on failure.
    Handle output = get_std_handle(StdHandle::Output);
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast, performance-no-int-to-ptr)
    if (output == reinterpret_cast<Handle>(-1))
    {
        return -1;
    }

    u32 written = 0;
    if (!write_file(output, data, static_cast<u32>(size), &written))
    {
        return -1;
    }
    return static_cast<isize>(written);
}

/// Writes a NUL-terminated string to the standard output stream.
/// Returns the number of bytes written, or a negative value on failure.
export [[nodiscard]]
auto print(const char* string) noexcept -> isize
{
    return print(string, str_length(string));
}

/// Terminates the process with the given exit code.
export [[noreturn]]
void exit(isize status) noexcept
{
    ExitProcess(static_cast<u32>(status));

#if defined(IB_COMPILER_CLANG) || defined(IB_COMPILER_GCC)
    __builtin_unreachable();
#elifdef IB_COMPILER_MSVC
    __assume(false);
#endif
}
}  // namespace ib::sys

// =============================================================================
// POSIX-like: Linux, FreeBSD, OpenBSD, NetBSD, DragonFly, macOS, Solaris
// =============================================================================
#elif defined(IB_OS_LINUX) || defined(IB_OS_FREEBSD) || defined(IB_OS_OPENBSD) || defined(IB_OS_NETBSD) || \
    defined(IB_OS_DRAGONFLY) || defined(IB_OS_MACOS) || defined(IB_OS_SOLARIS)

namespace ib::sys
{
// -------------------------------------------------------------------------
// Raw syscall primitive (per platform and architecture)
// -------------------------------------------------------------------------

#if defined(IB_OS_LINUX)

// Linux syscall numbers (see <asm/unistd.h>).
#if defined(IB_ARCH_AMD64)
export constexpr usize SYS_WRITE = 1;
export constexpr usize SYS_EXIT = 60;
#elif defined(IB_ARCH_I386) || defined(IB_ARCH_ARM) || defined(IB_ARCH_PPC64) || defined(IB_ARCH_S390X)
export constexpr usize SYS_WRITE = 4;
export constexpr usize SYS_EXIT = 1;
#elif defined(IB_ARCH_AARCH64) || defined(IB_ARCH_RISCV64) || defined(IB_ARCH_LOONGARCH64)
export constexpr usize SYS_WRITE = 64;
export constexpr usize SYS_EXIT = 93;
#elif defined(IB_ARCH_MIPS64)
// MIPS64 n64 ABI: __NR_Linux base is 5000.
export constexpr usize SYS_WRITE = 5001;
export constexpr usize SYS_EXIT = 5058;
#else
#error "ib.sys: unsupported Linux architecture"
#endif

// amd64: write=1, exit=60
#if defined(IB_ARCH_AMD64)
export [[nodiscard]]
auto syscall3(usize n, usize a1, usize a2, usize a3) noexcept -> isize
{
    register isize rax __asm__("rax") = static_cast<isize>(n);
    register isize rdi __asm__("rdi") = static_cast<isize>(a1);
    register isize rsi __asm__("rsi") = static_cast<isize>(a2);
    register isize rdx __asm__("rdx") = static_cast<isize>(a3);

    __asm__ volatile("syscall" : "+r"(rax) : "r"(rdi), "r"(rsi), "r"(rdx) : "rcx", "r11", "memory", "cc");

    return rax;
}

// i386: write=4, exit=1
#elif defined(IB_ARCH_I386)
export [[nodiscard]]
auto syscall3(usize n, usize a1, usize a2, usize a3) noexcept -> isize
{
    register isize eax __asm__("eax") = static_cast<isize>(n);
    register isize ebx __asm__("ebx") = static_cast<isize>(a1);
    register isize ecx __asm__("ecx") = static_cast<isize>(a2);
    register isize edx __asm__("edx") = static_cast<isize>(a3);

    __asm__ volatile("int $0x80" : "+r"(eax) : "r"(ebx), "r"(ecx), "r"(edx) : "memory", "cc");

    return eax;
}

// AArch64: write=64, exit=93
#elif defined(IB_ARCH_AARCH64)
export [[nodiscard]]
auto syscall3(usize n, usize a1, usize a2, usize a3) noexcept -> isize
{
    register isize x0 __asm__("x0") = static_cast<isize>(a1);
    register isize x1 __asm__("x1") = static_cast<isize>(a2);
    register isize x2 __asm__("x2") = static_cast<isize>(a3);
    register isize x8 __asm__("x8") = static_cast<isize>(n);

    __asm__ volatile("svc #0" : "+r"(x0) : "r"(x1), "r"(x2), "r"(x8) : "memory");

    return x0;
}

// ARMv7 EABI: write=4, exit=1
#elif defined(IB_ARCH_ARM)
export [[nodiscard]]
auto syscall3(usize n, usize a1, usize a2, usize a3) noexcept -> isize
{
    register isize r0 __asm__("r0") = static_cast<isize>(a1);
    register isize r1 __asm__("r1") = static_cast<isize>(a2);
    register isize r2 __asm__("r2") = static_cast<isize>(a3);
    register isize r7 __asm__("r7") = static_cast<isize>(n);

    __asm__ volatile("svc 0" : "+r"(r0) : "r"(r1), "r"(r2), "r"(r7) : "memory", "cc");

    return r0;
}

// MIPS64 little-endian, n64 ABI: __NR_Linux=5000, write=5001, exit=5058
#elif defined(IB_ARCH_MIPS64)
export [[nodiscard]]
auto syscall3(usize n, usize a1, usize a2, usize a3) noexcept -> isize
{
    register usize v0 __asm__("$2") = n;
    register usize a0 __asm__("$4") = a1;
    register usize a1r __asm__("$5") = a2;
    register usize a2r __asm__("$6") = a3;

    __asm__ volatile("syscall" : "+r"(v0) : "r"(a0), "r"(a1r), "r"(a2r) : "$1", "$3", "$7", "memory");

    return static_cast<isize>(v0);
}

// PowerPC64 little-endian: write=4, exit=1
#elif defined(IB_ARCH_PPC64)
export [[nodiscard]]
auto syscall3(usize n, usize a1, usize a2, usize a3) noexcept -> isize
{
    register isize r0 __asm__("r0") = static_cast<isize>(n);
    register isize r3 __asm__("r3") = static_cast<isize>(a1);
    register isize r4 __asm__("r4") = static_cast<isize>(a2);
    register isize r5 __asm__("r5") = static_cast<isize>(a3);
    __asm__ volatile("sc"
        : "+r"(r3)
        : "r"(r0), "r"(r4), "r"(r5)
        : "memory", "cc", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "ctr", "xer");

    return r3;
}

// RISC-V 64: write=64, exit=93
#elif defined(IB_ARCH_RISCV64)
export [[nodiscard]]
auto syscall3(usize n, usize a1, usize a2, usize a3) noexcept -> isize
{
    register isize a0 __asm__("a0") = static_cast<isize>(a1);
    register isize a1r __asm__("a1") = static_cast<isize>(a2);
    register isize a2r __asm__("a2") = static_cast<isize>(a3);
    register isize a7 __asm__("a7") = static_cast<isize>(n);

    __asm__ volatile("ecall" : "+r"(a0) : "r"(a1r), "r"(a2r), "r"(a7) : "memory");

    return a0;
}

// IBM Z / s390x: write=4, exit=1
#elif defined(IB_ARCH_S390X)
export [[nodiscard]]
auto syscall3(usize n, usize a1, usize a2, usize a3) noexcept -> isize
{
    register isize r1 __asm__("r1") = static_cast<isize>(n);
    register isize r2 __asm__("r2") = static_cast<isize>(a1);
    register isize r3 __asm__("r3") = static_cast<isize>(a2);
    register isize r4 __asm__("r4") = static_cast<isize>(a3);

    __asm__ volatile("svc 0" : "+r"(r2) : "r"(r1), "r"(r3), "r"(r4) : "memory", "cc");

    return r2;
}

// LoongArch64: write=64, exit=93
#elif defined(IB_ARCH_LOONGARCH64)
export [[nodiscard]]
auto syscall3(usize n, usize a1, usize a2, usize a3) noexcept -> isize
{
    register isize a0 __asm__("a0") = static_cast<isize>(a1);
    register isize a1r __asm__("a1") = static_cast<isize>(a2);
    register isize a2r __asm__("a2") = static_cast<isize>(a3);
    register isize a7 __asm__("a7") = static_cast<isize>(n);

    __asm__ volatile("syscall 0" : "+r"(a0) : "r"(a1r), "r"(a2r), "r"(a7) : "memory");

    return a0;
}
#endif

#elif defined(IB_OS_FREEBSD) || defined(IB_OS_OPENBSD) || defined(IB_OS_NETBSD) || defined(IB_OS_DRAGONFLY)

// FreeBSD, OpenBSD, NetBSD, and DragonFly share the same syscall numbers
// (write=4, exit=1) and the same register/instruction conventions.
export constexpr usize SYS_WRITE = 4;
export constexpr usize SYS_EXIT = 1;

// amd64
#if defined(IB_ARCH_AMD64)
export [[nodiscard]]
auto syscall3(usize n, usize a1, usize a2, usize a3) noexcept -> isize
{
    isize ret = 0;
    register isize r10 __asm__("r10") = 0;
    register isize r8 __asm__("r8") = 0;
    register isize r9 __asm__("r9") = 0;
    __asm__ volatile("syscall"
        : "=a"(ret)
        : "0"(static_cast<isize>(n)), "D"(static_cast<isize>(a1)), "S"(static_cast<isize>(a2)),
        "d"(static_cast<isize>(a3)), "r"(r10), "r"(r8), "r"(r9)
        : "rcx", "r11", "memory");
    return ret;
}

// i386: args are passed on the stack (like a normal C call), the syscall
// number goes in eax, and the trap is `int $0x80`.
#elif defined(IB_ARCH_I386)
export [[nodiscard]]
auto syscall3(usize n, usize a1, usize a2, usize a3) noexcept -> isize
{
    isize ret = 0;
    __asm__ volatile("pushl %4\n\t"
                     "pushl %3\n\t"
                     "pushl %2\n\t"
                     "pushl %%eax\n\t"
                     "int   $0x80\n\t"
                     "addl  $16, %%esp\n\t"
        : "=a"(ret)
        : "0"(static_cast<isize>(n)), "g"(static_cast<isize>(a1)), "g"(static_cast<isize>(a2)),
        "g"(static_cast<isize>(a3))
        : "memory", "cc");
    return ret;
}

// AArch64
#elif defined(IB_ARCH_AARCH64)
export [[nodiscard]]
auto syscall3(usize n, usize a1, usize a2, usize a3) noexcept -> isize
{
    register isize x8 __asm__("x8") = static_cast<isize>(n);
    register isize x0 __asm__("x0") = static_cast<isize>(a1);
    register isize x1 __asm__("x1") = static_cast<isize>(a2);
    register isize x2 __asm__("x2") = static_cast<isize>(a3);

    __asm__ volatile("svc #0" : "+r"(x0) : "r"(x8), "r"(x1), "r"(x2) : "memory", "cc");

    return x0;
}

// ARMv7
#elif defined(IB_ARCH_ARM)
export [[nodiscard]]
auto syscall3(usize n, usize a1, usize a2, usize a3) noexcept -> isize
{
    register isize r7 __asm__("r7") = static_cast<isize>(n);
    register isize r0 __asm__("r0") = static_cast<isize>(a1);
    register isize r1 __asm__("r1") = static_cast<isize>(a2);
    register isize r2 __asm__("r2") = static_cast<isize>(a3);

    __asm__ volatile("swi #0" : "+r"(r0) : "r"(r7), "r"(r1), "r"(r2) : "memory", "cc");

    return r0;
}

// PowerPC64 (also covers powerpc64le)
#elif defined(IB_ARCH_PPC64)
export [[nodiscard]]
auto syscall3(usize n, usize a1, usize a2, usize a3) noexcept -> isize
{
    register isize r0 __asm__("r0") = static_cast<isize>(n);
    register isize r3 __asm__("r3") = static_cast<isize>(a1);
    register isize r4 __asm__("r4") = static_cast<isize>(a2);
    register isize r5 __asm__("r5") = static_cast<isize>(a3);

    __asm__ volatile("sc"
        : "+r"(r3)
        : "r"(r0), "r"(r4), "r"(r5)
        : "memory", "cc", "r6", "r7", "r8", "r9", "r10", "r11", "r12");

    return r3;
}

// RISC-V 64
#elif defined(IB_ARCH_RISCV64)
export [[nodiscard]]
auto syscall3(usize n, usize a1, usize a2, usize a3) noexcept -> isize
{
    register isize a7 __asm__("a7") = static_cast<isize>(n);
    register isize a0 __asm__("a0") = static_cast<isize>(a1);
    register isize a1r __asm__("a1") = static_cast<isize>(a2);
    register isize a2r __asm__("a2") = static_cast<isize>(a3);

    __asm__ volatile("ecall" : "+r"(a0) : "r"(a7), "r"(a1r), "r"(a2r) : "memory");

    return a0;
}

#else
#error "ib.sys: unsupported BSD architecture"
#endif

#elif defined(IB_OS_MACOS)

// macOS syscalls live in the BSD class (class 2): 0x2000000 + number.
export constexpr usize SYS_WRITE = 0x2000004;
export constexpr usize SYS_EXIT = 0x2000001;

// amd64
#if defined(IB_ARCH_AMD64)
export [[nodiscard]]
auto syscall3(usize n, usize a1, usize a2, usize a3) noexcept -> isize
{
    register isize rax __asm__("rax") = static_cast<isize>(n);
    register isize rdi __asm__("rdi") = static_cast<isize>(a1);
    register isize rsi __asm__("rsi") = static_cast<isize>(a2);
    register isize rdx __asm__("rdx") = static_cast<isize>(a3);

    __asm__ volatile("syscall" : "+r"(rax) : "r"(rdi), "r"(rsi), "r"(rdx) : "rcx", "r11", "memory");

    return rax;
}

// AArch64
#elif defined(IB_ARCH_AARCH64)
export [[nodiscard]]
auto syscall3(usize n, usize a1, usize a2, usize a3) noexcept -> isize
{
    register isize x0 __asm__("x0") = static_cast<isize>(a1);
    register isize x1 __asm__("x1") = static_cast<isize>(a2);
    register isize x2 __asm__("x2") = static_cast<isize>(a3);
    register isize x16 __asm__("x16") = static_cast<isize>(n);

    __asm__ volatile("svc #0" : "+r"(x0) : "r"(x1), "r"(x2), "r"(x16) : "memory");

    return x0;
}

#else
#error "ib.sys: unsupported macOS architecture"
#endif

#elif defined(IB_OS_SOLARIS)

// Solaris / Illumos syscall numbers (from <sys/syscall.h>).
export constexpr usize SYS_WRITE = 4;
export constexpr usize SYS_EXIT = 1;

// amd64 (SysV syscall ABI)
#if defined(IB_ARCH_AMD64)
export [[nodiscard]]
auto syscall3(usize n, usize a1, usize a2, usize a3) noexcept -> isize
{
    register isize rax __asm__("rax") = static_cast<isize>(n);
    register isize rdi __asm__("rdi") = static_cast<isize>(a1);
    register isize rsi __asm__("rsi") = static_cast<isize>(a2);
    register isize rdx __asm__("rdx") = static_cast<isize>(a3);

    __asm__ volatile("syscall" : "+r"(rax) : "r"(rdi), "r"(rsi), "r"(rdx) : "rcx", "r11", "memory");

    return rax;
}

// SPARC64 (sparcv9): the number goes in g1, args in o0-o2, trap is ta 0x40.
// Note: macros.hpp has no SPARC arch macro yet, so raw __sparc* checks are used.
#elif defined(__sparcv9) || defined(__sparc64__) || (defined(__sparc__) && defined(__arch64__))
export [[nodiscard]]
auto syscall3(usize n, usize a1, usize a2, usize a3) noexcept -> isize
{
    register isize o0 __asm__("o0") = static_cast<isize>(a1);
    register isize o1 __asm__("o1") = static_cast<isize>(a2);
    register isize o2 __asm__("o2") = static_cast<isize>(a3);
    register isize g1 __asm__("g1") = static_cast<isize>(n);

    __asm__ volatile("ta 0x40" : "+r"(o0) : "r"(o1), "r"(o2), "r"(g1) : "cc", "memory");

    return o0;
}

#else
#error "ib.sys: unsupported Solaris architecture (amd64 or sparc64 only)"
#endif

#endif  // platform

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
auto write(isize fd, const void* data, usize size) noexcept -> isize
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    return syscall3(SYS_WRITE, static_cast<usize>(fd), reinterpret_cast<usize>(data), size);
}

/// Writes a NUL-terminated string to the given file descriptor.
/// Returns the number of bytes written, or a negative error code on failure.
export [[nodiscard]]
auto write_string(isize fd, const char* string) noexcept -> isize
{
    return write(fd, string, str_length(string));
}

/// Writes raw bytes to the standard output stream.
/// Returns the number of bytes written, or a negative error code on failure.
export [[nodiscard]]
auto print(const void* data, usize size) noexcept -> isize
{
    return write(STDOUT, data, size);
}

/// Writes a NUL-terminated string to the standard output stream.
/// Returns the number of bytes written, or a negative error code on failure.
export [[nodiscard]]
auto print(const char* string) noexcept -> isize
{
    return write_string(STDOUT, string);
}

// -------------------------------------------------------------------------
// Process
// -------------------------------------------------------------------------

/// Terminates the process with the given exit status.
export [[noreturn]]
void exit(isize status) noexcept
{
    static_cast<void>(syscall3(SYS_EXIT, static_cast<usize>(status), 0, 0));
    __builtin_unreachable();
}
}  // namespace ib::sys

#else
// Android / iOS / OHOS are detected by macros.hpp but intentionally not covered yet;
// they share the Linux/macOS syscall ABI and would map onto those sections above.
#error "ib.sys: unsupported operating system"
#endif
