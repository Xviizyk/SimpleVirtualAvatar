# SimpleVirtualAvatar

![License](https://img.shields.io/badge/license-MIT-green)
![C++](https://img.shields.io/badge/C++-20-blue)
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey)

## States & Animations

The avatar has 3 different states:

| Animation | Volume |
|-----------|-----------|
| Idle | 0.0 - 0.2 |
| Talk | 0.2 - 0.7 |
| Scream | 0.7 - 1.0 |

## What was used

| Component | Technology |
|-----------|-----------|
| Graphics | [Raylib](https://www.raylib.com/) |
| Audio Input | [miniAudio](https://miniaud.io/) |
| Build System | CMake 4.0+ |
| Language | C++20 |
| Profiling (Optional) | [Tracy](https://github.com/wolfpld/tracy) |

## Requirements

### Windows (MSVC / MinGW)
- **CMake** 4.0 or later
- **Visual Studio 2022** (Recommended) or **MinGW-w64** (GCC 11.0+)
- **Git**

### Linux
- **CMake** 4.0 or later
- **GCC/Clang** with C++20 support
- **Git**
- Development packages: `build-essential`, `libx11-dev`, `libxrandr-dev`, `libxinerama-dev`, `libxcursor-dev`, `libxi-dev`, `libgl1-mesa-dev`

### macOS
- **CMake** 4.0 or later
- **Clang** (Xcode Command Line Tools)
- **Git**

## Getting started

### 1. Clone and setup dependencies

```bash
git clone https://github.com/Xviizyk/SimpleVirtualAvatar.git
cd SimpleVirtualAvatar

# Clone required external libraries
git clone https://github.com/raysan5/raylib.git external/raylib
git clone https://github.com/mackron/miniaudio.git external/miniaudio
```

### 2. Install platform-specific dependencies

#### Linux
```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake git libx11-dev libxrandr-dev \
    libxinerama-dev libxcursor-dev libxi-dev libgl1-mesa-dev
```

#### macOS
```bash
xcode-select --install
brew install cmake
```

#### Windows
Everything is already pre-installed in Visual Studio 2022

### 3. Build the project

#### Windows (Visual Studio)
```powershell
mkdir build
cd build
cmake -G "Visual Studio 17 2022" -A x64 ..
cmake --build . --config Release
```

#### Windows (Ninja/Clang)
```powershell
mkdir build
cd build
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release ^
  -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang ..
cmake --build .
```

#### Linux / macOS
```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j$(nproc)  # Linux
cmake --build . -j$(sysctl -n hw.ncpu)  # macOS
```

### 4. Run the application

**Windows:**
```bash
build\Release\SimpleVirtualAvatar.exe
```

**Linux/macOS:**
```bash
./build/SimpleVirtualAvatar
```

## Build options

```bash
# Debug build with symbols
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Release build (optimized)
cmake -DCMAKE_BUILD_TYPE=Release ..

# Release with debug info
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..

# Enable Tracy profiler (optional)
cmake -DENABLE_TRACY=ON ..

# Cross-compile for ARM64
cmake -DCROSS_COMPILE_ARM64=ON ..
```

## Optimizations

The CMakeLists.txt includes platform-specific optimizations:
- **MSVC**: `/Ot` (speed), `/GL` (whole program optimization), `/arch:AVX2`
- **GCC/Clang**: Standard optimization flags with ARM64 support for cross-compilation

## Profiling

To enable Tracy profiler integration:
```bash
cmake -DENABLE_TRACY=ON ..
```

Then connect with [Tracy Client](https://github.com/wolfpld/tracy) to profile real-time performance.

## License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE).

Copyright (c) 2026 Xviizyk

## Contributing

No.

## Support

For issues, questions, or suggestions, please don't send them.

## Asset Credits

Custom avatar sprites included. Feel free to replace with your own animated sprites.

---

Sorry for my English, it’s not my native language.