# CLAUDE.md

本文件为 Claude Code（claude.ai/code）在此仓库中工作时提供的指导。

## 项目概览

CppConcurrency 是一个 C++ 并发学习项目，演示了多种并行编程技术，包括：
- 多种实现的顺序快速排序算法
- 使用 `std::thread` 的并行累加
- 带归约的 OpenMP 并行 `for`
- 线程安全操作与时间测量

## 构建说明

本项目使用 CMake，支持 Windows / Linux / macOS。使用 presets 自动适配平台。

```bash
# 配置（选择当前平台）
cmake --preset windows-ninja   # Windows (需 VS 开发者命令行)
cmake --preset windows-vs      # Windows (VS 生成器，仅调试用)
cmake --preset linux           # Linux
cmake --preset macos           # macOS

# 构建
cmake --build --preset windows-ninja-debug
cmake --build --preset linux-debug
cmake --build --preset macos-debug
```

运行可执行文件：
```bash
# Windows
out/build/windows-ninja/HelloWorld/HelloWorld.exe

# Linux / macOS
out/build/linux/HelloWorld/HelloWorld
```

## 关键源文件

- `HelloWorld/HelloWorld.cpp` – 主程序，包含并发示例
- `HelloWorld/sequential_quick_sort.h` – 快速排序实现（拷贝版、原地版、范围版）
- `HelloWorld/HelloWorld.h` – 头文件
- `CMakeLists.txt` – 顶层 CMake 配置
- `HelloWorld/CMakeLists.txt` – HelloWorld 目标配置

## 架构说明

项目展示了以下核心概念：
1. **sequential_quick_sort** – 多种快速排序实现（拷贝版、原地版、范围版）
2. **parallel_accumulate** – 基于线程的并行归约实现
3. **measure_time** – 用于测量函数执行时间的模板函数
4. 使用归约的 OpenMP 并行 `for`

## 依赖

- C++23 标准库特性（`std::println`、`std::format`）
- OpenMP 用于并行执行（macOS 需 `brew install libomp`）
- CMake ≥ 3.10

## clangd 补全

`compile_commands.json` 仅在 Ninja/Makefile 生成器下自动生成。VS 用户需额外执行一次 Ninja 配置：

```bash
cmake --preset windows-ninja
```

生成的文件位于 `out/build/windows-ninja/compile_commands.json`。
