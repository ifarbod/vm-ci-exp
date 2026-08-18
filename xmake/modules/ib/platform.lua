-- SPDX-FileCopyrightText: 2026 iFarbod
--
-- SPDX-License-Identifier: LicenseRef-Proprietary

--!ib target platform normalization
--
-- xmake's is_plat() only knows the plat name, and for `-p cross --cross=<triple>`
-- the plat is literally "cross". Everything the build needs (OS family, object
-- format, linker dialect, entry-symbol decoration) is therefore derived from the
-- target triple, which works for native plats and cross triples alike.

import("core.project.config")

-- order matters: "aarch64-linux-android23" is android, not linux
local OSMAP = {
    {"android",    "android"},
    {"emscripten", "wasm"},
    {"wasi",       "wasm"},
    {"wasm",       "wasm"},
    {"freebsd",    "freebsd"},
    {"openbsd",    "openbsd"},
    {"netbsd",     "netbsd"},
    {"dragonfly",  "dragonfly"},
    {"solaris",    "solaris"},
    {"darwin",     "macos"},
    {"macos",      "macos"},
    {"ios",        "iphoneos"},
    {"watchos",    "watchos"},
    {"tvos",       "appletvos"},
    {"apple",      "macos"},
    {"linux",      "linux"},
    {"mingw",      "mingw"},
    {"cygwin",     "mingw"},
    {"windows",    "windows"},
    {"uefi",       "uefi"},
    {"none",       "baremetal"},
    {"elf",        "baremetal"},
}

-- xmake plat -> normalized os name
local PLAT_ALIAS = {
    macosx    = "macos",
    iphoneos  = "iphoneos",
    appletvos = "appletvos",
    watchos   = "watchos",
    android   = "android",
    bsd       = "freebsd",
    msys      = "mingw",
}

-- triple arch token -> xmake arch
local ARCH_ALIAS = {
    aarch64 = "arm64",
    arm64   = "arm64",
    amd64   = "x64",
    x86_64  = "x64",
    i386    = "x86",
    i486    = "x86",
    i586    = "x86",
    i686    = "x86",
    arm     = "arm",
    armv7   = "arm",
    riscv64 = "riscv64",
    wasm32  = "wasm32",
}

function triple(target)
    local value = target:values("ib.triple")
    if type(value) == "table" then
        value = value[1]
    end
    if value and value ~= "" then
        return value
    end
    local cross = config.get("cross")
    if cross and tostring(cross) ~= "" then
        return tostring(cross)
    end
    return nil
end

local function os_from_triple(tri)
    tri = tri:lower()
    for _, entry in ipairs(OSMAP) do
        if tri:find(entry[1], 1, true) then
            return entry[2]
        end
    end
    return "baremetal"
end

local function arch_from_triple(tri)
    local arch = (tri:split("-", {plain = true})[1] or ""):lower()
    return ARCH_ALIAS[arch] or arch
end

local function detect(target)
    local plat = target:plat()
    local tri  = triple(target)

    local osname = PLAT_ALIAS[plat] or plat
    local arch = target:arch()
    if plat == "cross" or plat == "generic" then
        osname = (tri and os_from_triple(tri)) or "baremetal"
        if tri then
            arch = arch_from_triple(tri)
        end
    end

    local binfmt = "elf"
    if osname == "windows" or osname == "mingw" then
        binfmt = "pe"
    elseif osname == "macos" or osname == "iphoneos" or osname == "watchos" or osname == "appletvos" then
        binfmt = "macho"
    elseif osname == "wasm" then
        binfmt = "wasm"
    end

    local ldstyle = "gnu"
    if osname == "windows" then
        ldstyle = "msvc"
    elseif binfmt == "macho" then
        ldstyle = "ld64"
    elseif binfmt == "wasm" then
        ldstyle = "wasm"
    end

    local symprefix = ""
    if binfmt == "macho" then
        symprefix = "_"
    elseif binfmt == "pe" and (arch == "x86" or arch == "i386" or arch == "i686") then
        symprefix = "_"
    end

    return {
        plat      = plat,
        arch      = arch,
        triple    = tri,
        os        = osname,
        binfmt    = binfmt,
        ldstyle   = ldstyle,
        symprefix = symprefix,
    }
end

function get(target)
    local info = target:data("ib.platform")
    if info == nil then
        info = detect(target)
        target:data_set("ib.platform", info)
    end
    return info
end

function is_os(target, ...)
    local this = get(target).os
    for _, name in ipairs({...}) do
        if this == name then
            return true
        end
    end
    return false
end

function is_pe(target)
    return is_os(target, "windows", "mingw")
end

function is_macho(target)
    return get(target).binfmt == "macho"
end

function is_elf(target)
    return get(target).binfmt == "elf"
end

-- The linker's -e/--entry takes the *mangled* symbol name; Mach-O prefixes
-- C symbols with '_', as does 32-bit x86 PE (cdecl).
function entry_symbol(target, name)
    return get(target).symprefix .. name
end
