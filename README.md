# SimpleVirtualAvatar Setup Guide

## Requirements

### Windows (MSVC / MinGW)
- CMake 3.24+
- Visual Studio 2022 (Recommended) or MinGW-w64 (GCC 11.0+)
- Git

### Linux
- CMake 3.24+
- GCC/Clang with C++20 support
- Git
- Development tools: `build-essential`, `libx11-dev`, `libxrandr-dev`, `libxinerama-dev`, `libxcursor-dev`, `libxi-dev`, `libgl1-mesa-dev`

### macOS
- CMake 3.24+
- Clang (Xcode Command Line Tools)
- Git

## Dependencies Setup

### Cloning Libraries
Run these commands from the project root:

```bash
git clone https://github.com/raysan5/raylib.git external/raylib
git clone https://github.com/mackron/miniaudio.git external/miniaudio
```

### Linux Package Installation

```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake git libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl1-mesa-dev
```

### macOS Setup

```bash
xcode-select --install
brew install cmake
```

## Building the Project

### Windows (Visual Studio / Ninja)

```powershell
mkdir build
cd build

# For Visual Studio 2022
cmake -G "Visual Studio 17 2022" -A x64 ..

# Or for Clang/Ninja
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang ..

# Build command
cmake --build . --config Release
```

### Linux

```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j$(nproc)
```

### macOS

```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j$(sysctl -n hw.ncpu)
```

## Running the Application

Once built, the executable can be found in:
- **Windows**: `build/Release/SimpleVirtualAvatar.exe`
- **Linux/macOS**: `build/SimpleVirtualAvatar`

```bash
./SimpleVirtualAvatar
```

## Troubleshooting

### Error: "external/raylib not found"
Ensure you have cloned the dependencies into the `external/` folder as described in the Setup section.

### Windows Build Error: "Rectangle/LoadImage redeclared"
This is a conflict between Raylib and `windows.h`. Ensure your `Renderer.cpp` uses the fix with `#undef` or specific include ordering.

### Error: "OpenGL not found"
**Linux:** Install the mesa dev package: `sudo apt-get install libgl1-mesa-dev`.

### Asset Loading Issues
Verify that the `img/` directory is located correctly relative to the executable. Check the paths defined in `main.cpp`.

## Project Structure

```
SimpleVirtualAvatar/
├── CMakeLists.txt          # Build configuration
├── src/
│   ├── main.cpp            # Entry point
│   ├── AudioInput.cpp      # Audio capture (miniaudio wrapper)
│   ├── AudioHandler.cpp    # State logic & volume processing
│   ├── Renderer.cpp        # Raylib window & rendering logic
│   └── miniaudio.cpp       # Miniaudio implementation
├── external/
│   ├── raylib/             # Graphics library
│   └── miniaudio/          # Audio library
└── img/                    # Sprite assets
    ├── idle.png
    ├── speaking.png
    └── ...
```

## CMake Build Options

- **Debug Build:** `cmake -DCMAKE_BUILD_TYPE=Debug ..`
- **Release Build:** `cmake -DCMAKE_BUILD_TYPE=Release ..`
- **Release with Symbols:** `cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..`