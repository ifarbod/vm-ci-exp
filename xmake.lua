-- SPDX-FileCopyrightText: 2026 iFarbod
--
-- SPDX-License-Identifier: LicenseRef-Proprietary

set_project("ib")
set_xmakever("3.1.0")

set_languages("c23", "cxx23")

add_rules("mode.debug", "mode.releasedbg", "mode.release")
add_rules("plugin.vsxmake.autoupdate")
add_rules("plugin.compile_commands.autoupdate")

set_allowedplats("windows", "mingw", "linux", "macosx", "bsd", "wasm", "android")
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

function ib_library(name, configure)
    target(name, function ()
        set_kind("static")

        if configure then
            configure()
        end
    end)
end

function ib_executable(name, configure)
    target(name, function ()
        set_kind("binary")

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

    add_cxxflags("-Wno-c++23-extensions", {tools = "clang", force = true})

    add_forceincludes(path.absolute(path.join(os.projectdir(), "src/ib/macros.hpp")))
end)

ib_executable("ib_app", function ()
    set_group("ib")

    add_files("src/app/**.cpp")

    add_cxxflags("-Wno-c++23-extensions", {tools = "clang", force = true})

    add_forceincludes(path.absolute(path.join(os.projectdir(), "src/ib/macros.hpp")))
end)
