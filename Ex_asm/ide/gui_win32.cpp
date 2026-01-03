// Minimal Win32 GUI prototype for DISP IDE
#include <windows.h>

static const wchar_t* CLASS_NAME = L"DispIDEClass";

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc;
        GetClientRect(hwnd, &rc);
        // Background
        HBRUSH bg = CreateSolidBrush(RGB(30,30,30));
        FillRect(hdc, &rc, bg);
        DeleteObject(bg);

        // Panels: explorer (left), editor (center), outline (right)
        int w = rc.right - rc.left;
        int h = rc.bottom - rc.top;
        int left_w = w * 20 / 100;
        int right_w = w * 18 / 100;
        RECT left = {rc.left, rc.top, rc.left + left_w, rc.bottom};
        RECT center = {rc.left + left_w, rc.top, rc.right - right_w, rc.bottom};
        RECT right = {rc.right - right_w, rc.top, rc.right, rc.bottom};

        HBRUSH panel = CreateSolidBrush(RGB(40,40,40));
        FillRect(hdc, &left, panel);
        FillRect(hdc, &center, CreateSolidBrush(RGB(24,24,24)));
        FillRect(hdc, &right, panel);

        // Draw sample file tree text in left
        SetTextColor(hdc, RGB(120,200,255));
        SetBkMode(hdc, TRANSPARENT);
        HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
        SelectObject(hdc, hFont);
        int y = 10;
        const wchar_t* tree[] = {L"[D] bin", L"[D] build", L"build.sh", L"examples", L"hello_test.dis", L"src", L"test.dis"};
        for (int i=0;i<7;i++) {
            TextOutW(hdc, left.left + 8, y, tree[i], (int)wcslen(tree[i]));
            y += 18;
        }

        // Editor sample: draw the screenshot-like header and code lines
        SetTextColor(hdc, RGB(220,220,220));
        const wchar_t* header = L"#!/bin/bash";
        TextOutW(hdc, center.left + 12, 12, header, (int)wcslen(header));
        const wchar_t* sample = L"echo \"Installing DISP Programming Language...\"";
        SetTextColor(hdc, RGB(200,150,60));
        TextOutW(hdc, center.left + 12, 36, sample, (int)wcslen(sample));

        // Outline panel
        SetTextColor(hdc, RGB(180,180,180));
        const wchar_t* outline[] = {L"Functions", L"- main", L"- build"};
        int oy = 12;
        for (int i=0;i<3;i++) {
            TextOutW(hdc, right.left + 8, oy, outline[i], (int)wcslen(outline[i]));
            oy += 18;
        }

        EndPaint(hwnd, &ps);
        return 0;
    }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"DISP IDE v2.0 - Prototype",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1000, 700,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) return 0;

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
