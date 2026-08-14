// SPDX-FileCopyrightText: 2026 iFarbod
//
// SPDX-License-Identifier: LicenseRef-Proprietary

// Windows backend: there are no raw syscalls here. Everything goes through
// direct kernel32.dll imports, so no CRT is involved.

export module ib.sys:syscall_windows;

import ib.core;
import :detail;

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

IB_WARNING_DISABLE_CLANG_PUSH("-Wswitch-default")

/// Returns the standard input, output, or error handle.
/// Returns INVALID_HANDLE_VALUE (-1) when the handle cannot be obtained.
export [[nodiscard]]
auto get_std_handle(StdHandle which) -> Handle
{
    constexpr auto std_input_handle = static_cast<u32>(-10);
    constexpr auto std_output_handle = static_cast<u32>(-11);
    constexpr auto std_error_handle = static_cast<u32>(-12);

    u32 n_std_handle = std_output_handle;
    switch (which)
    {
        case StdHandle::Input:
        {
            n_std_handle = std_input_handle;
            break;
        }
        case StdHandle::Error:
        {
            n_std_handle = std_error_handle;
            break;
        }
        case StdHandle::Output:
        {
            n_std_handle = std_output_handle;
            break;
        }
    }
    return GetStdHandle(n_std_handle);
}

IB_WARNING_DISABLE_CLANG_POP

/// Returns whether the given handle refers to a console.
export [[nodiscard]]
auto get_console_mode(Handle console, u32* mode) -> bool
{
    return GetConsoleMode(console, mode) != 0;
}

/// Writes UTF-16 text to a console output handle.
export [[nodiscard]]
auto write_console_w(Handle output, const wchar_t* text, u32 chars, u32* written) -> bool
{
    return WriteConsoleW(output, text, chars, written, nullptr) != 0;
}

/// Writes raw bytes to any file-like handle (console or redirected).
export [[nodiscard]]
auto write_file(Handle file, const void* data, u32 bytes, u32* written) -> bool
{
    return WriteFile(file, data, bytes, written, nullptr) != 0;
}

/// Writes raw bytes to the standard output stream.
/// \return the number of bytes written, or a negative value on failure.
export [[nodiscard]]
auto print(const void* data, usize size) -> isize
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
auto print(const char* string) -> isize
{
    return print(string, detail::str_length(string));
}

/// Terminates the process with the given exit code.
export [[noreturn]]
void exit(isize status)
{
    ExitProcess(static_cast<u32>(status));

#if defined(IB_COMPILER_CLANG) || defined(IB_COMPILER_GCC)
    __builtin_unreachable();
#elifdef IB_COMPILER_MSVC
    __assume(false);
#endif
}
}  // namespace ib::sys

#endif  // IB_OS_WINDOWS
