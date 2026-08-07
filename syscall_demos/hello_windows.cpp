// No headers, no CRT, no STL.

using BOOL = int;
using DWORD = unsigned long;  // 32-bit on Windows
using UINT = unsigned int;    // 32-bit on Windows
using HANDLE = void*;
using LPVOID = void*;
using LPDWORD = DWORD*;

#if defined(_M_IX86) || defined(__i386__)
#define WINAPI __stdcall
#else
#define WINAPI
#endif

extern "C"
{
    __declspec(dllimport) HANDLE WINAPI GetStdHandle(DWORD nStdHandle);
    __declspec(dllimport) BOOL WINAPI GetConsoleMode(HANDLE hConsoleHandle, LPDWORD lpMode);

    __declspec(dllimport) BOOL WINAPI WriteConsoleW(HANDLE hConsoleOutput, const void* lpBuffer,
        DWORD nNumberOfCharsToWrite, LPDWORD lpNumberOfCharsWritten, LPVOID lpReserved);

    __declspec(dllimport) BOOL WINAPI WriteFile(HANDLE hFile, const void* lpBuffer, DWORD nNumberOfBytesToWrite,
        LPDWORD lpNumberOfBytesWritten, LPVOID lpOverlapped);

    __declspec(dllimport) __declspec(noreturn) void WINAPI ExitProcess(UINT uExitCode);
}

static_assert(sizeof(wchar_t) == 2, "Windows wchar_t must be UTF-16.");

static constexpr DWORD K_STD_OUTPUT_HANDLE = static_cast<DWORD>(-11);

static const wchar_t K_CONSOLE_TEXT[] = L"Hello from a freestanding Windows program.\r\n";

static constexpr DWORD K_CONSOLE_TEXT_LENGTH = static_cast<DWORD>((sizeof(K_CONSOLE_TEXT) / sizeof(K_CONSOLE_TEXT[0])) - 1);

static const char K_REDIRECTED_TEXT[] = "Hello from a freestanding Windows program.\r\n";

static constexpr DWORD K_REDIRECTED_TEXT_LENGTH = static_cast<DWORD>(sizeof(K_REDIRECTED_TEXT) - 1);

// This is the PE entry point. There is intentionally no main/wmain.
extern "C" void mainCRTStartup()
{
    HANDLE output = GetStdHandle(K_STD_OUTPUT_HANDLE);

    DWORD ignored = 0;
    DWORD console_mode = 0;
    BOOL wrote = 0;

    // Use Unicode console output when stdout is actually a console.
    if (GetConsoleMode(output, &console_mode) != 0)
    {
        wrote = WriteConsoleW(output, K_CONSOLE_TEXT, K_CONSOLE_TEXT_LENGTH, &ignored, nullptr);
    }

    // If stdout is redirected, WriteConsoleW does not work. The fallback
    // text is ASCII, therefore also valid UTF-8.
    if (wrote == 0)
    {
        WriteFile(output, K_REDIRECTED_TEXT, K_REDIRECTED_TEXT_LENGTH, &ignored, nullptr);
    }

    ExitProcess(0);
}
