// SPDX-FileCopyrightText: 2026 iFarbod
//
// SPDX-License-Identifier: LicenseRef-Proprietary

#pragma once

// NOLINTBEGIN(cppcoreguidelines-macro-usage)

// MARK: compiler

#ifdef __clang__
#define IB_COMPILER_CLANG 1
#elifdef __GNUC__
#define IB_COMPILER_GCC 1
#elifdef _MSC_VER
#define IB_COMPILER_MSVC 1
#else
#define IB_COMPILER_UNKNOWN 1
#endif

// MARK: os

#ifdef _WIN32
#define IB_OS_WINDOWS 1

#elifdef __ANDROID__
#define IB_OS_ANDROID 1

#elifdef __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__
#define IB_OS_MACOS 1

#elifdef __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__
#define IB_OS_IOS 1

#elifdef __linux__
#define IB_OS_LINUX 1

#elifdef __FreeBSD__
#define IB_OS_FREEBSD 1
#elifdef __OpenBSD__
#define IB_OS_OPENBSD 1
#elifdef __NetBSD__
#define IB_OS_NETBSD 1
#elifdef __DragonFly__
#define IB_OS_DRAGONFLY 1

#elifdef __sun__
#define IB_OS_SOLARIS 1

#elifdef __OHOS__
#define IB_OS_OHOS 1

#else
#define IB_OS_UNKNOWN 1
#endif

// MARK: arch

#if defined(__amd64__) || defined(_M_AMD64)
#define IB_ARCH_AMD64 1
#elif defined(__i386__) || defined(_M_IX86)
#define IB_ARCH_I386 1
#elif defined(__aarch64__) || defined(_M_ARM64)
#define IB_ARCH_AARCH64 1
#elif defined(__arm__) || defined(_M_ARM)
#define IB_ARCH_ARM 1
#elif defined(__powerpc64__) || defined(__ppc64__)
#define IB_ARCH_PPC64 1
#elif defined(__mips64)
#define IB_ARCH_MIPS64 1
#elif defined(__riscv) && __riscv_xlen == 64
#define IB_ARCH_RISCV64 1
#elif defined(__s390x__)
#define IB_ARCH_S390X 1
#elif defined(__loongarch64)
#define IB_ARCH_LOONGARCH64 1
#else
#define IB_ARCH_UNKNOWN 1
#endif

// TODO: add convenient swift-like macros
// #if COMPILER(MSVC)
// #if OS(WINDOWS)
// #if ARCH(AMD64)

// MARK: warnings

// useful macros for disabling warnings in a cross-platform way
// usage:
// IB_WARNING_PUSH
// IB_WARNING_DISABLE_GCC_CLANG("-Wfloat-equal")
// IB_WARNING_DISABLE_MSVC(4800)
// code that triggers the warning
// IB_WARNING_POP
//
// or, using the compiler-specific push/pop pairs:
// IB_WARNING_DISABLE_CLANG_PUSH("-Wfloat-equal")
// code
// IB_WARNING_DISABLE_CLANG_POP

#define IB_PRAGMA_TO_STR(x) _Pragma(#x)

#ifdef IB_COMPILER_CLANG

#define IB_WARNING_PUSH IB_PRAGMA_TO_STR(clang diagnostic push)
#define IB_WARNING_POP IB_PRAGMA_TO_STR(clang diagnostic pop)

#define IB_WARNING_DISABLE_CLANG(warning) IB_PRAGMA_TO_STR(clang diagnostic ignored warning)
#define IB_WARNING_DISABLE_CLANG_PUSH(warning) IB_WARNING_PUSH IB_WARNING_DISABLE_CLANG(warning)
#define IB_WARNING_DISABLE_CLANG_POP IB_WARNING_POP

#define IB_WARNING_DISABLE_GCC(warning)
#define IB_WARNING_DISABLE_GCC_PUSH(warning)
#define IB_WARNING_DISABLE_GCC_POP

#define IB_WARNING_DISABLE_MSVC(warning)
#define IB_WARNING_DISABLE_MSVC_PUSH(warning)
#define IB_WARNING_DISABLE_MSVC_POP

#define IB_WARNING_DISABLE_GCC_CLANG(warning) IB_WARNING_DISABLE_CLANG(warning)
#define IB_WARNING_DISABLE_GCC_CLANG_PUSH(warning) IB_WARNING_PUSH IB_WARNING_DISABLE_GCC_CLANG(warning)
#define IB_WARNING_DISABLE_GCC_CLANG_POP IB_WARNING_POP

#define IB_WARNING_DISABLE_DEPRECATED \
    IB_WARNING_DISABLE_CLANG("-Wdeprecated-declarations") IB_WARNING_DISABLE_CLANG("-Wdeprecated-pragma")

#elifdef IB_COMPILER_GCC

#define IB_WARNING_PUSH IB_PRAGMA_TO_STR(GCC diagnostic push)
#define IB_WARNING_POP IB_PRAGMA_TO_STR(GCC diagnostic pop)

#define IB_WARNING_DISABLE_CLANG(warning)
#define IB_WARNING_DISABLE_CLANG_PUSH(warning)
#define IB_WARNING_DISABLE_CLANG_POP

#define IB_WARNING_DISABLE_GCC(warning) IB_PRAGMA_TO_STR(GCC diagnostic ignored warning)
#define IB_WARNING_DISABLE_GCC_PUSH(warning) IB_WARNING_PUSH IB_WARNING_DISABLE_GCC(warning)
#define IB_WARNING_DISABLE_GCC_POP IB_WARNING_POP

#define IB_WARNING_DISABLE_MSVC(warning)
#define IB_WARNING_DISABLE_MSVC_PUSH(warning)
#define IB_WARNING_DISABLE_MSVC_POP

#define IB_WARNING_DISABLE_GCC_CLANG(warning) IB_WARNING_DISABLE_GCC(warning)
#define IB_WARNING_DISABLE_GCC_CLANG_PUSH(warning) IB_WARNING_PUSH IB_WARNING_DISABLE_GCC_CLANG(warning)
#define IB_WARNING_DISABLE_GCC_CLANG_POP IB_WARNING_POP

#define IB_WARNING_DISABLE_DEPRECATED IB_WARNING_DISABLE_GCC("-Wdeprecated-declarations")

#elifdef IB_COMPILER_MSVC

#undef IB_PRAGMA_TO_STR

#define IB_WARNING_PUSH __pragma(warning(push))
#define IB_WARNING_POP __pragma(warning(pop))

#define IB_WARNING_DISABLE_CLANG(warning)
#define IB_WARNING_DISABLE_CLANG_PUSH(warning)
#define IB_WARNING_DISABLE_CLANG_POP

#define IB_WARNING_DISABLE_GCC(warning)
#define IB_WARNING_DISABLE_GCC_PUSH(warning)
#define IB_WARNING_DISABLE_GCC_POP

#define IB_WARNING_DISABLE_MSVC(warning) __pragma(warning(disable : warning))
#define IB_WARNING_DISABLE_MSVC_PUSH(warning) IB_WARNING_PUSH IB_WARNING_DISABLE_MSVC(warning)
#define IB_WARNING_DISABLE_MSVC_POP IB_WARNING_POP

#define IB_WARNING_DISABLE_GCC_CLANG(warning)
#define IB_WARNING_DISABLE_GCC_CLANG_PUSH(warning)
#define IB_WARNING_DISABLE_GCC_CLANG_POP

#define IB_WARNING_DISABLE_DEPRECATED IB_WARNING_DISABLE_MSVC(4996) IB_WARNING_DISABLE_MSVC(4995)

#else

#define IB_WARNING_PUSH
#define IB_WARNING_POP

#define IB_WARNING_DISABLE_CLANG(warning)
#define IB_WARNING_DISABLE_CLANG_PUSH(warning)
#define IB_WARNING_DISABLE_CLANG_POP

#define IB_WARNING_DISABLE_GCC(warning)
#define IB_WARNING_DISABLE_GCC_PUSH(warning)
#define IB_WARNING_DISABLE_GCC_POP

#define IB_WARNING_DISABLE_MSVC(warning)
#define IB_WARNING_DISABLE_MSVC_PUSH(warning)
#define IB_WARNING_DISABLE_MSVC_POP

#define IB_WARNING_DISABLE_GCC_CLANG(warning)
#define IB_WARNING_DISABLE_GCC_CLANG_PUSH(warning)
#define IB_WARNING_DISABLE_GCC_CLANG_POP

#define IB_WARNING_DISABLE_DEPRECATED

#endif

// MARK: utils

#define IB_DISCARD(expr) (static_cast<void>(expr))
// we also need something like std::ignore
// very good post by Raymond Chen: https://devblogs.microsoft.com/oldnewthing/20240329-00/?p=109592
// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2968r2.html

#ifdef IB_COMPILER_CLANG
#define IB_UNREACHABLE() __builtin_unreachable()
#elifdef IB_COMPILER_GCC
#define IB_UNREACHABLE() __builtin_unreachable()
#elifdef IB_COMPILER_MSVC
#define IB_UNREACHABLE() __assume(false)
#endif

// NOLINTEND(cppcoreguidelines-macro-usage)
