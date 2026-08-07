// OpenIndiana (Solaris/Illumos like)
// --target=x86_64-pc-solaris2.11
// --target=sparc64-solaris2.11

#pragma clang diagnostic ignored "-Wmain-return-type"
#pragma clang diagnostic ignored "-Wreserved-identifier"
#pragma clang diagnostic ignored "-Wmissing-prototypes"

#ifndef __sun__
#error "not sun"
#endif

using usize = decltype(sizeof(nullptr));

using isize = decltype("" - "");

// Illumos / Solaris system call numbers (from <sys/syscall.h>)
constexpr auto SYS_WRITE = 4;
constexpr auto SYS_EXIT = 1;

// ---------------------------------------------------------------------------
// AMD64 (x86_64)
//
// Illumos uses the AMD64 SysV syscall ABI:
//   args:   %rdi, %rsi, %rdx, %r10, %r8, %r9
//   number: %rax
//   trap:   syscall
//   error:  carry flag (CF) set -> %rax holds errno
//   clobbers: %rcx, %r11
// ---------------------------------------------------------------------------
#if defined(__x86_64__) || defined(__amd64__)
extern "C" isize sys_write(int fd, const void* buf, usize count)
{
    isize ret;
    asm volatile("movq %1, %%rdi\n\t"
                 "movq %2, %%rsi\n\t"
                 "movq %3, %%rdx\n\t"
                 "movq %4, %%rax\n\t"
                 "syscall"
        : "=a"(ret)
        : "g"(static_cast<isize>(fd)), "g"(buf), "g"(count), "i"(SYS_WRITE)
        : "rdi", "rsi", "rdx", "rcx", "r11", "memory");
    return ret;
}

extern "C" [[noreturn]]
void sys_exit(int code)
{
    asm volatile("movq %0, %%rdi\n\t"
                 "movq %1, %%rax\n\t"
                 "syscall"
        :
        : "g"(static_cast<isize>(code)), "i"(SYS_EXIT)
        : "rdi", "memory");
    __builtin_unreachable();
}

// ---------------------------------------------------------------------------
// SPARC64 (sparcv9)
//
// Illumos 64-bit SPARC syscall ABI:
//   args:   %o0, %o1, %o2, %o3, %o4, %o5
//   number: %g1
//   trap:   ta 0x40   (ST_SYSCALL for the 64-bit / LP64 trap)
//   error:  carry flag (icc/xcc C) set -> %o0 holds errno
//   result: %o0
// ---------------------------------------------------------------------------
#elif defined(__sparcv9) || defined(__sparc64__) || (defined(__sparc__) && defined(__arch64__))
extern "C" isize sys_write(int fd, const void* buf, usize count)
{
    register isize o0 asm("o0") = fd;
    register const void* o1 asm("o1") = buf;
    register usize o2 asm("o2") = count;
    register isize g1 asm("g1") = SYS_WRITE;

    asm volatile("ta 0x40" : "+r"(o0) : "r"(o1), "r"(o2), "r"(g1) : "cc", "memory");
    return o0;
}

extern "C" [[noreturn]]
void sys_exit(int code)
{
    register isize o0 asm("o0") = code;
    register isize g1 asm("g1") = SYS_EXIT;

    asm volatile("ta 0x40" : : "r"(o0), "r"(g1) : "cc", "memory");
    __builtin_unreachable();
}
#else
#error "Unsupported architecture for Illumos port (only amd64 and sparc64)"
#endif

// Custom entry point replacing crt1.o / main
extern "C" [[noreturn]]
void _start()
{
    const char msg[] = "Hello from freestanding C++ on OpenIndiana!\n";

    // Write message to stdout (file descriptor 1)
    sys_write(1, msg, sizeof(msg) - 1);

    // Exit with status code 0
    sys_exit(0);
}
