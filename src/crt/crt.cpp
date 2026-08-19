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

#if OS(FREEBSD)

constexpr auto ELF_NOTE_FREEBSD_NAMESZ = 8;
constexpr auto ELF_NOTE_FREEBSD_DESCSZ = 4;
constexpr auto ELF_NOTE_FREEBSD_ABI_TAG = 1;
constexpr auto ELF_NOTE_FREEBSD_NOINIT_TAG = 2;
constexpr auto ELF_NOTE_FREEBSD_FEATURE_CTL_TAG = 4;
constexpr auto ELF_NOTE_FREEBSD_VERSION = 1203000;  // 12.3-RELEASE
constexpr auto ELF_NOTE_FREEBSD_FCTL_ASLR_DISABLE = 0x1;

extern "C" __attribute__((section(".note.tag"), used, retain, aligned(4))) struct
{
    struct
    {
        u32 namesz;
        u32 descsz;
        u32 type;
        char name[ELF_NOTE_FREEBSD_NAMESZ];
        u32 version;
    } abi_tag;

    struct
    {
        u32 namesz;
        u32 descsz;
        u32 type;
        char name[ELF_NOTE_FREEBSD_NAMESZ];
        u32 noinit;
    } noinit_tag;

    struct
    {
        u32 namesz;
        u32 descsz;
        u32 type;
        char name[ELF_NOTE_FREEBSD_NAMESZ];
        u32 feature_ctl;
    } feature_ctl_tag;
} ib_abi_note = {
    .abi_tag =
        {
            .namesz = ELF_NOTE_FREEBSD_NAMESZ,
            .descsz = ELF_NOTE_FREEBSD_DESCSZ,
            .type = ELF_NOTE_FREEBSD_ABI_TAG,
            .name = "FreeBSD",
            .version = ELF_NOTE_FREEBSD_VERSION,
        },
    .noinit_tag =
        {
            .namesz = ELF_NOTE_FREEBSD_NAMESZ,
            .descsz = ELF_NOTE_FREEBSD_DESCSZ,
            .type = ELF_NOTE_FREEBSD_NOINIT_TAG,
            .name = "FreeBSD",
            .noinit = 0,
        },
    .feature_ctl_tag =
        {
            .namesz = ELF_NOTE_FREEBSD_NAMESZ,
            .descsz = ELF_NOTE_FREEBSD_DESCSZ,
            .type = ELF_NOTE_FREEBSD_FEATURE_CTL_TAG,
            .name = "FreeBSD",
            .feature_ctl = 0,
        },
};

#elif OS(OPENBSD)

constexpr auto ELF_NOTE_OPENBSD_NAMESZ = 8;
constexpr auto ELF_NOTE_OPENBSD_DESCSZ = 4;
constexpr auto ELF_NOTE_OPENBSD_TAG = 1;
constexpr auto ELF_NOTE_OPENBSD_VERSION = 0;

extern "C" __attribute__((section(".note.openbsd.ident"), used, aligned(4))) struct
{
    u32 namesz;
    u32 descsz;
    u32 type;
    char name[8];
    u32 version;
} ib_abi_note = {.namesz = ELF_NOTE_OPENBSD_NAMESZ,
    .descsz = ELF_NOTE_OPENBSD_DESCSZ,
    .type = ELF_NOTE_OPENBSD_TAG,
    .name = "OpenBSD",
    .version = ELF_NOTE_OPENBSD_VERSION};

#elif OS(NETBSD)

constexpr auto ELF_NOTE_NETBSD_NAMESZ = 7;
constexpr auto ELF_NOTE_NETBSD_DESCSZ = 4;
constexpr auto ELF_NOTE_NETBSD_TAG = 1;
constexpr auto ELF_NOTE_NETBSD_VERSION = 700000000;  // 7.0

extern "C" __attribute__((section(".note.netbsd.ident"), used, retain, aligned(4))) struct
{
    u32 namesz;
    u32 descsz;
    u32 type;
    char name[8];
    u32 version;
} ib_abi_note = {.namesz = ELF_NOTE_NETBSD_NAMESZ,
    .descsz = ELF_NOTE_NETBSD_DESCSZ,
    .type = ELF_NOTE_NETBSD_TAG,
    .name = "NetBSD\0",
    .version = ELF_NOTE_NETBSD_VERSION};

#endif
