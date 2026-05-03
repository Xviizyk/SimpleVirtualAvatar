# CHANGELOG

All notable changes to the **SimpleVirtualAvatar** project are documented in this file. This project adheres to Semantic Versioning.

## [3.1.0] — 2026-05-03

### Added
*   **Shader Library System:** Introduced a new UI for managing multiple shaders[cite: 1].
    *   **Preset Library:** Quick access to standard presets: `Wave`, `RGB Shift`, `Scanlines`, and `CRT`[cite: 1].
    *   **Custom Shader Support:** Users can now import and save their own `.frag` files via a native file dialog[cite: 1].
    *   **Shader Management:** Added the ability to delete custom shader entries from the library[cite: 1].
*   **New "Wiggle" Animation:** Implemented a continuous sine-based idle movement separate from the state-triggered "Shake" system[cite: 1].
    *   Added configuration options for `wiggle_speed` and `wiggle_amplitude`[cite: 1].
*   **Dynamic Terminal Branding:** The CMake configuration now detects terminal width to display either a standard or wide-format ASCII logo[cite: 1].
*   **Example Shaders:** Added reference fragment shaders: `heat_distortion.frag`, `neon_pulse.frag`, and `swirl_vortex.frag`[cite: 1].

### Changed
*   **UI Decoupling:** Refactored the `Renderer` to use self-contained sub-panels. `ShakeSettings` and `ShaderEditor` now manage their own internal states[cite: 1].
*   **DPI Scaling Logic:** Improved UI responsiveness by applying the `last_dpi_scale` multiplier across all sub-menus and buttons[cite: 1].
*   **Config v2.1:** Updated the configuration schema to support wiggle parameters and shader path persistence[cite: 1].

### Fixed
*   **UI Input Passthrough:** Resolved an issue where dragging the avatar would trigger if the user was clicking inside a UI sub-window[cite: 1].
*   **Shader Memory Safety:** Improved `Renderer` and `ShaderEditor` destructors to ensure GPU resources are freed correctly upon exit[cite: 1].

## [3.0.0] — 2026-05-02

### Major Architectural Overhaul
* **Core Module Refactoring:** Conducted a comprehensive reorganization of the entire source tree into specialized logical modules: `core`, `graphics`, `system`, `ui`, and `utils` to enhance long-term maintainability.
* **Namespace Standardization:** Transitioned from CamelCase to snake_case for internal API methods across `ConfigManager`, `Engine`, and `Renderer` classes to align with modern C++ style guides.
* **Global OsUtils Abstraction:** Replaced legacy Windows-only utilities with a robust, platform-agnostic `OsUtils` layer. This includes dedicated implementations for:
    - **Windows:** Win32/DWM based window manipulation and AppData path resolution.
    - **Linux:** X11/Xext integration for transparent window support.
    - **macOS:** Native Cocoa framework linking for Apple-specific environment handling.

### Build System & Dependency Management
* **CMake Modernization:** Upgraded the minimum required version to **CMake 3.31** and implemented a sophisticated target-based build configuration.
* **CPM Integration:** Fully adopted `CPM.cmake` for dependency management, removing manual Git submodules for `raylib` and `miniaudio`.
* **Static Analysis & Safety:** Integrated a comprehensive `project_warnings` interface with strict compiler flags (`-Wall`, `-Wextra`, `-Wpedantic`) to ensure high code quality.
* **Branding & CLI:** Added a custom ASCII art logo output during the CMake configuration phase for improved developer experience.
* **Build Optimization:** - Implemented `GNUInstallDirs` for standard Unix installation paths.
    - Added conditional linking for system libraries like `dwmapi`, `X11`, and `AppKit` based on the target OS.
    - Optimized MSVC link options for GUI-only applications (removing the console window in Release builds).

### Graphics & UI Evolution
* **New Sidebar UI:** Replaced the legacy floating buttons with a sleek, semi-transparent sidebar and menu bar system.
* **Dynamic DPI scale change:** You can dynamicly change DPI scale. Made for improved user experience.
* **State Badge System:** Introduced a new "State Badge" UI component with interactive dots and color-coded statuses (Idle, Talk, Scream).
* **Enhanced Visual Feedback:**
    - Redesigned the volume bar with dynamic color transitions: `BAR_IDLE`, `BAR_TALK`, and `BAR_SCREAM` based on current audio levels.
    - Implemented a "No avatar loaded" placeholder state with high-contrast borders and text for easier debugging.
* **DPI Awareness:** Improved UI scaling logic to ensure labels, dividers, and buttons render sharply across different monitor scales.
* **Performance:** VSync is now enabled by default in Release builds (`FLAG_VSYNC_HINT`) to reduce GPU overhead and prevent screen tearing.

### Core Engine & Configuration
* **ConfigManager 2.0:** - The configuration system now uses platform-specific paths (e.g., `%APPDATA%` on Windows) for storing `config.json` and assets.
    - Added automated directory creation for `assets/` and `shaders/` folders on first launch.
* **Input Abstraction:** Standardized hardware key mapping through `src/system/Keys.hpp`, decoupling the engine from backend-specific input codes.
* **Shake System:** Refined the avatar "shake" effect with new trigger modes and duration controls linked to the configuration file.

### Changed
* **Language Standard:** Upgraded the project toolchain to target **C++20**, enabling advanced features like concepts and improved filesystem interactions.
* **Asset Loading:** Transitioned to absolute path resolution for assets to prevent loading failures when the executable is launched from different directories.
* **Cleanup:** Removed the Tracy Profiler integration from standard builds to minimize performance overhead in development.

### Fixed
* **Linux Build Stability:** Resolved critical X11 dependency issues that previously caused linker errors on Debian-based distributions.
* **Window Transparency:** Fixed a bug where the window background would occasionally flicker on Windows when resizing.
* **Memory Management:** Corrected potential leaks in the `Renderer` destructor by ensuring `CloseWindow()` is only called when a window context exists.

### Removed
* **Legacy Code:** Deleted `src/WinUtils.cpp` and related legacy headers in favor of the unified `OsUtils` library.
* **Submodules:** Removed `.gitmodules` and the `external/` directory to simplify the repository structure.

## [2.1.0] — 2026-04-30

### Added
* **Automated Package Management:** Integrated `CPM.cmake` as the primary dependency manager to streamline the acquisition and version control of third-party libraries.
* **Extended System Dialogs:** Integrated the `nativefiledialog-extended` library to provide a consistent, native file-picking experience across different operating systems.
* **Managed Dependency Migration:** Transitioned core libraries, including `raylib` (v6.0) and `miniaudio` (v0.11.25), from manual management to the automated CPM ecosystem.
* **Enhanced Platform Compatibility:** Improved macOS framework linking and optimized X11/OpenGL discovery logic for more reliable builds on Linux distributions.
* **Standardized Input Mapping:** Introduced `src/system/Keys.hpp` to provide a unified abstraction layer for hardware key codes and input handling.
* **Deployment Standards:** Implemented `GNUInstallDirs` support to ensure the build system follows standard installation paths and hierarchy conventions on Unix-like systems.

### Changed
* **Architectural Refactoring:** Conducted a comprehensive reorganization of the source tree into logical modules: `core`, `graphics`, `system`, `ui`, and `utils` for better maintainability.
* **Modern Language Standards:** Upgraded the project toolchain to target **C20** and **C++20**, enabling modern language features and improved type safety.
* **Cross-Platform Utility Abstraction:** Replaced the legacy Windows-specific `WinUtils` with a newly implemented `OsUtils` layer to ensure cross-platform functional parity.
* **Build System Modernization:** Completely rewrote `CMakeLists.txt` using a modern, target-based approach, including enhanced safety checks and modular configurations.
* **Performance Defaults:** Modified Release build configurations to enable `FLAG_VSYNC_HINT` by default, reducing screen tearing and optimizing CPU/GPU synchronization.

### Removed
* **Instrumentation Cleanup:** Removed Tracy Profiler integration from the core engine loop to reduce overhead in standard development builds.
* **Legacy Windows Dependencies:** Deprecated and removed `src/WinUtils.cpp/hpp` in favor of the new platform-agnostic implementation.

---

## [2.0.0] — 2026-03-15

### Added
* **CI/CD Infrastructure:** Introduced GitHub Actions workflows to automate continuous integration and testing for both Linux and Windows environments.
* **Automated Release Pipeline:** Implemented `release.yml` to automate the generation and distribution of production-ready binary assets.

### Changed
* **Dependency Strategy:** Shifted external library management to Git submodules to ensure build reproducibility and explicit version tracking.
* **Workflow Optimization:** Refined CI build steps to minimize execution time and improve developer feedback loops.

### Fixed
* **Linux Build Environment:** Resolved critical build failures in Linux CI environments by correctly configuring missing X11 and OpenGL header dependencies.