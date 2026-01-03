#define UNICODE
#define _UNICODE
#include <windows.h>
#include <string>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    
    // Get directory of exe
    std::wstring dir = exePath;
    size_t pos = dir.rfind(L'\\');
    if (pos != std::wstring::npos) dir = dir.substr(0, pos);
    
    // Set environment variables
    SetEnvironmentVariableW(L"XDG_CONFIG_HOME", dir.c_str());
    SetEnvironmentVariableW(L"XDG_DATA_HOME", (dir + L"\\data").c_str());
    SetEnvironmentVariableW(L"XDG_STATE_HOME", (dir + L"\\state").c_str());
    
    // Get command line args (file to open)
    std::wstring cmdLine = GetCommandLineW();
    std::wstring fileArg;
    
    // Skip exe name in command line
    size_t start = 0;
    if (cmdLine[0] == L'"') {
        start = cmdLine.find(L'"', 1);
        if (start != std::wstring::npos) start++;
    } else {
        start = cmdLine.find(L' ');
    }
    if (start != std::wstring::npos && start < cmdLine.length()) {
        fileArg = cmdLine.substr(start);
        // Trim leading spaces
        while (!fileArg.empty() && fileArg[0] == L' ') fileArg.erase(0, 1);
    }
    
    // Try neovide first
    wchar_t neovide[MAX_PATH];
    DWORD len = SearchPathW(NULL, L"neovide.exe", NULL, MAX_PATH, neovide, NULL);
    
    std::wstring cmd;
    if (len > 0) {
        cmd = L"\"" + std::wstring(neovide) + L"\" --neovim-bin nvim";
        if (!fileArg.empty()) cmd += L" " + fileArg;
    } else {
        // Fall back to nvim
        wchar_t nvim[MAX_PATH];
        len = SearchPathW(NULL, L"nvim.exe", NULL, MAX_PATH, nvim, NULL);
        if (len == 0) {
            MessageBoxW(NULL, L"Neovim not found!\n\nPlease install Neovim from https://neovim.io/\nor Neovide from https://neovide.dev/", L"DISP IDE", MB_ICONERROR);
            return 1;
        }
        cmd = L"\"" + std::wstring(nvim) + L"\"";
        if (!fileArg.empty()) cmd += L" " + fileArg;
    }
    
    // Launch
    STARTUPINFOW si = {sizeof(si)};
    PROCESS_INFORMATION pi;
    
    if (CreateProcessW(NULL, (LPWSTR)cmd.c_str(), NULL, NULL, FALSE, 0, NULL, dir.c_str(), &si, &pi)) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    
    return 0;
}
