# Build Guide

## Prerequisites
- CMake >= 3.21
- Conan 2.x
- C compiler (clang or gcc)

## Setup Conan (project local)
```bash
python3 -m venv .venv
.venv/bin/pip install conan
```

`build.sh` will prefer `.venv/bin/conan` automatically and uses project-local `CONAN_HOME=.conan2`.

## Configure + Build (Release, static deps)
```bash
./build.sh
```

Outputs:
```text
build/Release/bin
build/Release/install/bin
```

## Debug Build
```bash
./build.sh Debug
```

Outputs:
```text
build/Debug/bin
build/Debug/install/bin
```

## Direct Commands
If you want to run steps manually:

```bash
conan profile detect --force
conan install . --output-folder build/Release/conan --build=missing -s build_type=Release
cmake -S . -B build/Release/cmake -DCMAKE_TOOLCHAIN_FILE=build/Release/conan/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build build/Release/cmake -j
cmake --install build/Release/cmake --prefix build/Release/install
```
