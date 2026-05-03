#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <dwmapi.h>
#include <shlobj_core.h>
#include <iostream>
#include <string>

#include <nfd.h>

#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "ole32.lib")

namespace OsUtilsWin {
    static HWND resolve_hwnd(void* handle) {
        return handle ? static_cast<HWND>(handle) : GetActiveWindow();
    }

    void setup_overlay(void* nativeHandle) {
        HWND hwnd = resolve_hwnd(nativeHandle);
        if (!hwnd) return;

        LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
        style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
        SetWindowLongPtrW(hwnd, GWL_STYLE, style);

        LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
        exStyle |= WS_EX_TOOLWINDOW;
        exStyle &= ~WS_EX_APPWINDOW;
        exStyle |= WS_EX_TOPMOST;
        SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle);

        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_FRAMECHANGED);

        MARGINS margins = {-1};
        DwmExtendFrameIntoClientArea(hwnd, &margins);
    }

    void set_click_through(void* nativeHandle, bool enabled) {
        HWND hwnd = resolve_hwnd(nativeHandle);
        if (!hwnd) return;
        
        LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
        if (enabled) {
            exStyle |= WS_EX_LAYERED | WS_EX_TRANSPARENT;
        } else {
            exStyle &= ~WS_EX_TRANSPARENT;
        }
        SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle);
    }

    void set_window_opacity(void* nativeHandle, float alpha) {
        HWND hwnd = resolve_hwnd(nativeHandle);
        if (!hwnd) return;
        LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
        exStyle |= WS_EX_LAYERED;
        SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle);

        SetLayeredWindowAttributes(hwnd, 0, (BYTE)(alpha * 255), LWA_ALPHA);
    }

    void force_show(void* nativeHandle) {
        HWND hwnd = resolve_hwnd(nativeHandle);
        if (!hwnd) return;

        if (IsIconic(hwnd)) {
            ShowWindow(hwnd, SW_RESTORE);
        }
        
        ShowWindow(hwnd, SW_SHOWNOACTIVATE);
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }

    void make_window_ghost(void* nativeHandle) {
        setup_overlay(nativeHandle);
        set_click_through(nativeHandle, true);
    }

    static int raylib_to_vk(int k) {
        if (k >= 290 && k <= 301) return VK_F1 + (k - 290);

        switch (k) {
            case 340: return VK_LSHIFT;
            case 344: return VK_RSHIFT;
            case 341: return VK_LCONTROL;
            case 345: return VK_RCONTROL;
            case 342: return VK_LMENU;
            case 346: return VK_RMENU;

            case 256: return VK_ESCAPE;
            case 257: return VK_RETURN;
            case 32:  return VK_SPACE;
            case 258: return VK_TAB;
            case 261: return VK_DELETE;
            case 260: return VK_INSERT;
            case 268: return VK_HOME;
            case 269: return VK_END;
            case 266: return VK_PRIOR;
            case 267: return VK_NEXT;

            case 263: return VK_LEFT;
            case 262: return VK_RIGHT;
            case 265: return VK_UP;
            case 264: return VK_DOWN;

            default:  return k;
        }
    }

    bool is_key_down_global(int raylibKey) {
        return (GetAsyncKeyState(raylib_to_vk(raylibKey)) & 0x8000) != 0;
    }

    bool is_shift_pressed_global() {
        return (GetAsyncKeyState(VK_LSHIFT) & 0x8000) != 0
            || (GetAsyncKeyState(VK_RSHIFT) & 0x8000) != 0;
    }

    std::string get_appdata_path() {
        PWSTR path = nullptr;
        if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &path))) {
            std::wstring wpath(path);
            CoTaskMemFree(path);
            if (wpath.empty()) return ".";
            int size_needed = WideCharToMultiByte(CP_UTF8, 0, wpath.c_str(), -1, nullptr, 0, nullptr, nullptr);
            std::string strTo(size_needed, 0);
            WideCharToMultiByte(CP_UTF8, 0, wpath.c_str(), -1, &strTo[0], size_needed, nullptr, nullptr);
            strTo.pop_back();
            return strTo;
        }
        return ".";
    }

    void keep_focus(void* nativeHandle) {
        HWND hwnd = resolve_hwnd(nativeHandle);
        if (!hwnd) return;

        HWND hCurWnd = GetForegroundWindow();
        if (hCurWnd == hwnd) return;

        DWORD dwMyId = GetCurrentThreadId();
        DWORD dwCurId = GetWindowThreadProcessId(hCurWnd, NULL);

        AttachThreadInput(dwCurId, dwMyId, TRUE);
        
        SetForegroundWindow(hwnd);
        SetFocus(hwnd);
        BringWindowToTop(hwnd);
        
        AttachThreadInput(dwCurId, dwMyId, FALSE);
    }

    void set_window_always_on_top(void* nativeHandle) {
        HWND hwnd = resolve_hwnd(nativeHandle);
        if (!hwnd) return;
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }

    void set_show_in_taskbar(void* nativeHandle, bool show) {
        HWND hwnd = resolve_hwnd(nativeHandle);
        if (!hwnd) return;

        LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
        if (show) {
            exStyle |= WS_EX_APPWINDOW;
            exStyle &= ~WS_EX_TOOLWINDOW;
        } else {
            exStyle &= ~WS_EX_APPWINDOW;
            exStyle |= WS_EX_TOOLWINDOW;
        }

        SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle);
        SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, 
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    }

    bool open_file_dialog(void*, std::string& outPath, std::string type) {
        nfdchar_t* outPathC = nullptr;
        nfdfilteritem_t* filters = nullptr;
        nfdfiltersize_t filterCount = 0;

        nfdfilteritem_t shaderFilters[] = { { "Shader Files", "frag,fs,glsl" } };
        nfdfilteritem_t imageFilters[] = { { "Image Files", "png,jpg,jpeg,bmp,tga" } };

        if (type == "Shaders") {
            filters = shaderFilters;
            filterCount = 1;
        } else if (type == "Images") {
            filters = imageFilters;
            filterCount = 1;
        }

        nfdresult_t result = NFD_OpenDialog(&outPathC, filters, filterCount, nullptr);
        
        if (result == NFD_OKAY) {
            outPath = outPathC;
            NFD_FreePath(outPathC);
            return true;
        }
        
        return false;
    }
}