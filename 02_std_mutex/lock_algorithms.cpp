#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>
#include <thread>
#include <mutex>

void demo_std_lock_two()
{
    SPDLOG_INFO("[8.1] std::lock 同时锁定两个 mutex（避免死锁）");

    std::mutex mtx1, mtx2;
    int a = 0, b = 0;

    auto worker1 = [&]() {
        std::lock(mtx1, mtx2);
        std::lock_guard<std::mutex> lock1(mtx1, std::adopt_lock);
        std::lock_guard<std::mutex> lock2(mtx2, std::adopt_lock);
        ++a;
        ++b;
        SPDLOG_INFO("  worker1: a={}, b={}", a, b);
    };

    auto worker2 = [&]() {
        std::lock(mtx2, mtx1); // 顺序相反，但 std::lock 会避免死锁
        std::lock_guard<std::mutex> lock1(mtx1, std::adopt_lock);
        std::lock_guard<std::mutex> lock2(mtx2, std::adopt_lock);
        --a;
        --b;
        SPDLOG_INFO("  worker2: a={}, b={}", a, b);
    };

    std::thread t1(worker1);
    std::thread t2(worker2);
    t1.join();
    t2.join();
    SPDLOG_INFO("  最终: a={}, b={}", a, b);
}

void demo_std_lock_three()
{
    SPDLOG_INFO("[8.2] std::lock 同时锁定三个 mutex");

    std::mutex mtx1, mtx2, mtx3;
    int a = 0, b = 0, c = 0;

    std::thread t([&]() {
        std::lock(mtx1, mtx2, mtx3);
        std::lock_guard<std::mutex> lock1(mtx1, std::adopt_lock);
        std::lock_guard<std::mutex> lock2(mtx2, std::adopt_lock);
        std::lock_guard<std::mutex> lock3(mtx3, std::adopt_lock);
        ++a;
        ++b;
        ++c;
        SPDLOG_INFO("  a={}, b={}, c={}", a, b, c);
    });

    t.join();
    SPDLOG_INFO("  最终: a={}, b={}, c={}", a, b, c);
}

void demo_std_try_lock_two()
{
    SPDLOG_INFO("[8.3] std::try_lock 尝试锁定两个 mutex");

    std::mutex mtx1, mtx2;
    int a = 0, b = 0;

    auto worker1 = [&]() {
        int result = std::try_lock(mtx1, mtx2);
        if (result == -1)
        {
            std::lock_guard<std::mutex> lock1(mtx1, std::adopt_lock);
            std::lock_guard<std::mutex> lock2(mtx2, std::adopt_lock);
            ++a;
            ++b;
            SPDLOG_INFO("  worker1: 全部获取, a={}, b={}", a, b);
        }
        else
        {
            SPDLOG_WARN("  worker1: 第 {} 个 mutex 获取失败", result);
        }
    };

    std::thread t1(worker1);
    t1.join();
    SPDLOG_INFO("  最终: a={}, b={}", a, b);
}

void demo_std_try_lock_three()
{
    SPDLOG_INFO("[8.4] std::try_lock 尝试锁定三个 mutex");

    std::mutex mtx1, mtx2, mtx3;
    int a = 0, b = 0, c = 0;

    std::thread t([&]() {
        int result = std::try_lock(mtx1, mtx2, mtx3);
        if (result == -1)
        {
            std::lock_guard<std::mutex> lock1(mtx1, std::adopt_lock);
            std::lock_guard<std::mutex> lock2(mtx2, std::adopt_lock);
            std::lock_guard<std::mutex> lock3(mtx3, std::adopt_lock);
            ++a;
            ++b;
            ++c;
            SPDLOG_INFO("  全部获取: a={}, b={}, c={}", a, b, c);
        }
        else
        {
            SPDLOG_WARN("  第 {} 个 mutex 获取失败", result);
        }
    });

    t.join();
    SPDLOG_INFO("  最终: a={}, b={}, c={}", a, b, c);
}

void demo_std_lock_vs_scoped_lock()
{
    SPDLOG_INFO("[8.5] std::lock vs std::scoped_lock 对比");

    SPDLOG_INFO("  std::lock(m1, m2): 需配合 adopt_lock 使用，较繁琐");
    SPDLOG_INFO("  std::scoped_lock(m1, m2): C++17，更简洁安全");
    SPDLOG_INFO("  推荐使用 scoped_lock");
}

void demo_lock_with_unique_lock()
{
    SPDLOG_INFO("[8.6] std::lock 与 unique_lock 配合");

    std::mutex mtx1, mtx2;
    int a = 0, b = 0;

    std::thread t([&]() {
        std::unique_lock<std::mutex> lock1(mtx1, std::defer_lock);
        std::unique_lock<std::mutex> lock2(mtx2, std::defer_lock);
        std::lock(lock1, lock2);
        ++a;
        ++b;
        SPDLOG_INFO("  a={}, b={}", a, b);
    });

    t.join();
    SPDLOG_INFO("  最终: a={}, b={}", a, b);
}

void run_lock_algorithms_demo()
{
    SPDLOG_INFO("=== std::lock / std::try_lock 算法 ===\n");

    demo_std_lock_two();
    demo_std_lock_three();
    demo_std_try_lock_two();
    demo_std_try_lock_three();
    demo_std_lock_vs_scoped_lock();
    demo_lock_with_unique_lock();

    SPDLOG_INFO("\n=== lock 算法演示完成 ===\n");
}
