#define UNICODE
#define _UNICODE
#include <windows.h>
#include <shellapi.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

static const wchar_t CLASS_NAME[]  = L"DisplexityInstallerWizard";
static HWND hLog;

void appendLog(const std::wstring &text) {
    int len = GetWindowTextLengthW(hLog);
    SendMessageW(hLog, EM_SETSEL, (WPARAM)len, (LPARAM)len);
    SendMessageW(hLog, EM_REPLACESEL, FALSE, (LPARAM)text.c_str());
}

int runEnvSetup(bool apply) {
    // Run tools\env_setup.exe with --dry-run or --apply and capture output to a file
    std::string exe = "tools\\env_setup.exe";
    std::string args = apply ? " --apply" : " --dry-run";
    std::string outFile = "tools\\installer_out.txt";

    // Build command: cmd /C "tools\env_setup.exe --dry-run > tools\installer_out.txt 2>&1"
    std::string cmd = "cmd /C \"" + exe + args + " > " + outFile + " 2>&1\"";

    appendLog(std::wstring(cmd.begin(), cmd.end()) + L"\r\n");
    int rc = std::system(cmd.c_str());

    // Read outFile (narrow) and append to log (convert to wide)
    std::ifstream fin(outFile);
    if (fin) {
        std::ostringstream ss;
        ss << fin.rdbuf();
        std::string s = ss.str();
        // convert to wide
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), NULL, 0);
        std::wstring ws(size_needed, 0);
        if (size_needed > 0) MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), &ws[0], size_needed);
        appendLog(ws);
    } else {
        appendLog(L"(no output file found)\r\n");
    }
    return rc;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        CreateWindowW(L"STATIC", L"Displexity Installer Wizard", WS_VISIBLE | WS_CHILD | SS_CENTER, 10, 10, 460, 20, hwnd, NULL, NULL, NULL);
        CreateWindowW(L"BUTTON", L"Dry Run", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 10, 40, 100, 30, hwnd, (HMENU)1, NULL, NULL);
        CreateWindowW(L"BUTTON", L"Install (apply)", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 120, 40, 140, 30, hwnd, (HMENU)2, NULL, NULL);
        CreateWindowW(L"BUTTON", L"Open MSYS2", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 270, 40, 100, 30, hwnd, (HMENU)3, NULL, NULL);
        CreateWindowW(L"BUTTON", L"Exit", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 380, 40, 80, 30, hwnd, (HMENU)4, NULL, NULL);

        hLog = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", NULL, WS_CHILD | WS_VISIBLE | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY | WS_VSCROLL, 10, 80, 460, 280, hwnd, NULL, NULL, NULL);
        appendLog(L"Welcome to Displexity Installer Wizard\r\n");
        appendLog(L"Use Dry Run to preview changes, Install to apply.\r\n\r\n");
        return 0;
    }
    case WM_COMMAND: {
        int id = LOWORD(wParam);
        if (id == 1) { // Dry run
            appendLog(L"--- Dry run started ---\r\n");
            runEnvSetup(false);
            appendLog(L"--- Dry run finished ---\r\n\r\n");
        } else if (id == 2) { // Install
            int ret = MessageBoxW(hwnd, L"This will run package manager commands which may require elevated privileges. Continue?", L"Confirm Install", MB_YESNO | MB_ICONQUESTION);
            if (ret == IDYES) {
                appendLog(L"--- Install started ---\r\n");
                runEnvSetup(true);
                appendLog(L"--- Install finished ---\r\n\r\n");
            }
        } else if (id == 3) { // Open MSYS2
            // Try to launch msys2 shell if available
            STARTUPINFOW si = { sizeof(si) };
            PROCESS_INFORMATION pi;
            std::wstring cmd = L"C:\\msys64\\usr\\bin\\bash.exe --login -i";
            if (!CreateProcessW(NULL, (LPWSTR)cmd.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
                MessageBoxW(hwnd, L"Could not open MSYS2 bash. Please run MSYS2 shell manually.", L"Info", MB_OK | MB_ICONINFORMATION);
            }
        } else if (id == 4) { // Exit
            PostQuitMessage(0);
        }
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow) {
    WNDCLASSW wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(0, CLASS_NAME, L"Displexity Installer Wizard", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 500, 420, NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = { };
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return 0;
}
