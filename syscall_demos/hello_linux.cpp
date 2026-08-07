
#if !defined(__linux__)
#error "This source is for Linux only."
#endif

static long sys_write(int fd, const void* buf, unsigned long count);
static void sys_exit(int code);

// amd64 / x86_64: write=1, exit=60
#if defined(__x86_64__)

static long sys_write(int fd, const void* buf, unsigned long count)
{
    register long rax __asm__("rax") = 1;
    register long rdi __asm__("rdi") = (long)fd;
    register const void* rsi __asm__("rsi") = buf;
    register unsigned long rdx __asm__("rdx") = count;

    __asm__ volatile("syscall" : "+r"(rax) : "r"(rdi), "r"(rsi), "r"(rdx) : "rcx", "r11", "memory", "cc");

    return rax;
}

static void sys_exit(int code)
{
    register long rax __asm__("rax") = 60;
    register long rdi __asm__("rdi") = (long)code;

    __asm__ volatile("syscall" : : "r"(rax), "r"(rdi) : "rcx", "r11", "memory", "cc");

    __builtin_unreachable();
}

// i686 / x86 32-bit: write=4, exit=1
#elif defined(__i386__)

static long sys_write(int fd, const void* buf, unsigned long count)
{
    register long eax __asm__("eax") = 4;
    register long ebx __asm__("ebx") = (long)fd;
    register const void* ecx __asm__("ecx") = buf;
    register unsigned long edx __asm__("edx") = count;

    __asm__ volatile("int $0x80" : "+r"(eax) : "r"(ebx), "r"(ecx), "r"(edx) : "memory", "cc");

    return eax;
}

static [[noreturn]] void sys_exit(int code)
{
    register long eax __asm__("eax") = 1;
    register long ebx __asm__("ebx") = (long)code;

    __asm__ volatile("int $0x80" : : "r"(eax), "r"(ebx) : "memory", "cc");

    __builtin_unreachable();
}

// ARM64 / AArch64: write=64, exit=93
#elif defined(__aarch64__)

static long sys_write(int fd, const void* buf, unsigned long count)
{
    register long x0 __asm__("x0") = (long)fd;
    register const void* x1 __asm__("x1") = buf;
    register unsigned long x2 __asm__("x2") = count;
    register long x8 __asm__("x8") = 64;

    __asm__ volatile("svc #0" : "+r"(x0) : "r"(x1), "r"(x2), "r"(x8) : "memory");

    return x0;
}

static [[noreturn]] void sys_exit(int code)
{
    register long x0 __asm__("x0") = (long)code;
    register long x8 __asm__("x8") = 93;

    __asm__ volatile("svc #0" : : "r"(x0), "r"(x8) : "memory");

    __builtin_unreachable();
}

// ARMv7 EABI: write=4, exit=1
#elif defined(__arm__)

static long sys_write(int fd, const void* buf, unsigned long count)
{
    register long r0 __asm__("r0") = (long)fd;
    register const void* r1 __asm__("r1") = buf;
    register unsigned long r2 __asm__("r2") = count;
    register long r7 __asm__("r7") = 4;

    __asm__ volatile("svc 0" : "+r"(r0) : "r"(r1), "r"(r2), "r"(r7) : "memory", "cc");

    return r0;
}

static [[noreturn]] void sys_exit(int code)
{
    register long r0 __asm__("r0") = (long)code;
    register long r7 __asm__("r7") = 1;

    __asm__ volatile("svc 0" : : "r"(r0), "r"(r7) : "memory", "cc");

    __builtin_unreachable();
}

// MIPS64 little-endian, n64 ABI:
// __NR_Linux=5000, write=5001, exit=5058
#elif defined(__mips__) && defined(__mips64) && defined(__MIPSEL__)

static long sys_write(int fd, const void* buf, unsigned long count)
{
    register long v0 __asm__("$2") = 5001;
    register long a0 __asm__("$4") = (long)fd;
    register const void* a1 __asm__("$5") = buf;
    register unsigned long a2 __asm__("$6") = count;
    register long a3 __asm__("$7");

    __asm__ volatile("syscall" : "+r"(v0), "=r"(a3) : "r"(a0), "r"(a1), "r"(a2) : "memory");

    return v0;
}

static [[noreturn]] void sys_exit(int code)
{
    register long v0 __asm__("$2") = 5058;
    register long a0 __asm__("$4") = (long)code;

    __asm__ volatile("syscall" : : "r"(v0), "r"(a0) : "memory");

    __builtin_unreachable();
}

// PowerPC64 little-endian: write=4, exit=1
#elif defined(__powerpc64__) && defined(__LITTLE_ENDIAN__)

static long sys_write(int fd, const void* buf, unsigned long count)
{
    register long r0 __asm__("r0") = 4;
    register long r3 __asm__("r3") = (long)fd;
    register const void* r4 __asm__("r4") = buf;
    register unsigned long r5 __asm__("r5") = count;

    __asm__ volatile("sc" : "+r"(r3) : "r"(r0), "r"(r4), "r"(r5) : "memory", "cc");

    return r3;
}

static [[noreturn]] void sys_exit(int code)
{
    register long r0 __asm__("r0") = 1;
    register long r3 __asm__("r3") = (long)code;

    __asm__ volatile("sc" : : "r"(r0), "r"(r3) : "memory", "cc");

    __builtin_unreachable();
}

// RISC-V 64: write=64, exit=93
#elif defined(__riscv) && (__riscv_xlen == 64)

static long sys_write(int fd, const void* buf, unsigned long count)
{
    register long a0 __asm__("a0") = (long)fd;
    register const void* a1 __asm__("a1") = buf;
    register unsigned long a2 __asm__("a2") = count;
    register long a7 __asm__("a7") = 64;

    __asm__ volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2), "r"(a7) : "memory");

    return a0;
}

static [[noreturn]] void sys_exit(int code)
{
    register long a0 __asm__("a0") = (long)code;
    register long a7 __asm__("a7") = 93;

    __asm__ volatile("ecall" : : "r"(a0), "r"(a7) : "memory");

    __builtin_unreachable();
}

// IBM Z / s390x: write=4, exit=1
#elif defined(__s390x__)

static long sys_write(int fd, const void* buf, unsigned long count)
{
    register long r1 __asm__("r1") = 4;
    register long r2 __asm__("r2") = (long)fd;
    register const void* r3 __asm__("r3") = buf;
    register unsigned long r4 __asm__("r4") = count;

    __asm__ volatile("svc 0" : "+r"(r2) : "r"(r1), "r"(r3), "r"(r4) : "memory", "cc");

    return r2;
}

static [[noreturn]] void sys_exit(int code)
{
    register long r1 __asm__("r1") = 1;
    register long r2 __asm__("r2") = (long)code;

    __asm__ volatile("svc 0" : : "r"(r1), "r"(r2) : "memory", "cc");

    __builtin_unreachable();
}

// LoongArch64: write=64, exit=93
#elif defined(__loongarch64)

static long sys_write(int fd, const void* buf, unsigned long count)
{
    register long a0 __asm__("a0") = (long)fd;
    register const void* a1 __asm__("a1") = buf;
    register unsigned long a2 __asm__("a2") = count;
    register long a7 __asm__("a7") = 64;

    __asm__ volatile("syscall 0" : "+r"(a0) : "r"(a1), "r"(a2), "r"(a7) : "memory");

    return a0;
}

static [[noreturn]] void sys_exit(int code)
{
    register long a0 __asm__("a0") = (long)code;
    register long a7 __asm__("a7") = 93;

    __asm__ volatile("syscall 0" : : "r"(a0), "r"(a7) : "memory");

    __builtin_unreachable();
}

#else
#error "Unsupported Linux architecture."
#endif

static const char message[] = "Hello from freestanding C++ on Linux!\n";

extern "C" __attribute__((noreturn, used)) void _start()
{
    (void)sys_write(1, message, sizeof(message) - 1);
    sys_exit(0);
}
