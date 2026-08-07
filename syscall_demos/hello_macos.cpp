
#pragma clang diagnostic ignored "-Wmain-return-type"
#pragma clang diagnostic ignored "-Wreserved-identifier"
#pragma clang diagnostic ignored "-Wmissing-prototypes"

using usize = decltype(sizeof(nullptr));

using isize = decltype("" - "");

// macOS Unix System Call Numbers (Class 2: UNIX / BSD)
#define SYS_WRITE 0x2000004
#define SYS_EXIT 0x2000001

// Direct syscall wrapper for x86_64 (amd64)
#if defined(__x86_64__)
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

// Direct syscall wrapper for ARM64 (aarch64)
#elif defined(__aarch64__)
extern "C" isize sys_write(int fd, const void* buf, usize count)
{
    register isize x0 asm("x0") = fd;
    register const void* x1 asm("x1") = buf;
    register usize x2 asm("x2") = count;
    register isize x16 asm("x16") = SYS_WRITE;

    asm volatile("svc #0" : "+r"(x0) : "r"(x1), "r"(x2), "r"(x16) : "memory");
    return x0;
}

extern "C" [[noreturn]]
void sys_exit(int code)
{
    register isize x0 asm("x0") = code;
    register isize x16 asm("x16") = SYS_EXIT;

    asm volatile("svc #0" : : "r"(x0), "r"(x16) : "memory");
    __builtin_unreachable();
}
#endif

// Custom entry point replacing crt0.o / main
extern "C" [[noreturn]]
void _start()
{
    const char msg[] = "Hello from freestanding C++ on macOS!\n";

    // Write message to stdout (file descriptor 1)
    sys_write(1, msg, sizeof(msg) - 1);

    // Exit with status code 0
    sys_exit(0);
}
