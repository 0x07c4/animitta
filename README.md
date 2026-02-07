# animitta

`animitta` 是一个用 C 实现的图形化终端实验项目，当前主程序为 `cuterm`。

## Build System

项目使用 `Conan 2 + CMake`，默认通过 `build.sh` 完成依赖安装、配置、编译和安装。

### Prerequisites

- `cmake >= 3.21`
- `python3` (用于安装 Conan)
- `clang` 或 `gcc`

### Setup Conan (project local)

```bash
python3 -m venv .venv
.venv/bin/pip install conan
```

`build.sh` 会优先使用 `.venv/bin/conan`，并默认设置 `CONAN_HOME=.conan2`（项目内本地 Conan 缓存）。

### Build

默认 `Release`：

```bash
./build.sh
```

构建 `Debug`：

```bash
./build.sh Debug
```

产物目录：

- `build/<BuildType>/bin`
- `build/<BuildType>/install/bin`

### Run

```bash
./build/Release/bin/cuterm
```

或安装后运行：

```bash
./build/Release/install/bin/cuterm
```

### Font

`cuterm` 默认优先使用 `Maple Mono NF CN`，以便更好显示中文与 Nerd Font 图标：

- 项目地址：`https://github.com/subframe7536/maple-font`
- 默认路径：`/usr/share/fonts/maple/MapleMono-NF-CN-Regular.ttf`

也可以通过环境变量覆盖字体：

```bash
CUTERM_FONT=/path/to/your-font.ttf ./build/Release/bin/cuterm
```

## Manual Conan/CMake Flow

```bash
conan profile detect --force
conan install . --output-folder build/Release/conan --build=missing -s build_type=Release
cmake -S . -B build/Release/cmake -DCMAKE_TOOLCHAIN_FILE=build/Release/conan/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build build/Release/cmake -j
cmake --install build/Release/cmake --prefix build/Release/install
```

## Release (GitHub Actions)

发布工作流位于 `.github/workflows/release.yml`，支持两种触发方式：

- Push tag（如 `v0.1.0`）
- 手动 `workflow_dispatch` 并输入 tag

工作流会：

1. 调用 `./build.sh Release`
2. 打包 `build/Release/install/bin` 到 `animitta-<tag>-linux-x86_64.tar.gz`
3. 生成同名 `.sha256`
4. 上传到 GitHub Release

## More

更详细的构建说明见 `BUILDING.md`。

## License

本项目采用 `GPL-2.0-only`（与 Linux 内核一致）。详见 `LICENSE`。
