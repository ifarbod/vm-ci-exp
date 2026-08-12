// SPDX-FileCopyrightText: 2026 iFarbod
//
// SPDX-License-Identifier: LicenseRef-Proprietary

import ib.core;
import ib.sys;

auto main() -> i32;

extern "C" [[noreturn]] void _start()
{
    ib::sys::exit(main());
}
