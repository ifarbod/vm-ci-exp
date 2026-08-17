// SPDX-FileCopyrightText: 2026 iFarbod
//
// SPDX-License-Identifier: LicenseRef-Proprietary

export module ib.core:types;

// NOLINTBEGIN(readability-identifier-naming, google-runtime-int)
export {

using i8 = signed char;
using i16 = signed short;
using i32 = signed int;
using i64 = signed long long;

using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

using isize = decltype("" - "");
using usize = decltype(sizeof(nullptr));

using f32 = float;
using f64 = double;
}
// NOLINTEND(readability-identifier-naming, google-runtime-int)
