-- SPDX-FileCopyrightText: 2026 iFarbod
--
-- SPDX-License-Identifier: LicenseRef-Proprietary

set_project("ib")
set_xmakever("3.0.9") -- raise to 3.1.0 later

set_languages("c23", "cxx23")

add_rules("mode.debug", "mode.releasedbg", "mode.release")
add_rules("plugin.vsxmake.autoupdate")
add_rules("plugin.compile_commands.autoupdate")

set_allowedplats("windows", "mingw", "linux", "cross", "macosx", "bsd", "wasm", "android")
set_defaultplat("windows")
set_allowedarchs(
    "windows|x86",
    "windows|x64",
    "windows|arm",
    "windows|arm64",

    "mingw|i686",
    "mingw|x86_64",
    "mingw|arm",
    "mingw|arm64",

    "android|armeabi-v7a",
    "android|arm64-v8a",
    "android|riscv64",
    "android|x86",
    "android|x86_64"
)
set_defaultarchs(
    "windows|x64",
    "mingw|x86_64",

    "linux|x86_64",
    "macosx|arm64",
    "bsd|x86_64",
    "wasm|wasm32",
    "android|arm64-v8a"
)
set_allowedmodes("debug", "releasedbg", "release")
set_defaultmode("debug")

set_policy("build.c++.modules.std", false)

--- MARK: this is my first attempt xd

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
        if not is_plat("macosx") then
            return
        end

        local triples = {
            x86_64 = "x86_64-apple-macos11.0",
            arm64 = "aarch64-apple-macos11.0"
        }

        local triple = triples[target:arch()]
        if triple then
            target:add("cxxflags", "--target="..triple, {force = true})
            target:add("cxxflags", "-fcxx-modules", {force = true}) -- TODO(iFarbod): until we find a better way to limit this to apple-clang
            target:add("ldflags", "--target="..triple, {force = true})

            target:add("ldflags", "-v", {force = true}) -- TODO(iFarbod): compiler option for this?
        end
    end)
end)

function ib_library(name, configure)
    target(name, function ()
        set_kind("static")

        add_rules("ib.warnings")
        add_rules("ib.clang.target")

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

        add_cxxflags("-fno-rtti", "-fno-stack-protector", {tools = {"clang", "gcc", "clangxx", "gxx"}, force = true})
        add_cxxflags("/GS-", {tools = {"cl", "clang_cl"}, force = true})
        set_exceptions("no-cxx")

        if is_plat("windows") then
            add_ldflags("/NODEFAULTLIB", {tools = "link", force = true})
            add_ldflags("-Wl,/NODEFAULTLIB", {tools = "clang", force = true})

            add_ldflags("/SUBSYSTEM:CONSOLE", {tools = "link", force = true})
            add_ldflags("-Wl,/SUBSYSTEM:CONSOLE", {tools = "clang", force = true})

            add_ldflags("/ENTRY:ib_start", {tools = "link", force = true})
            add_ldflags("-Wl,/ENTRY:ib_start", {tools = "clang", force = true})
        end

        if is_plat("linux") then
            add_ldflags("-nostartfiles", "-nostdlib", "-Wl,-e,ib_start", {tools = {"gcc", "gxx", "clang", "clangxx"}, force = true})
        end

        if is_plat("macosx") then
            add_ldflags("-nostdlib", "-Wl,-e,_ib_start", {tools = {"clang", "clangxx"}, force = true})
        end

        if is_plat("mingw") then
            add_ldflags("-nostartfiles", "-Wl,-e,ib_start", "-Wl,--subsystem,console", {tools = {"gcc", "gxx", "clang", "clangxx"}, force = true})
        end

        if is_plat("windows", "mingw") then
            add_links("kernel32")
        end

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
