# CppConcurrency

C++ 并发编程学习项目，演示多种并行编程技术。

## 功能

- **快速排序** — 多种实现（拷贝版、原地版、范围版）
- **并行累加** — 基于 `std::thread` 的并行归约
- **OpenMP 并行** — 使用归约的并行 `for`
- **性能测量** — 模板化的执行时间测量函数

## 依赖

- C++23 编译器（GCC 13+ / Clang 17+ / MSVC 2022+）
- CMake ≥ 3.21
- OpenMP（macOS 需 `brew install libomp`）

## 构建

```bash
# 配置（选择当前平台）
cmake --preset win-ninja     # Windows (需 VS 开发者命令行)
cmake --preset win-vs        # Windows (VS 生成器，仅调试用)
cmake --preset linux         # Linux
cmake --preset macos         # macOS

# 构建
cmake --build --preset win-ninja-debug      # Windows Debug
cmake --build --preset win-ninja-release    # Windows Release
cmake --build --preset linux-debug          # Linux Debug
cmake --build --preset macos-debug          # macOS Debug
```

运行：
```bash
out/build/win-ninja/HelloWorld/HelloWorld.exe   # Windows
out/build/linux/HelloWorld/HelloWorld           # Linux
```

## 项目结构

```
CppConcurrency/
├── CMakeLists.txt              # 顶层配置
├── CMakePresets.json           # 跨平台构建预设
└── HelloWorld/
    ├── CMakeLists.txt          # 目标配置
    ├── HelloWorld.cpp          # 主程序
    ├── HelloWorld.h            # 头文件
    └── sequential_quick_sort.h # 快排实现
```

## License

MIT
