#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>
#include <thread>
#include <mutex>

void demo_lock_guard_basic()
{
    SPDLOG_INFO("[6.1] lock_guard 基本用法");

    std::mutex mtx;
    int value = 0;

    std::thread t([&]() {
        std::lock_guard<std::mutex> lock(mtx);
        ++value;
        SPDLOG_INFO("  lock_guard 持有锁, value = {}", value);
        // lock 析构时自动 unlock
    });

    t.join();
    SPDLOG_INFO("  最终 value = {}", value);
}

void demo_lock_guard_scope()
{
    SPDLOG_INFO("[6.2] lock_guard 作用域控制");

    std::mutex mtx;
    int value = 0;

    std::thread t([&]() {
        SPDLOG_INFO("  进入线程");
        {
            std::lock_guard<std::mutex> lock(mtx);
            ++value;
            SPDLOG_INFO("  作用域内: value = {}", value);
        }
        SPDLOG_INFO("  作用域外: 锁已释放");
        {
            std::lock_guard<std::mutex> lock(mtx);
            ++value;
            SPDLOG_INFO("  再次进入: value = {}", value);
        }
    });

    t.join();
    SPDLOG_INFO("  最终 value = {}", value);
}

void demo_scoped_lock_basic()
{
    SPDLOG_INFO("[6.3] scoped_lock 基本用法（C++17）");

    std::mutex mtx;
    int value = 0;

    std::thread t([&]() {
        std::scoped_lock lock(mtx);
        ++value;
        SPDLOG_INFO("  scoped_lock 持有锁, value = {}", value);
    });

    t.join();
    SPDLOG_INFO("  最终 value = {}", value);
}

void demo_scoped_lock_multiple()
{
    SPDLOG_INFO("[6.4] scoped_lock 同时锁定多个 mutex（避免死锁）");

    std::mutex mtx1, mtx2;
    int resource_a = 0, resource_b = 0;

    auto worker1 = [&]() {
        std::scoped_lock lock(mtx1, mtx2);
        ++resource_a;
        ++resource_b;
        SPDLOG_INFO("  worker1: a={}, b={}", resource_a, resource_b);
    };

    auto worker2 = [&]() {
        std::scoped_lock lock(mtx2, mtx1); // 顺序不同，但 scoped_lock 会避免死锁
        --resource_a;
        --resource_b;
        SPDLOG_INFO("  worker2: a={}, b={}", resource_a, resource_b);
    };

    std::thread t1(worker1);
    std::thread t2(worker2);
    t1.join();
    t2.join();
    SPDLOG_INFO("  最终: a={}, b={}", resource_a, resource_b);
}

void demo_lock_guard_vs_scoped_lock()
{
    SPDLOG_INFO("[6.5] lock_guard vs scoped_lock 对比");

    SPDLOG_INFO("  std::lock_guard (C++11): 锁定单个 mutex");
    SPDLOG_INFO("  std::scoped_lock (C++17): 可同时锁定多个 mutex，使用 std::lock 算法避免死锁");
    SPDLOG_INFO("  优先使用 scoped_lock，特别是需要锁定多个 mutex 时");
}

void demo_lock_guard_defer()
{
    SPDLOG_INFO("[6.6] lock_guard 不支持 defer_lock");

    std::mutex mtx;

    // lock_guard 不支持 defer_lock / try_to_lock / adopt_lock
    // std::lock_guard<std::mutex> lock(mtx, std::defer_lock); // 编译错误

    SPDLOG_INFO("  lock_guard 只能在构造时立即加锁");
    SPDLOG_INFO("  需要延迟加锁请使用 std::unique_lock");
}

void demo_three_mutex_lock()
{
    SPDLOG_INFO("[6.7] scoped_lock 同时锁定三个 mutex");

    std::mutex mtx1, mtx2, mtx3;
    int a = 0, b = 0, c = 0;

    std::thread t([&]() {
        std::scoped_lock lock(mtx1, mtx2, mtx3);
        ++a;
        ++b;
        ++c;
        SPDLOG_INFO("  a={}, b={}, c={}", a, b, c);
    });

    t.join();
    SPDLOG_INFO("  最终: a={}, b={}, c={}", a, b, c);
}

void run_lock_guards_demo()
{
    SPDLOG_INFO("=== std::lock_guard / std::scoped_lock 功能 ===\n");

    demo_lock_guard_basic();
    demo_lock_guard_scope();
    demo_scoped_lock_basic();
    demo_scoped_lock_multiple();
    demo_lock_guard_vs_scoped_lock();
    demo_lock_guard_defer();
    demo_three_mutex_lock();

    SPDLOG_INFO("\n=== lock_guard / scoped_lock 演示完成 ===\n");
}
