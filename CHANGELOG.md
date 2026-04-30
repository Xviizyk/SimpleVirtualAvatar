# CHANGELOG

All notable changes to the **SimpleVirtualAvatar** project are documented in this file. This project adheres to Semantic Versioning.

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
