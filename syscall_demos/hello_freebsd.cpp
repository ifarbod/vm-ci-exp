// Freestanding FreeBSD C++23 "hello world" using raw syscalls.
// No libc, no STL, no CRT.

// -----------------------------------------------------------------------------
// FreeBSD syscall numbers (stable ABI, see sys/sys/syscall.h)
// -----------------------------------------------------------------------------
inline constexpr long SYS_exit = 1;
inline constexpr long SYS_write = 4;

// -----------------------------------------------------------------------------
// Per-arch syscall wrapper. FreeBSD uses the same numbers on every arch;
// only the register/instruction convention differs.
// -----------------------------------------------------------------------------
#if defined(__amd64__)
static long fbsd_syscall3(long n, long a1, long a2, long a3)
{
    long ret;
    register long r10 __asm__("r10") = 0;
    register long r8 __asm__("r8") = 0;
    register long r9 __asm__("r9") = 0;
    __asm__ volatile("syscall"
        : "=a"(ret)
        : "0"(n), "D"(a1), "S"(a2), "d"(a3), "r"(r10), "r"(r8), "r"(r9)
        : "rcx", "r11", "memory");
    return ret;
}

#elif defined(__i386__)
static long fbsd_syscall3(long n, long a1, long a2, long a3)
{
    // FreeBSD i386 ABI: args passed on the stack (like a normal C call),
    // syscall number in %eax, invoked via `int $0x80`.
    long ret;
    __asm__ volatile("pushl %4\n\t"
                     "pushl %3\n\t"
                     "pushl %2\n\t"
                     "pushl %%eax\n\t"  // dummy return-addr slot
                     "int   $0x80\n\t"
                     "addl  $16, %%esp\n\t"
        : "=a"(ret)
        : "0"(n), "g"(a1), "g"(a2), "g"(a3)
        : "memory", "cc");
    return ret;
}

#elif defined(__aarch64__)
static long fbsd_syscall3(long n, long a1, long a2, long a3)
{
    register long x8 __asm__("x8") = n;
    register long x0 __asm__("x0") = a1;
    register long x1 __asm__("x1") = a2;
    register long x2 __asm__("x2") = a3;
    __asm__ volatile("svc #0" : "+r"(x0) : "r"(x8), "r"(x1), "r"(x2) : "memory", "cc");
    return x0;
}

#elif defined(__arm__)
static long fbsd_syscall3(long n, long a1, long a2, long a3)
{
    register long r7 __asm__("r7") = n;
    register long r0 __asm__("r0") = a1;
    register long r1 __asm__("r1") = a2;
    register long r2 __asm__("r2") = a3;
    __asm__ volatile("swi #0" : "+r"(r0) : "r"(r7), "r"(r1), "r"(r2) : "memory", "cc");
    return r0;
}

#elif defined(__powerpc__) || defined(__powerpc64__)
// Covers powerpc, powerpc64, powerpc64le, powerpcspe.
static long fbsd_syscall3(long n, long a1, long a2, long a3)
{
    register long r0 __asm__("r0") = n;
    register long r3 __asm__("r3") = a1;
    register long r4 __asm__("r4") = a2;
    register long r5 __asm__("r5") = a3;
    __asm__ volatile("sc"
        : "+r"(r3)
        : "r"(r0), "r"(r4), "r"(r5)
        : "memory", "cc", "r6", "r7", "r8", "r9", "r10", "r11", "r12");
    return r3;
}

#elif defined(__riscv) && (__riscv_xlen == 64)
static long fbsd_syscall3(long n, long a1, long a2, long a3)
{
    register long a7 __asm__("a7") = n;
    register long a0 __asm__("a0") = a1;
    register long a1r __asm__("a1") = a2;
    register long a2r __asm__("a2") = a3;
    __asm__ volatile("ecall" : "+r"(a0) : "r"(a7), "r"(a1r), "r"(a2r) : "memory");
    return a0;
}

#else
#error "Unsupported architecture"
#endif

// -----------------------------------------------------------------------------
// Tiny helpers
// -----------------------------------------------------------------------------
using usize = decltype(sizeof(nullptr));

using isize = decltype("" - "");

[[nodiscard]]
static constexpr usize str_len(const char* s)
{
    usize n = 0;
    while (s[n] != '\0')
    {
        ++n;
    }
    return n;
}

static long sys_write(int fd, const char* buf, usize len)
{
    return fbsd_syscall3(SYS_write, static_cast<long>(fd), reinterpret_cast<long>(buf), static_cast<long>(len));
}

[[noreturn]]
static void sys_exit(int code)
{
    fbsd_syscall3(SYS_exit, static_cast<long>(code), 0, 0);
    __builtin_unreachable();
}

// -----------------------------------------------------------------------------
// Entry point. We use `_start` and set -nostdlib so no CRT is needed.
// -----------------------------------------------------------------------------
extern "C" [[noreturn]]
void _start()
{
    constexpr const char msg[] = "Hello from freestanding FreeBSD C++23!\n";
    (void)sys_write(1, msg, str_len(msg));
    sys_exit(0);
}
