#include "OsUtils.hpp"

#if defined(_WIN32)
  #include "windows/OsUtils_win.hpp"
  namespace Impl = OsUtilsWin;

#elif defined(__APPLE__)
  #include "macos/OsUtils_mac.hpp"
  namespace Impl = OsUtilsMac;

#elif defined(__linux__)
  #include "linux/OsUtils_lin.hpp"
  namespace Impl = OsUtilsLin;

#else
  #error "Unsupported platform"
#endif

float OsUtils::get_dpi_scale(void* h) { return Impl::get_dpi_scale(h); }

void OsUtils::set_show_in_taskbar(void* h, bool show) { Impl::set_show_in_taskbar(h, show); }
void OsUtils::setup_overlay(void* h) { Impl::setup_overlay(h); }
void OsUtils::keep_focus(void* h) { Impl::keep_focus(h); }
void OsUtils::set_window_opacity(void* h, float a) { Impl::set_window_opacity(h, a); }
void OsUtils::set_click_through(void* h, bool e) { Impl::set_click_through(h, e); }
void OsUtils::set_window_always_on_top(void* h) { Impl::set_window_always_on_top(h); }
void OsUtils::force_show(void* h) { Impl::force_show(h); }
void OsUtils::set_overlay_mode(void* h, bool o) { Impl::set_overlay_mode(h, o); }
void OsUtils::set_window_borderless(void* h, bool b) { Impl::set_window_borderless(h, b); }
void OsUtils::make_window_ghost(void* h) { Impl::make_window_ghost(h); }

bool OsUtils::is_key_down_global(int k) { return Impl::is_key_down_global(k); }
bool OsUtils::is_shift_pressed_global() { return Impl::is_shift_pressed_global(); }
bool OsUtils::open_file_dialog(void* h, std::string& p, std::string t) { return Impl::open_file_dialog(h, p, t); }

std::string OsUtils::get_appdata_path() { return Impl::get_appdata_path(); }