/*
 cl /LD gvimfullscreen.c user32.lib
 ------------------------------
 :call libcallnr("gvimfullscreen.dll", "EnableFullScreen", 1)
*/
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam);
BOOL CALLBACK FindWindowProc(HWND hwnd, LPARAM lParam);

struct WindowInfo;
void ReadWindowInfo(char* serialized_state, WindowInfo* window_info);
void WriteWindowInfo(WindowInfo* window_info);

struct WindowInfo {
    BOOL maximized;
    BOOL fullscreen;
    LONG style;
    LONG exstyle;
    RECT rect;
};

// string buffer to return back to vim with the state information to persist
char g_serialized_window_info[2048] = "";

extern "C"
__declspec(dllexport)
char* ToggleFullScreen(char* prev_state) {
    HWND hwnd = NULL;
    DWORD dwThreadID = GetCurrentThreadId();

    EnumThreadWindows(dwThreadID, FindWindowProc, (LPARAM)&hwnd);

    WindowInfo window_info;
    ReadWindowInfo(prev_state, &window_info);

    if (hwnd) {
        if (!window_info.fullscreen) {
            // Save original window state, position, and size
            window_info.maximized = IsZoomed(hwnd);
            window_info.style = GetWindowLong(hwnd, GWL_STYLE);
            window_info.exstyle = GetWindowLong(hwnd, GWL_EXSTYLE);
            GetWindowRect(hwnd, &window_info.rect);

            // Get the monitor it is on
            MONITORINFO mi;
            mi.cbSize = sizeof(MONITORINFO);
            GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST),
                           &mi);

            // Fullscreen it
            if (window_info.maximized) {
                // Restore the window if it was maximized first, because
                // Windows seems to have trouble hiding the taskbar when
                // changing the size of a maximized window.
                SendMessage(hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
            }
            SetWindowLong(hwnd, GWL_STYLE,
                          window_info.style & ~(WS_CAPTION |
                              WS_THICKFRAME));
            SetWindowLong(hwnd, GWL_EXSTYLE,
                          window_info.exstyle & ~(WS_EX_DLGMODALFRAME |
                              WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE |
                              WS_EX_STATICEDGE));
            SetWindowPos(hwnd, NULL,
                         mi.rcMonitor.left,
                         mi.rcMonitor.top,
                         mi.rcMonitor.right - mi.rcMonitor.left,
                         mi.rcMonitor.bottom - mi.rcMonitor.top,
                         SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

            // Now need to find the child text area window
            // and set it's size accordingly
            EnumChildWindows(hwnd, EnumChildProc, 0);

            window_info.fullscreen = TRUE;
        } else {
            // Already full screen, so restore all the previous styles
            SetWindowLong(hwnd, GWL_STYLE, window_info.style);
            SetWindowLong(hwnd, GWL_EXSTYLE, window_info.exstyle);
            SetWindowPos(hwnd, NULL,
                         window_info.rect.left,
                         window_info.rect.top,
                         window_info.rect.right - window_info.rect.left,
                         window_info.rect.bottom - window_info.rect.top,
                         SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
            if (window_info.maximized) {
                SendMessage(hwnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
            }
            window_info.fullscreen = FALSE;
        }
    }

    WriteWindowInfo(&window_info);
    return g_serialized_window_info;
}

BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam) {
    char lpszClassName[100];
    HBRUSH b;

    UNREFERENCED_PARAMETER(lParam);

    GetClassName(hwnd, lpszClassName, 100);
    if (strcmp(lpszClassName, "VimTextArea") == 0) {
        RECT rect;
        GetWindowRect(GetParent(hwnd), &rect);
        //SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_EX_CLIENTEDGE);
        //SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_EX_WINDOWEDGE);
        SetWindowPos(hwnd, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW | SWP_NOZORDER);

        b = CreateSolidBrush(RGB(0,0,0));
        SetClassLongPtr(GetParent(hwnd), GCLP_HBRBACKGROUND, (LONG_PTR) b);
    }
    return TRUE;
}

BOOL CALLBACK FindWindowProc(HWND hwnd, LPARAM lParam) {
    HWND* pphWnd = (HWND*)lParam;

    if (GetParent(hwnd)) {
        *pphWnd = NULL;
        return TRUE;
    }

    *pphWnd = hwnd;
    return FALSE;
}

void ReadWindowInfo(char* serialized_state, WindowInfo* window_info) {
    ZeroMemory(window_info, sizeof(WindowInfo));

    if (!serialized_state || !serialized_state[0]) {
        window_info->fullscreen = FALSE;
        return;
    }

    sscanf_s(serialized_state,
             "%d,%d,%d,%d,%d,%d,%d,%d",
             &window_info->fullscreen,
             &window_info->maximized,
             &window_info->style,
             &window_info->exstyle,
             &window_info->rect.left,
             &window_info->rect.top,
             &window_info->rect.right,
             &window_info->rect.bottom);
}

void WriteWindowInfo(WindowInfo* window_info) {
    sprintf_s(g_serialized_window_info,
              "%d,%d,%d,%d,%d,%d,%d,%d",
              window_info->fullscreen,
              window_info->maximized,
              window_info->style,
              window_info->exstyle,
              window_info->rect.left,
              window_info->rect.top,
              window_info->rect.right,
              window_info->rect.bottom);
}
