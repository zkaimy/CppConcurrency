# AGENTS.md

C++ 并发学习小项目，单一构建目标：`HelloWorld`。支持 Windows / Linux / macOS。

## 构建

使用 CMake presets，自动适配当前平台。

```bash
# 配置（选择当前平台）
cmake --preset win-ninja   # Windows (需 VS 开发者命令行)
cmake --preset win-vs      # Windows (VS 生成器，仅调试用)
cmake --preset linux       # Linux
cmake --preset macos       # macOS

# 构建
cmake --build --preset win-ninja-debug
cmake --build --preset win-ninja-release
cmake --build --preset win-vs-debug
cmake --build --preset linux-debug
cmake --build --preset macos-debug

# 运行
out/build/<preset名>/HelloWorld/HelloWorld.exe  # Windows
out/build/<preset名>/HelloWorld/HelloWorld      # Linux/macOS
```

## 关键信息

- **需要 C++23** — 使用了 `std::println`、`std::format`，编译选项为 `cxx_std_23`。
- **需要 OpenMP** — 通过 `find_package(OpenMP REQUIRED)` 链接。macOS 需 `brew install libomp`。
- **MSVC `/utf-8` 标志** — 源文件包含中文注释，仅 MSVC 编译时启用。
- **无测试、无 lint、无格式化工具**，只能通过构建和运行来验证。
- **clangd 补全**：`compile_commands.json` 仅 Ninja/Makefile 生成器支持。VS 用户需额外执行一次 Ninja 配置：`cmake --preset win-ninja`，生成在 `out/build/win-ninja/compile_commands.json`。

## 架构

- `HelloWorld/HelloWorld.cpp` — 主入口，包含并行累加、OpenMP 示例、快排演示
- `HelloWorld/sequential_quick_sort.h` — 多种快排实现（拷贝版、原地版、范围版）
- `01_std_thread/main.cpp` — std::thread 完整接口演示
- 全部逻辑在一个构建目标内，无库、无子包。
