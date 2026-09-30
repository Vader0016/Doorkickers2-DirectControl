#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <tlhelp32.h>

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace {

constexpr wchar_t kGameExecutableName[] = L"DoorKickers2.exe";
constexpr wchar_t kSteamLaunchUri[] = L"steam://run/1239080";

void ShowError(const std::wstring& message) {
    std::wcerr << message << L'\n';
    MessageBoxW(nullptr, message.c_str(), L"DK2 Direct Action",
                MB_OK | MB_ICONERROR);
}

std::filesystem::path ExecutableDirectory() {
    std::wstring buffer(32768, L'\0');
    const DWORD size = GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
    buffer.resize(size);
    return std::filesystem::path(buffer).parent_path();
}

std::filesystem::path FindGame(const wchar_t* explicitPath) {
    if (explicitPath && *explicitPath) {
        return std::filesystem::absolute(explicitPath);
    }

    const std::vector<std::filesystem::path> candidates = {
        LR"(C:\Program Files (x86)\Steam\steamapps\common\DoorKickers2\DoorKickers2.exe)",
        LR"(C:\Program Files\Steam\steamapps\common\DoorKickers2\DoorKickers2.exe)",
        LR"(D:\SteamLibrary\steamapps\common\DoorKickers2\DoorKickers2.exe)",
        LR"(E:\SteamLibrary\steamapps\common\DoorKickers2\DoorKickers2.exe)"
    };
    for (const auto& candidate : candidates) {
        if (std::filesystem::is_regular_file(candidate)) {
            return candidate;
        }
    }
    return {};
}

bool InjectDll(HANDLE process, const std::filesystem::path& dllPath) {
    const std::wstring dll = dllPath.wstring();
    const SIZE_T byteCount = (dll.size() + 1) * sizeof(wchar_t);
    void* remotePath = VirtualAllocEx(
        process, nullptr, byteCount, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!remotePath) {
        return false;
    }

    bool succeeded = false;
    bool remoteThreadTimedOut = false;
    if (WriteProcessMemory(process, remotePath, dll.c_str(), byteCount, nullptr)) {
        const HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
        const auto loadLibrary = reinterpret_cast<LPTHREAD_START_ROUTINE>(
            GetProcAddress(kernel32, "LoadLibraryW"));
        if (loadLibrary) {
            if (HANDLE thread = CreateRemoteThread(
                    process, nullptr, 0, loadLibrary, remotePath, 0, nullptr)) {
                const DWORD waitResult = WaitForSingleObject(thread, 10000);
                if (waitResult == WAIT_OBJECT_0) {
                    DWORD remoteModule = 0;
                    succeeded = GetExitCodeThread(thread, &remoteModule) && remoteModule != 0;
                } else if (waitResult == WAIT_TIMEOUT) {
                    remoteThreadTimedOut = true;
                }
                CloseHandle(thread);
            }
        }
    }

    // A timed-out LoadLibrary call may still be reading this path.
    if (!remoteThreadTimedOut) {
        VirtualFreeEx(process, remotePath, 0, MEM_RELEASE);
    }
    return succeeded;
}

DWORD FindRunningGame(const std::filesystem::path& gamePath) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }
    PROCESSENTRY32W entry{};
    entry.dwSize = sizeof(entry);
    DWORD matchingPid = 0;
    if (Process32FirstW(snapshot, &entry)) {
        do {
            if (_wcsicmp(entry.szExeFile, kGameExecutableName) != 0) {
                continue;
            }
            HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,
                                         FALSE, entry.th32ProcessID);
            if (!process) {
                continue;
            }
            std::wstring path(32768, L'\0');
            DWORD pathLength = static_cast<DWORD>(path.size());
            if (QueryFullProcessImageNameW(process, 0, path.data(), &pathLength)) {
                path.resize(pathLength);
                std::error_code error;
                if (std::filesystem::equivalent(gamePath, path, error) && !error) {
                    matchingPid = entry.th32ProcessID;
                    CloseHandle(process);
                    break;
                }
            }
            CloseHandle(process);
        } while (Process32NextW(snapshot, &entry));
    }
    CloseHandle(snapshot);
    return matchingPid;
}

bool AlreadyInjected(DWORD processId, const std::filesystem::path& dllPath) {
    HANDLE snapshot = CreateToolhelp32Snapshot(
        TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return false;
    }
    MODULEENTRY32W entry{};
    entry.dwSize = sizeof(entry);
    bool found = false;
    if (Module32FirstW(snapshot, &entry)) {
        do {
            std::error_code error;
            if (std::filesystem::equivalent(dllPath, entry.szExePath, error) &&
                !error) {
                found = true;
                break;
            }
        } while (Module32NextW(snapshot, &entry));
    }
    CloseHandle(snapshot);
    return found;
}

int AttachToRunningGame(const std::filesystem::path& gamePath,
                        const std::filesystem::path& dllPath,
                        bool launchThroughSteam) {
    DWORD processId = FindRunningGame(gamePath);
    if (launchThroughSteam && processId == 0) {
        const auto result = reinterpret_cast<INT_PTR>(ShellExecuteW(
            nullptr, L"open", kSteamLaunchUri, nullptr, nullptr, SW_SHOWNORMAL));
        if (result <= 32) {
            ShowError(L"Could not ask Steam to launch DK2 (ShellExecute result " +
                      std::to_wstring(result) + L").");
            return 3;
        }
        std::wcout << L"Waiting for DK2 to start through Steam...\n";
        const ULONGLONG deadline = GetTickCount64() + 90000;
        while (processId == 0 && GetTickCount64() < deadline) {
            Sleep(250);
            processId = FindRunningGame(gamePath);
        }
    }
    if (processId == 0) {
        ShowError(L"DK2 was not found at the expected executable path. "
                  L"Launch it through Steam first or pass its full path.");
        return 3;
    }
    if (AlreadyInjected(processId, dllPath)) {
        const std::wstring message =
            L"Direct Action is already loaded in DK2 (PID " +
            std::to_wstring(processId) +
            L"). Close the game completely and launch it again to load "
            L"a newly built version.";
        std::wcout << message << L'\n';
        MessageBoxW(nullptr, message.c_str(), L"DK2 Direct Action",
                    MB_OK | MB_ICONINFORMATION);
        return 0;
    }
    HANDLE process = OpenProcess(
        PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION |
            PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ,
        FALSE, processId);
    if (!process) {
        ShowError(L"Could not open DK2 (PID " + std::to_wstring(processId) +
                  L", error " + std::to_wstring(GetLastError()) + L").");
        return 4;
    }
    const bool injected = InjectDll(process, dllPath);
    const DWORD error = GetLastError();
    CloseHandle(process);
    if (!injected) {
        ShowError(L"DLL attach failed (error " + std::to_wstring(error) + L").");
        return 4;
    }
    std::wcout << L"Direct Control attached to DK2 (PID " << processId
               << L"). Press F6 in-game to enable it.\n";
    return 0;
}

} // namespace

int wmain(int argc, wchar_t** argv) {
    const bool directMode = argc > 1 && wcscmp(argv[1], L"--direct") == 0;
    const bool attachMode = argc > 1 && wcscmp(argv[1], L"--attach") == 0;
    const bool steamOption = argc > 1 && wcscmp(argv[1], L"--steam") == 0;
    const bool steamMode = !directMode && !attachMode;
    const int pathArgument = directMode || attachMode || steamOption ? 2 : 1;
    const wchar_t* explicitGamePath = argc > pathArgument
        ? argv[pathArgument] : nullptr;
    const std::filesystem::path loaderDir = ExecutableDirectory();
    const std::filesystem::path dllPath = loaderDir / L"dk2_direct_control.dll";
    const std::filesystem::path gamePath = FindGame(explicitGamePath);

    if (!std::filesystem::is_regular_file(dllPath)) {
        ShowError(L"DLL not found: " + dllPath.wstring());
        return 1;
    }
    if (gamePath.empty() || !std::filesystem::is_regular_file(gamePath)) {
        ShowError(L"DoorKickers2.exe was not found. Pass its full path "
                  L"after the launcher option.");
        return 2;
    }

    if (!directMode) {
        return AttachToRunningGame(gamePath,
                                   std::filesystem::absolute(dllPath), steamMode);
    }

    std::wstring commandLine = L"\"" + gamePath.wstring() + L"\"";
    std::vector<wchar_t> writableCommand(commandLine.begin(), commandLine.end());
    writableCommand.push_back(L'\0');

    STARTUPINFOW startup{};
    startup.cb = sizeof(startup);
    PROCESS_INFORMATION process{};
    const std::wstring workingDirectory = gamePath.parent_path().wstring();

    if (!CreateProcessW(
            gamePath.c_str(),
            writableCommand.data(),
            nullptr,
            nullptr,
            FALSE,
            CREATE_SUSPENDED,
            nullptr,
            workingDirectory.c_str(),
            &startup,
            &process)) {
        std::wcerr << L"Could not launch the game (error " << GetLastError() << L").\n";
        return 3;
    }

    const bool injected = InjectDll(process.hProcess, std::filesystem::absolute(dllPath));
    if (!injected) {
        std::wcerr << L"DLL injection failed (error " << GetLastError() << L"). The game was not started.\n";
        TerminateProcess(process.hProcess, 4);
        CloseHandle(process.hThread);
        CloseHandle(process.hProcess);
        return 4;
    }

    ResumeThread(process.hThread);
    CloseHandle(process.hThread);
    CloseHandle(process.hProcess);
    std::wcout << L"DK2 Direct Control loaded. Press F6 in-game to enable it.\n";
    return 0;
}
