#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <format>

// enumerate all processes and check the name
int find_process(const char* process_name) {
    PROCESSENTRY32 entry = { };
    entry.dwSize = sizeof(PROCESSENTRY32);

    // snap all processes currently runing
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (!Process32First(snapshot, &entry))
        throw std::exception("Failed to create PROCESSENTRY32W of first process in snapshot!");

    int pid = -1;

    do {
        if (strcmp(process_name, entry.szExeFile) == 0)
            pid = entry.th32ProcessID;

    } while (Process32Next(snapshot, &entry));

    CloseHandle(snapshot);

    if (pid == -1)
        throw std::exception("Failed to find process.");
    else
        return pid;
}

void walk_modules(int pid) {
    MODULEENTRY32 entry = { };
    entry.dwSize = sizeof(MODULEENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);

    if (!Module32First(snapshot, &entry))
        throw std::exception("Failed to create MODULEENTRY32 of first module in snapshot!");

    do {

        // dump information about the module

        std::cout << std::format("[{}] Base: 0x{:x}, Size: {}", entry.szModule, (int) entry.modBaseAddr, entry.modBaseSize) << std::endl;

    } while (Module32Next(snapshot, &entry));

    CloseHandle(snapshot);
}

int main(int argc, char* argv[]) {
    try {
        const char* process = argv[1];

        int pid = find_process(process);
        walk_modules(pid);
    }
    catch (const std::exception& ex) {
        std::cout << ex.what() << std::endl;
    }
}