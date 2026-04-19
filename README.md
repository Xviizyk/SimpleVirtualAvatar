# Инструкция по установке SimpleVirtualAvatar

## Требования

### Windows (MinGW)
- CMake 3.24+
- MinGW-w64 (GCC 11.0+)
- Git

### Linux
- CMake 3.24+
- GCC/Clang с C++20 поддержкой
- Git
- Development tools: `build-essential`, `libx11-dev`, `libxrandr-dev`, `libxinerama-dev`, `libxcursor-dev`, `libxi-dev`, `libgl1-mesa-dev`

### macOS
- CMake 3.24+
- Clang (Xcode Command Line Tools)
- Git

## Установка зависимостей

### Windows (MinGW)

```bash
git clone https://github.com/raysan5/raylib.git external/raylib
git clone https://github.com/mackron/miniaudio.git external/miniaudio
```

### Linux

```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake git libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl1-mesa-dev

git clone https://github.com/raysan5/raylib.git external/raylib
git clone https://github.com/mackron/miniaudio.git external/miniaudio
```

### macOS

```bash
xcode-select --install

brew install cmake

git clone https://github.com/raysan5/raylib.git external/raylib
git clone https://github.com/mackron/miniaudio.git external/miniaudio
```

## Сборка проекта

### Windows (MinGW) 

```bash
mkdir build
cd build

cmake -G "YOUR_CMAKE_GENERATOR" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER="YOUR_C++_COMPILER" -DCMAKE_C_COMPILER="YOUR_C_COMPILER" ..
#Example: cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang ..

cmake --build . --config Release
```

Если используется специальный toolchain файл:

```bash
cmake -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=../mingw-toolchain.cmake -DCMAKE_BUILD_TYPE=Release ..
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

## Запуск

После успешной сборки исполняемый файл будет находиться в:
- **Windows**: `build/Release/SimpleVirtualAvatar.exe`
- **Linux**: `build/SimpleVirtualAvatar`
- **macOS**: `build/SimpleVirtualAvatar`

```bash
./SimpleVirtualAvatar
```

## Решение проблем

### Ошибка: "external/raylib not found"

Убедитесь, что инициализировали submodules:

```bash
git clone https://github.com/raysan5/raylib.git external/raylib
git clone https://github.com/mackron/miniaudio.git external/miniaudio
```

### Ошибка компиляции на Windows: "Rectangle redeclared"

Проверьте, что используется исправленный `Renderer.cpp` с правильным порядком включения заголовков.

### Ошибка: "OpenGL not found"

**Linux:**
```bash
sudo apt-get install libgl1-mesa-dev
```

**macOS:** Обычно встроена в Xcode Command Line Tools.

### Ошибка при загрузке изображений

Убедитесь, что директория `img/` находится в корректном месте относительно исполняемого файла, или измените пути в `main.cpp`.

## Структура проекта

```
SimpleVirtualAvatar/
├── CMakeLists.txt          # Конфигурация сборки
├── src/
│   ├── main.cpp            # Главная функция
│   ├── AudioInput.cpp/.hpp  # Захват аудио
│   ├── AudioHandler.cpp/.hpp # Обработка состояний
│   ├── Renderer.cpp/.hpp    # Рендеринг окна
│   └── miniaudio.cpp        # Реализация miniaudio
├── external/
│   ├── raylib/              # Графическая библиотека
│   └── miniaudio/           # Аудио библиотека
└── img/
    ├── idle.png
    ├── speaking.png
    ├── speaking_blink.png
    ├── idle_blink.png
    ├── scream.png
    └── scream_blink.png
```

## Дополнительные опции CMake

### Отладочная сборка
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
```
### С оптимизацией
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
```

### С информацией об отладке в Release
```bash
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
```
