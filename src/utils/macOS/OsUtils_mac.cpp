#include "OsUtils_mac.hpp"

#include <objc/objc.h>
#include <objc/message.h>
#include <objc/runtime.h>
#include <nfd.h>
#include <raylib.h>
#include <cstdlib>
#include <iostream>
#include <string>

namespace OsUtilsMac {
    using MsgFn  = id    (*)(id, SEL, ...);
    using MsgFnV = void  (*)(id, SEL, ...);
    using MsgFnB = BOOL  (*)(id, SEL, ...);
    using MsgFnD = double(*)(id, SEL, ...);

    static id msg(id obj, const char* sel, ...) {
        return reinterpret_cast<MsgFn>(objc_msgSend)(obj, sel_registerName(sel));
    }

    static id get_ns_app() {
        return reinterpret_cast<MsgFn>(objc_msgSend)(
            reinterpret_cast<id>(objc_getClass("NSApplication")),
            sel_registerName("sharedApplication"));
    }

    static id get_ns_window(void* handle) {
        if (handle) return static_cast<id>(handle);

        id app = get_ns_app();
        return reinterpret_cast<MsgFn>(objc_msgSend)(app, sel_registerName("mainWindow"));
    }

    static constexpr int kNSNormalWindowLevel      = 0;
    static constexpr int kNSFloatingWindowLevel    = 3;
    static constexpr int kNSScreenSaverWindowLevel = 1000;

    static constexpr unsigned long kCollectionCanJoinAllSpaces    = 1 << 0;
    static constexpr unsigned long kCollectionStationary          = 1 << 4;
    static constexpr unsigned long kCollectionIgnoresCycle        = 1 << 6;
    static constexpr unsigned long kCollectionFullScreenAuxiliary = 1 << 8;

    void setup_overlay(void* nativeHandle) {
        id win = get_ns_window(nativeHandle);
        if (!win) {
            std::cerr << "[OsUtils/Mac] NSWindow not found" << std::endl;
            return;
        }

        reinterpret_cast<MsgFnV>(objc_msgSend)(
            win, sel_registerName("setStyleMask:"), (unsigned long)0);

        reinterpret_cast<MsgFnV>(objc_msgSend)(
            win, sel_registerName("setLevel:"), kNSScreenSaverWindowLevel);

        unsigned long behavior = kCollectionCanJoinAllSpaces
                            | kCollectionStationary
                            | kCollectionIgnoresCycle
                            | kCollectionFullScreenAuxiliary;
        reinterpret_cast<MsgFnV>(objc_msgSend)(
            win, sel_registerName("setCollectionBehavior:"), behavior);

        reinterpret_cast<MsgFnV>(objc_msgSend)(
            win, sel_registerName("setOpaque:"), (BOOL)NO);

        id clearColor = reinterpret_cast<MsgFn>(objc_msgSend)(
            reinterpret_cast<id>(objc_getClass("NSColor")),
            sel_registerName("clearColor"));
        reinterpret_cast<MsgFnV>(objc_msgSend)(
            win, sel_registerName("setBackgroundColor:"), clearColor);

        reinterpret_cast<MsgFnV>(objc_msgSend)(
            win, sel_registerName("setIgnoresMouseEvents:"), (BOOL)NO);
        reinterpret_cast<MsgFnV>(objc_msgSend)(
            get_ns_app(), sel_registerName("setActivationPolicy:"), (long)1);
    }

    void keep_focus(void* nativeHandle) {
        id win = get_ns_window(nativeHandle);
        if (!win) return;

        id app = get_ns_app();
        reinterpret_cast<MsgFnV>(objc_msgSend)(
            app, sel_registerName("activateIgnoringOtherApps:"), (BOOL)YES);

        reinterpret_cast<MsgFnV>(objc_msgSend)(
            win, sel_registerName("makeKeyAndOrderFront:"), (id)nullptr);
        reinterpret_cast<MsgFnV>(objc_msgSend)(
            win, sel_registerName("orderFrontRegardless"));
    }

    void set_window_opacity(void* nativeHandle, float alpha) {
        id win = get_ns_window(nativeHandle);
        if (!win) return;

        reinterpret_cast<void(*)(id, SEL, double)>(objc_msgSend)(
            win, sel_registerName("setAlphaValue:"), static_cast<double>(alpha));
    }

    void set_click_through(void* nativeHandle, bool enabled) {
        id win = get_ns_window(nativeHandle);
        if (!win) return;

        reinterpret_cast<MsgFnV>(objc_msgSend)(
            win, sel_registerName("setIgnoresMouseEvents:"), (BOOL)(enabled ? YES : NO));
    }

    void set_show_in_taskbar(void* /*nativeHandle*/, bool show) {
        long policy = show ? 0L : 1L;
        reinterpret_cast<MsgFnV>(objc_msgSend)(
            get_ns_app(), sel_registerName("setActivationPolicy:"), policy);
    }

    float get_dpi_scale(void*) {
        Vector2 scale = GetWindowScaleDPI();
        return scale.x;
    }

    void set_window_always_on_top(void* h) {
        id win = get_ns_window(h);
        if (!win) return;
        reinterpret_cast<MsgFnV>(objc_msgSend)(
            win, sel_registerName("setLevel:"), kNSScreenSaverWindowLevel);
    }

    void force_show(void* h) {
        id win = get_ns_window(h);
        if (!win) return;
        reinterpret_cast<MsgFnV>(objc_msgSend)(
            win, sel_registerName("deminiaturize:"), (id)nullptr);
        keep_focus(h);
    }

    void set_overlay_mode(void* h, bool overlay) {
        if (overlay) {
            SetWindowState(FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_TOPMOST);
            setup_overlay(h);
        } else {
            ClearWindowState(FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_UNDECORATED);
        }
    }

    void set_window_borderless(void* h, bool borderless) {
        id win = get_ns_window(h);
        if (!win) return;
        unsigned long mask = borderless ? 0UL : 1UL;
        reinterpret_cast<MsgFnV>(objc_msgSend)(
            win, sel_registerName("setStyleMask:"), mask);
    }

    void make_window_ghost(void* h) {
        setup_overlay(h);
        set_click_through(h, true);
    }

    bool is_key_down_global(int raylibKey) {
        return IsKeyDown(raylibKey);
    }

    bool is_shift_pressed_global() {
        return IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
    }

    bool open_file_dialog(void*, std::string& outPath) {
        nfdchar_t* outPathC = nullptr;
        nfdfilteritem_t filters[] = { { "Image Files", "png,jpg,jpeg,bmp,tga" } };

        nfdresult_t result = NFD_OpenDialog(&outPathC, filters, 1, nullptr);

        if (result == NFD_OKAY) {
            outPath = outPathC;
            NFD_FreePath(outPathC);
            return true;
        }

        if (result == NFD_ERROR) {
            std::cerr << "[OsUtils/Mac] NFD Error: " << NFD_GetError() << std::endl;
        }

        return false;
    }

    std::string get_appdata_path() {
        const char* home = std::getenv("HOME");
        if (home && *home) return std::string(home) + "/Library/Application Support";
        return ".";
    }
}