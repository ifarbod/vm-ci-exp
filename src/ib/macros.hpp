// SPDX-FileCopyrightText: 2026 iFarbod
//
// SPDX-License-Identifier: LicenseRef-Proprietary

#pragma once

// NOLINTBEGIN(cppcoreguidelines-macro-usage)

// MARK: compiler

#ifdef __clang__
#define IB_COMPILER_CLANG 1
#else
#define IB_COMPILER_CLANG 0
#endif

#ifdef __GNUC__
#define IB_COMPILER_GCC 1
#else
#define IB_COMPILER_GCC 0
#endif

#ifdef _MSC_VER
#define IB_COMPILER_MSVC 1
#else
#define IB_COMPILER_MSVC 0
#endif

#if !IB_COMPILER_CLANG && !IB_COMPILER_GCC && !IB_COMPILER_MSVC
#define IB_COMPILER_UNKNOWN 1
#else
#define IB_COMPILER_UNKNOWN 0
#endif

#if IB_COMPILER_CLANG
#if __clang_major__ < 21
#error "Clang version 21 or higher is required"
#endif
#elif IB_COMPILER_GCC
#if __GNUC__ < 15
#error "GCC version 15 or higher is required"
#endif
#elif IB_COMPILER_MSVC
#if _MSC_VER < 1944
#error "MSVC version 19.44 or higher is required"
#endif
#endif

#define COMPILER(IB_FEATURE) (IB_COMPILER_##IB_FEATURE)

// MARK: os

#ifdef _WIN32
#define IB_OS_WINDOWS 1
#else
#define IB_OS_WINDOWS 0
#endif

#ifdef __ANDROID__
#define IB_OS_ANDROID 1
#else
#define IB_OS_ANDROID 0
#endif

#ifdef __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__
#define IB_OS_MACOS 1
#else
#define IB_OS_MACOS 0
#endif

#ifdef __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__
#define IB_OS_IOS 1
#else
#define IB_OS_IOS 0
#endif

#ifdef __linux__
#define IB_OS_LINUX 1
#else
#define IB_OS_LINUX 0
#endif

#ifdef __FreeBSD__
#define IB_OS_FREEBSD 1
#else
#define IB_OS_FREEBSD 0
#endif

#ifdef __OpenBSD__
#define IB_OS_OPENBSD 1
#else
#define IB_OS_OPENBSD 0
#endif

#ifdef __NetBSD__
#define IB_OS_NETBSD 1
#else
#define IB_OS_NETBSD 0
#endif

#ifdef __DragonFly__
#define IB_OS_DRAGONFLY 1
#else
#define IB_OS_DRAGONFLY 0
#endif

#ifdef __sun__
#define IB_OS_SOLARIS 1
#else
#define IB_OS_SOLARIS 0
#endif

#ifdef __OHOS__
#define IB_OS_OHOS 1
#else
#define IB_OS_OHOS 0
#endif

#if !IB_OS_WINDOWS && !IB_OS_ANDROID && !IB_OS_MACOS && !IB_OS_IOS && !IB_OS_LINUX && !IB_OS_FREEBSD && \
    !IB_OS_OPENBSD && !IB_OS_NETBSD && !IB_OS_DRAGONFLY && !IB_OS_SOLARIS && !IB_OS_OHOS
#define IB_OS_UNKNOWN 1
#else
#define IB_OS_UNKNOWN 0
#endif

#define OS(IB_FEATURE) (IB_OS_##IB_FEATURE)

// MARK: arch

// MARK: arch

#if defined(__amd64__) || defined(_M_AMD64)
#define IB_ARCH_AMD64 1
#else
#define IB_ARCH_AMD64 0
#endif

#if defined(__i386__) || defined(_M_IX86)
#define IB_ARCH_I386 1
#else
#define IB_ARCH_I386 0
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
#define IB_ARCH_AARCH64 1
#else
#define IB_ARCH_AARCH64 0
#endif

#if defined(__arm__) || defined(_M_ARM)
#define IB_ARCH_ARM 1
#else
#define IB_ARCH_ARM 0
#endif

#if defined(__powerpc64__) || defined(__ppc64__)
#define IB_ARCH_PPC64 1
#else
#define IB_ARCH_PPC64 0
#endif

#ifdef __mips64
#define IB_ARCH_MIPS64 1
#else
#define IB_ARCH_MIPS64 0
#endif

#if defined(__riscv) && __riscv_xlen == 64
#define IB_ARCH_RISCV64 1
#else
#define IB_ARCH_RISCV64 0
#endif

#ifdef __s390x__
#define IB_ARCH_S390X 1
#else
#define IB_ARCH_S390X 0
#endif

#ifdef __loongarch64
#define IB_ARCH_LOONGARCH64 1
#else
#define IB_ARCH_LOONGARCH64 0
#endif

#if !IB_ARCH_AMD64 && !IB_ARCH_I386 && !IB_ARCH_AARCH64 && !IB_ARCH_ARM && !IB_ARCH_PPC64 && !IB_ARCH_MIPS64 && \
    !IB_ARCH_RISCV64 && !IB_ARCH_S390X && !IB_ARCH_LOONGARCH64
#define IB_ARCH_UNKNOWN 1
#else
#define IB_ARCH_UNKNOWN 0
#endif

#define ARCH(IB_FEATURE) (IB_ARCH_##IB_FEATURE)

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

#ifdef IB_COMPILER_CLANG
#define IB_UNREACHABLE() __builtin_unreachable()
#elifdef IB_COMPILER_GCC
#define IB_UNREACHABLE() __builtin_unreachable()
#elifdef IB_COMPILER_MSVC
#define IB_UNREACHABLE() __assume(false)
#endif

// NOLINTEND(cppcoreguidelines-macro-usage)
