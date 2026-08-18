// SPDX-FileCopyrightText: 2026 iFarbod
//
// SPDX-License-Identifier: LicenseRef-Proprietary

import ib.core;
import ib.sys;

auto ib_main() -> i32;

extern "C" [[noreturn]]
void ib_start()
{
    ib::sys::exit(ib_main());
}

#if OS(MACOS)
extern "C" void dyld_stub_binder()
{
    // must panic
}
#endif
