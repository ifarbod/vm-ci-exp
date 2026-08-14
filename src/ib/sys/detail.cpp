// SPDX-FileCopyrightText: 2026 iFarbod
//
// SPDX-License-Identifier: LicenseRef-Proprietary

export module ib.sys:detail;

import ib.core;

namespace ib::sys::detail
{

IB_WARNING_DISABLE_GCC_PUSH("-Wattributes")
IB_WARNING_DISABLE_CLANG_PUSH("-Wunsafe-buffer-usage")

/// Returns the length of a NUL-terminated string, not counting the terminator.
///
/// clang::no_builtin keeps the optimizer from turning the loop into a `strlen`
/// libcall (there is no libc here) when the call is not constant-folded.
///
/// NOTE: superseded by `ib.c`'s `strlen` once Phase 1 lands.
export [[nodiscard, clang::no_builtin("strlen")]]
constexpr auto str_length(const char* string) -> usize
{
    usize length = 0;
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    while (string[length] != '\0')
    {
        ++length;
    }
    return length;
}

IB_WARNING_DISABLE_CLANG_POP
IB_WARNING_DISABLE_GCC_POP

}  // namespace ib::sys::detail
