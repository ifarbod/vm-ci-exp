-- SPDX-FileCopyrightText: 2026 iFarbod
--
-- SPDX-License-Identifier: LicenseRef-Proprietary

set_project("ib")
set_xmakever("3.0.9") -- raise to 3.1.0 later

set_languages("c23", "cxx23")

add_rules("mode.debug", "mode.releasedbg", "mode.release")
add_rules("plugin.vsxmake.autoupdate")
add_rules("plugin.compile_commands.autoupdate")

-- Everything past Windows/Linux is built through the generic "cross" platform
-- plus an explicit --cross=<triple>. xmake's "macos"/"bsd"/"android" plats
-- inject their own flags and links (e.g. -lz on macOS), which a freestanding
-- build does not want.
set_allowedplats("windows", "mingw", "linux", "cross", "wasm")
set_defaultplat("windows")
set_allowedarchs(
    "windows|x86",
    "windows|x64",
    "windows|arm",
    "windows|arm64",

    "mingw|i686",
    "mingw|x86_64",
    "mingw|arm",
    "mingw|arm64"
)
set_defaultarchs(
    "windows|x64",
    "mingw|x86_64",

    "linux|x86_64",
    "wasm|wasm32"
)
set_allowedmodes("debug", "releasedbg", "release")
set_defaultmode("debug")

set_policy("build.c++.modules.std", false)

add_moduledirs("xmake/modules")

rule("ib.warnings", function ()
    on_config(function (target)
        if target:has_tool("cxx", "clang", "clangxx", "clang_cl") then
            target:add("cxxflags", "-Weverything", "-Wno-c++98-compat-pedantic", "-Wno-missing-prototypes", {force = true})
        elseif target:has_tool("cxx", "gcc", "gxx") then
            target:add("cxxflags",
                "-Wall",
                "-Wextra",
                "-Wpedantic",
                "-Wshadow",
                "-Wconversion",
                "-Wsign-conversion",
                "-Wcast-align",
                "-Wunused",
                "-Wnull-dereference",
                "-Wdouble-promotion",
                "-Wformat=2",
                "-Wimplicit-fallthrough",
                "-Wduplicated-cond",
                "-Wduplicated-branches",
                "-Wlogical-op",
                {force = true})
        elseif target:has_tool("cxx", "cl") then
            target:add("cxxflags", "/Wall", {force = true})
            target:add("cxxflags", "/wd5030", {force = true}) -- unknown attribute
        end
    end)
end)

rule("ib.clang.target", function ()
    on_config(function (target)
        if not target:has_tool("cxx", "clang", "clangxx") then
            return
        end

        import("ib.platform")
        local pi  = platform.get(target)
        local tri = pi.triple

        if tri then
            target:add("cxxflags", "--target=" .. tri, {force = true})
            target:add("ldflags", "--target=" .. tri, {force = true})
        end

        -- apple-clang (only present on macOS hosts) needs this to enable C++20
        -- modules; upstream clang handles them natively.
        if pi.os == "macos" and is_host("macosx") then
            target:add("cxxflags", "-fcxx-modules", {force = true})

            target:add("ldflags", "-v", {force = true})
        end
    end)
end)

rule("ib.freestanding", function ()
    on_config(function (target)
        target:add("cxxflags", "-ffreestanding", "-fno-builtin",
            {tools = {"clang", "gcc", "clangxx", "gxx", "clang_cl"}, force = true})
        target:add("cxxflags", "-nostdinc",
            {tools = {"clang", "clangxx"}, force = true})
        target:add("cxxflags", "-nostdinc",
            {tools = {"gcc", "gxx"}, force = true})
        target:add("cxxflags", "/X",
            {tools = {"cl", "clang_cl"}, force = true})
    end)
end)

-- Entry point + subsystem + artifact shape, driven by the normalized platform.
-- Works for native plats and -p cross --cross=<triple> alike.
rule("ib.entrypoint", function ()
    on_load(function (target)
        import("ib.platform")
        local pi    = platform.get(target)
        local entry = platform.entry_symbol(target, "ib_start")
        local gnu   = {"gcc", "gxx", "clang", "clangxx"}

        if pi.binfmt == "wasm" then
            target:set("extension", ".wasm")
        end

        if pi.os == "windows" then
            -- msvc link.exe
            target:add("ldflags", "/NODEFAULTLIB", {tools = "link", force = true})
            target:add("ldflags", "/SUBSYSTEM:CONSOLE", {tools = "link", force = true})
            target:add("ldflags", "/ENTRY:ib_start", {tools = "link", force = true})
            -- clang driving link.exe / lld-link
            target:add("ldflags", "-Wl,/NODEFAULTLIB", {tools = "clang", force = true})
            target:add("ldflags", "-Wl,/SUBSYSTEM:CONSOLE", {tools = "clang", force = true})
            target:add("ldflags", "-Wl,/ENTRY:ib_start", {tools = "clang", force = true})
        elseif pi.os == "mingw" then
            target:add("ldflags", "-nostartfiles", "-Wl,-e," .. entry, "-Wl,--subsystem,console", {tools = gnu, force = true})
        elseif pi.binfmt == "macho" then
            -- macOS / iOS: -nostdlib, custom entry (Mach-O underscores C symbols)
            target:add("ldflags", "-nostdlib", "-Wl,-e," .. entry, {tools = {"clang", "clangxx"}, force = true})
        elseif pi.binfmt == "wasm" then
            target:add("ldflags", "-Wl,--no-entry", {tools = gnu, force = true})
        else
            -- linux / freebsd / openbsd / netbsd / dragonfly / solaris / android
            target:add("ldflags", "-nostartfiles", "-nostdlib", "-Wl,-e," .. entry, {tools = gnu, force = true})
        end

        if pi.os == "windows" or pi.os == "mingw" then
            target:add("links", "kernel32")
        end
    end)
end)

function ib_library(name, configure)
    target(name, function ()
        set_kind("static")

        add_rules("ib.warnings")
        add_rules("ib.clang.target")
        add_rules("ib.freestanding")

        add_cxxflags("-fno-rtti", "-fno-stack-protector", {tools = {"clang", "gcc", "clangxx", "gxx"}, force = true})
        add_cxxflags("/GS-", {tools = {"cl", "clang_cl"}, force = true})
        set_exceptions("no-cxx")

        if configure then
            configure()
        end
    end)
end

function ib_executable(name, configure)
    target(name, function ()
        set_kind("binary")

        add_rules("ib.warnings")
        add_rules("ib.clang.target")
        add_rules("ib.entrypoint")
        add_rules("ib.freestanding")

        add_cxxflags("-fno-rtti", "-fno-stack-protector", {tools = {"clang", "gcc", "clangxx", "gxx"}, force = true})
        add_cxxflags("/GS-", {tools = {"cl", "clang_cl"}, force = true})
        set_exceptions("no-cxx")

        add_deps("ib")
        add_files("src/crt/crt.cpp")

        if configure then
            configure()
        end
    end)
end

ib_library("ib", function ()
    set_group("ib")

    add_headerfiles("src/ib/**.hpp")
    add_files("src/ib/**.cpp", "src/ib/**.cppm", {public = true})

    add_forceincludes(path.absolute(path.join(os.projectdir(), "src/ib/macros.hpp")))
end)

ib_executable("ib_app", function ()
    set_group("ib")

    add_files("src/app/**.cpp")

    add_forceincludes(path.absolute(path.join(os.projectdir(), "src/ib/macros.hpp")))
end)

-- Configuration cheat sheet
--   windows (native):   xmake f -c -p windows -a x64 --toolchain=msvc|clang
--   mingw (native):     xmake f -c -p mingw -a x86_64
--   linux (native):     xmake f -c -p linux -a x86_64
--
--   cross (triple-driven; -a is ignored, the triple decides arch/OS):
--     macOS:            xmake f -c -p cross --cross=x86_64-apple-macos11.0  --toolchain=clang
--                       xmake f -c -p cross --cross=aarch64-apple-macos11.0 --toolchain=clang
--     iOS:              xmake f -c -p cross --cross=arm64-apple-ios13.0        --toolchain=clang
--     FreeBSD:          xmake f -c -p cross --cross=x86_64-unknown-freebsd12.3 --toolchain=clang
--     OpenBSD:          xmake f -c -p cross --cross=x86_64-unknown-openbsd7.0  --toolchain=clang
--     NetBSD:           xmake f -c -p cross --cross=x86_64-unknown-netbsd9.0   --toolchain=clang
--     DragonFly:        xmake f -c -p cross --cross=x86_64-unknown-dragonfly   --toolchain=clang
--     Android (termux): xmake f -c -p cross --cross=aarch64-linux-android23    --toolchain=clang
--     Linux:            xmake f -c -p cross --cross=aarch64-linux-gnu          --toolchain=clang
--     wasm:             xmake f -c -p cross --cross=wasm32-emscripten          --toolchain=clang
