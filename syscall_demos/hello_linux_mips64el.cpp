// clang-format off
// mips64el-linux-gnuabi64-g++-16 -std=c++26 -O2 -g3 -Wall -Wextra -Wpedantic -Werror -Wconversion -Wsign-conversion -Wshadow -Wformat=2 -ffreestanding -fno-exceptions -fno-rtti -fno-stack-protector -fno-pic -mno-abicalls -nostdlib -nostartfiles -nodefaultlibs -no-pie -Wl,-static -Wl,-e,_start hello.cpp -o hello
// clang-format on

using usize = decltype(sizeof(nullptr));

using isize = decltype("" - "");

namespace linux_mips64_n64
{

constexpr usize sys_write = 5001;
constexpr usize sys_exit = 5058;

constexpr usize stdout_fd = 1;

[[nodiscard]]
inline isize syscall3(usize number, usize arg0, usize arg1, usize arg2)
{
    register usize v0 asm("$2") = number;
    register usize a0 asm("$4") = arg0;
    register usize a1 asm("$5") = arg1;
    register usize a2 asm("$6") = arg2;

    asm volatile("syscall" : "+r"(v0) : "r"(a0), "r"(a1), "r"(a2) : "$1", "$3", "$7", "memory");

    return static_cast<isize>(v0);
}

[[noreturn]]
inline void syscall_exit(usize status)
{
    register usize v0 asm("$2") = sys_exit;
    register usize a0 asm("$4") = status;

    asm volatile("syscall" : : "r"(v0), "r"(a0) : "$1", "$3", "$7", "memory");

    __builtin_unreachable();
}

[[nodiscard]]
inline isize write(usize fd, const void* buffer, usize length)
{
    return syscall3(sys_write, fd, reinterpret_cast<usize>(buffer), static_cast<usize>(length));
}

}  // namespace linux_mips64_n64

extern "C" [[noreturn]]
void _start()
{
    static constexpr char message[] = "Hello from direct MIPS64EL Linux syscalls.\n";

    constexpr usize message_length = sizeof(message) - 1U;

    static_cast<void>(linux_mips64_n64::write(linux_mips64_n64::stdout_fd, message, message_length));

    linux_mips64_n64::syscall_exit(0);
}
