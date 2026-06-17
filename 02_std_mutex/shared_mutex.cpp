#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>
#include <thread>
#include <shared_mutex>
#include <vector>
#include <string>
#include <chrono>

using namespace std::chrono_literals;

void demo_basic_shared_lock()
{
    SPDLOG_INFO("[5.1] shared_mutex 基本读写锁");

    std::shared_mutex mtx;
    int shared_value = 0;

    auto reader = [&](int id) {
        std::shared_lock<std::shared_mutex> lock(mtx);
        SPDLOG_INFO("  读线程 {}: value = {}", id, shared_value);
        std::this_thread::sleep_for(10ms);
    };

    auto writer = [&](int id) {
        std::unique_lock<std::shared_mutex> lock(mtx);
        ++shared_value;
        SPDLOG_INFO("  写线程 {}: value = {}", id, shared_value);
        std::this_thread::sleep_for(5ms);
    };

    std::vector<std::thread> threads;
    threads.emplace_back(writer, 0);
    for (int i = 0; i < 3; ++i)
        threads.emplace_back(reader, i);
    threads.emplace_back(writer, 1);
    for (int i = 3; i < 6; ++i)
        threads.emplace_back(reader, i);

    for (auto& t : threads)
        t.join();

    SPDLOG_INFO("  最终 shared_value = {}", shared_value);
}

void demo_multiple_readers()
{
    SPDLOG_INFO("[5.2] 多个读线程并发");

    std::shared_mutex mtx;
    int shared_value = 42;
    int read_count = 0;

    auto reader = [&](int id) {
        std::shared_lock<std::shared_mutex> lock(mtx);
        ++read_count;
        SPDLOG_INFO("  读线程 {}: value={}, read_count={}", id, shared_value, read_count);
        std::this_thread::sleep_for(20ms);
        --read_count;
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i)
        threads.emplace_back(reader, i);

    for (auto& t : threads)
        t.join();

    SPDLOG_INFO("  所有读线程完成");
}

void demo_unique_lock_shared_lock()
{
    SPDLOG_INFO("[5.3] unique_lock vs shared_lock");

    std::shared_mutex mtx;
    int value = 0;

    // unique_lock: 独占访问
    std::thread writer([&]() {
        std::unique_lock<std::shared_mutex> lock(mtx);
        ++value;
        SPDLOG_INFO("  writer: value = {}", value);
        std::this_thread::sleep_for(50ms);
    });

    // shared_lock: 共享访问
    std::thread reader1([&]() {
        std::this_thread::sleep_for(10ms);
        std::shared_lock<std::shared_mutex> lock(mtx);
        SPDLOG_INFO("  reader1: value = {}", value);
    });

    std::thread reader2([&]() {
        std::this_thread::sleep_for(10ms);
        std::shared_lock<std::shared_mutex> lock(mtx);
        SPDLOG_INFO("  reader2: value = {}", value);
    });

    writer.join();
    reader1.join();
    reader2.join();
    SPDLOG_INFO("  最终 value = {}", value);
}

void demo_shared_mutex_performance()
{
    SPDLOG_INFO("[5.4] shared_mutex 性能对比说明");

    SPDLOG_INFO("  shared_mutex 适用于读多写少的场景");
    SPDLOG_INFO("  多个 shared_lock 可以同时持有");
    SPDLOG_INFO("  unique_lock 与 shared_lock 互斥");
    SPDLOG_INFO("  注意: shared_mutex 在写少读多时性能优于 mutex");
}

void demo_shared_lock_defer_lock()
{
    SPDLOG_INFO("[5.5] shared_lock 的 defer_lock");

    std::shared_mutex mtx;
    int value = 0;

    std::thread t([&]() {
        std::shared_lock<std::shared_mutex> lock(mtx, std::defer_lock);
        SPDLOG_INFO("  deferred lock 后, owns_lock = {}", lock.owns_lock());

        lock.lock();
        SPDLOG_INFO("  手动 lock 后, owns_lock = {}", lock.owns_lock());
        ++value;
        SPDLOG_INFO("  value = {}", value);
    });

    t.join();
    SPDLOG_INFO("  最终 value = {}", value);
}

void demo_shared_lock_adopt_lock()
{
    SPDLOG_INFO("[5.6] shared_lock 的 adopt_lock");

    std::shared_mutex mtx;
    int value = 0;

    std::thread t([&]() {
        mtx.lock_shared();
        std::shared_lock<std::shared_mutex> lock(mtx, std::adopt_lock);
        SPDLOG_INFO("  adopt_lock 后, owns_lock = {}", lock.owns_lock());
        ++value;
        SPDLOG_INFO("  value = {}", value);
        // lock 析构时自动 unlock_shared
    });

    t.join();
    SPDLOG_INFO("  最终 value = {}", value);
}

void run_shared_mutex_demo()
{
    SPDLOG_INFO("=== std::shared_mutex 功能 ===\n");

    demo_basic_shared_lock();
    demo_multiple_readers();
    demo_unique_lock_shared_lock();
    demo_shared_mutex_performance();
    demo_shared_lock_defer_lock();
    demo_shared_lock_adopt_lock();

    SPDLOG_INFO("\n=== std::shared_mutex 演示完成 ===\n");
}
