#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>
#include <thread>
#include <mutex>
#include <string>
#include <vector>

void demo_recursive_lock()
{
    SPDLOG_INFO("[2.1] 递归加锁（同一线程）");

    std::recursive_mutex mtx;
    int value = 0;

    std::thread t([&]() {
        std::lock_guard<std::recursive_mutex> lock1(mtx);
        ++value;
        SPDLOG_INFO("  第一次加锁, value = {}", value);

        std::lock_guard<std::recursive_mutex> lock2(mtx);
        ++value;
        SPDLOG_INFO("  第二次递归加锁, value = {}", value);

        {
            std::lock_guard<std::recursive_mutex> lock3(mtx);
            ++value;
            SPDLOG_INFO("  第三次递归加锁, value = {}", value);
        }
        SPDLOG_INFO("  lock3 析构后, value = {}", value);
    });

    t.join();
    SPDLOG_INFO("  最终 value = {}", value);
}

void demo_recursive_try_lock()
{
    SPDLOG_INFO("[2.2] recursive_mutex::try_lock");

    std::recursive_mutex mtx;
    int value = 0;

    std::thread t([&]() {
        if (mtx.try_lock())
        {
            ++value;
            SPDLOG_INFO("  第一次 try_lock 成功, value = {}", value);

            if (mtx.try_lock())
            {
                ++value;
                SPDLOG_INFO("  第二次 try_lock 成功, value = {}", value);
                mtx.unlock();
            }
            mtx.unlock();
        }
    });

    t.join();
    SPDLOG_INFO("  最终 value = {}", value);
}

void demo_recursive_lock_count()
{
    SPDLOG_INFO("[2.3] 递归加锁次数跟踪");

    std::recursive_mutex mtx;
    int lock_count = 0;

    std::thread t([&]() {
        for (int i = 0; i < 5; ++i)
        {
            std::lock_guard<std::recursive_mutex> lock(mtx);
            ++lock_count;
            SPDLOG_INFO("  第 {} 次加锁, 当前锁计数: {}", i + 1, lock_count);
        }
        SPDLOG_INFO("  循环结束, lock_count = {}", lock_count);
    });

    t.join();
    SPDLOG_INFO("  最终 lock_count = {}", lock_count);
}

void demo_recursive_mutex_vs_mutex()
{
    SPDLOG_INFO("[2.4] recursive_mutex vs mutex 对比");

    SPDLOG_INFO("  std::mutex: 同一线程重复 lock 会导致未定义行为（通常死锁）");
    SPDLOG_INFO("  std::recursive_mutex: 允许同一线程重复 lock，维护锁计数");
    SPDLOG_INFO("  注意: recursive_mutex 有性能开销，应优先使用 std::mutex");
}

void demo_recursive_multithread()
{
    SPDLOG_INFO("[2.5] recursive_mutex 多线程安全");

    std::recursive_mutex mtx;
    int counter = 0;
    const int increments_per_thread = 1000;
    const int num_threads = 10;

    auto worker = [&]() {
        for (int i = 0; i < increments_per_thread; ++i)
        {
            std::lock_guard<std::recursive_mutex> lock(mtx);
            ++counter;
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i)
        threads.emplace_back(worker);

    for (auto& t : threads)
        t.join();

    int expected = increments_per_thread * num_threads;
    SPDLOG_INFO("  counter = {} (预期: {})", counter, expected);
}

void run_recursive_mutex_demo()
{
    SPDLOG_INFO("=== std::recursive_mutex 功能 ===\n");

    demo_recursive_lock();
    demo_recursive_try_lock();
    demo_recursive_lock_count();
    demo_recursive_mutex_vs_mutex();
    demo_recursive_multithread();

    SPDLOG_INFO("\n=== std::recursive_mutex 演示完成 ===\n");
}
