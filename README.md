# SimpleVirtualAvatar

![License](https://img.shields.io/badge/license-MIT-green)
![C++](https://img.shields.io/badge/C++-20-blue)
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey)

**SimpleVirtualAvatar** is a lightweight, cross-platform desktop application designed to render reactive 2D avatars. It tracks your system audio in real-time to animate an avatar based on volume thresholds, making it perfect for streamers and content creators who want a simple, high-performance alternative to complex VTubing software.

---

## 🌟 Key Features

*   **Real-time Audio Reactivity:** Avatar states change dynamically based on microphone or system volume.
*   **Cross-Platform Support:** Native performance on Windows (Win32/DWM), Linux (X11), and macOS (AppKit).
*   **Ultra-Lightweight:** Built with C++20 and Raylib for minimal CPU/GPU footprint.
*   **Transparent Windowing:** Support for transparent backgrounds to overlay the avatar on other apps or streams.
*   **Modern UI:** Sleek sidebar and state-tracking badge system for easy configuration.

## 🎭 States & Animations

The avatar transitions between three distinct states based on the detected volume level:

| Animation | Volume Threshold | Description |
|-----------|------------------|-------------|
| **Idle**  | 0.0 - 0.2        | Static or breathing animation when silent. |
| **Talk**  | 0.2 - 0.7        | Active talking animation. |
| **Scream**| 0.7 - 1.0        | High-volume reactive state. |

---

## 🛠 Tech Stack

| Component | Technology |
|-----------|------------|
| **Language** | C++20 |
| **Graphics** | [Raylib](https://www.raylib.com/) |
| **Audio** | [miniAudio](https://miniaud.io/) |
| **Build System** | CMake 3.31+ / CPM[cite: 1] |
| **UI** | Custom Modular UI system |

---

## 🚀 Getting Started

### Prerequisites

*   **CMake 3.31** or later
*   **Git**
*   **C++20 Compliant Compiler:** 
    *   Windows: Visual Studio 2022 (MSVC) or MinGW-w64 (GCC 11+)
    *   Linux: GCC or Clang
    *   macOS: Xcode Command Line Tools (Clang)

### 1. Clone the Repository
```bash
git clone https://github.com/Xviizyk/SimpleVirtualAvatar.git
cd SimpleVirtualAvatar
```
*Note: Dependencies like `raylib` and `miniaudio` are handled automatically via CPM during the build process[cite: 1, 2].*

### 2. Build the Project

#### Windows (PowerShell)
```powershell
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

#### Linux / macOS (Terminal)
```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j$(nproc)
```

### 3. Run
**Windows:** `build\Release\SimpleVirtualAvatar.exe`
**Linux/macOS:** `./build/SimpleVirtualAvatar`

---

## ⚙️ Configuration

On the first launch, the application creates a `config.json` and folders for `assets/` and `shaders/` in your platform's local application data directory:
*   **Windows:** `%APPDATA%/SimpleVirtualAvatar`
*   **Linux/macOS:** `~/.config/SimpleVirtualAvatar`

You can customize audio thresholds, shake effects, and file paths directly in the configuration file.

---

## 📊 Build Options

Toggle advanced features via CMake flags:
*   `-DENABLE_TRACY=ON`: Enable integration with the Tracy Profiler (development only).
*   `-DCROSS_COMPILE_ARM64=ON`: Optimize for ARM64 architectures.

---

## 📄 License

Distributed under the **MIT License**. See `LICENSE` for more information.

Copyright (c) 2026 **Xviizyk**

---

## 🤝 Contributing & Support

*   **Contributing:** No.
*   **Support:** For issues, questions, or suggestions, please don't send them.

> *Sorry for my English, it’s not my native language.*