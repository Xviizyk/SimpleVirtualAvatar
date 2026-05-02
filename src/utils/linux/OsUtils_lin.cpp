#include "OsUtils_lin.hpp"

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/shape.h>
#include <nfd.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

namespace OsUtilsLin {
    extern "C" {
        __attribute__((weak)) unsigned long GetX11Window();
    }

    static Display* get_display() {
        static Display* dpy = XOpenDisplay(nullptr);
        if (!dpy) {
            std::cerr << "[OsUtils/Lin] Cannot open X11 display" << std::endl;
        }
        return dpy;
    }

    static Window get_window(void* handle) {
        if (handle) return static_cast<Window>(reinterpret_cast<uintptr_t>(handle));

        if (GetX11Window) return static_cast<Window>(GetX11Window());

        Display* dpy = get_display();
        if (!dpy) return 0;
        Window focused;
        int    revert;
        XGetInputFocus(dpy, &focused, &revert);
        return focused;
    }

    static void set_net_atoms(Display* dpy, Window win,
                            const char* prop,
                            const char* atoms[], int count) {
        Atom propAtom = XInternAtom(dpy, prop, False);
        Atom* vals    = new Atom[count];
        for (int i = 0; i < count; i++)
            vals[i] = XInternAtom(dpy, atoms[i], False);
        XChangeProperty(dpy, win, propAtom, XA_ATOM, 32,
                        PropModeReplace,
                        reinterpret_cast<unsigned char*>(vals), count);
        delete[] vals;
    }

    static void send_wm_state(Display* dpy, Window win,
                            int action,
                            const char* atom1,
                            const char* atom2 = nullptr) {
        XEvent ev     = {};
        ev.type       = ClientMessage;
        ev.xclient.window       = win;
        ev.xclient.message_type = XInternAtom(dpy, "_NET_WM_STATE", False);
        ev.xclient.format       = 32;
        ev.xclient.data.l[0]    = action;
        ev.xclient.data.l[1]    = XInternAtom(dpy, atom1, False);
        ev.xclient.data.l[2]    = atom2 ? XInternAtom(dpy, atom2, False) : 0;
        ev.xclient.data.l[3]    = 1;

        XSendEvent(dpy, DefaultRootWindow(dpy), False,
                SubstructureNotifyMask | SubstructureRedirectMask, &ev);
        XFlush(dpy);
    }

    void setup_overlay(void* nativeHandle) {
        Display* dpy = get_display();
        Window   win = get_window(nativeHandle);
        if (!dpy || !win) return;

        {
            const char* types[] = { "_NET_WM_WINDOW_TYPE_DOCK" };
            set_net_atoms(dpy, win, "_NET_WM_WINDOW_TYPE", types, 1);
        }

        {
            const char* states[] = {
                "_NET_WM_STATE_ABOVE",
                "_NET_WM_STATE_SKIP_TASKBAR",
                "_NET_WM_STATE_SKIP_PAGER",
                "_NET_WM_STATE_STAYS_ON_TOP"
            };
            set_net_atoms(dpy, win, "_NET_WM_STATE", states, 4);
        }

        {
            struct MotifHints {
                unsigned long flags;
                unsigned long functions;
                unsigned long decorations;
                long          inputMode;
                unsigned long status;
            } hints = { 2, 0, 0, 0, 0 };

            Atom motif = XInternAtom(dpy, "_MOTIF_WM_HINTS", False);
            XChangeProperty(dpy, win, motif, motif, 32, PropModeReplace,
                            reinterpret_cast<unsigned char*>(&hints), 5);
        }

        send_wm_state(dpy, win, 1,
                    "_NET_WM_STATE_ABOVE",
                    "_NET_WM_STATE_SKIP_TASKBAR");
        send_wm_state(dpy, win, 1,
                    "_NET_WM_STATE_SKIP_PAGER",
                    "_NET_WM_STATE_STAYS_ON_TOP");

        XFlush(dpy);
    }

    void keep_focus(void* nativeHandle) {
        Display* dpy = get_display();
        Window   win = get_window(nativeHandle);
        if (!dpy || !win) return;

        XRaiseWindow(dpy, win);
        XSetInputFocus(dpy, win, RevertToPointerRoot, CurrentTime);
        XFlush(dpy);
    }

    void set_window_opacity(void* nativeHandle, float alpha) {
        Display* dpy = get_display();
        Window   win = get_window(nativeHandle);
        if (!dpy || !win) return;

        unsigned long opacity = static_cast<unsigned long>(alpha * 0xFFFFFFFFUL);
        Atom prop = XInternAtom(dpy, "_NET_WM_WINDOW_OPACITY", False);
        XChangeProperty(dpy, win, prop, XA_CARDINAL, 32,
                        PropModeReplace,
                        reinterpret_cast<unsigned char*>(&opacity), 1);
        XFlush(dpy);
    }

    void set_click_through(void* nativeHandle, bool enabled) {
        Display* dpy = get_display();
        Window   win = get_window(nativeHandle);
        if (!dpy || !win) return;

        if (enabled) {
            XRectangle rect = {};
            XShapeCombineRectangles(dpy, win, ShapeInput, 0, 0, &rect, 0,
                                    ShapeSet, Unsorted);
        } else {
            XShapeCombineMask(dpy, win, ShapeInput, 0, 0, None, ShapeSet);
        }
        XFlush(dpy);
    }

    void set_show_in_taskbar(void* nativeHandle, bool show) {
        Display* dpy = get_display();
        Window   win = get_window(nativeHandle);
        if (!dpy || !win) return;

        int action = show ? 0 : 1;
        send_wm_state(dpy, win, action,
                    "_NET_WM_STATE_SKIP_TASKBAR",
                    "_NET_WM_STATE_SKIP_PAGER");
    }


    void set_window_always_on_top(void* h) {
        Display* dpy = get_display();
        Window   win = get_window(h);
        if (!dpy || !win) return;
        send_wm_state(dpy, win, 1, "_NET_WM_STATE_ABOVE");
    }

    void force_show(void* h) {
        Display* dpy = get_display();
        Window   win = get_window(h);
        if (!dpy || !win) return;
        XMapRaised(dpy, win);
        keep_focus(h);
    }

    void make_window_ghost(void* h) {
        setup_overlay(h);
        set_click_through(h, true);
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
            std::cerr << "[OsUtils/Lin] NFD Error: " << NFD_GetError() << std::endl;
        }

        return false;
    }

    std::string get_appdata_path() {
        const char* xdg = std::getenv("XDG_CONFIG_HOME");
        if (xdg && *xdg) return xdg;

        const char* home = std::getenv("HOME");
        if (home && *home) return std::string(home) + "/.config";

        return ".";
    }
}