#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>

using namespace std::chrono_literals;

void demo_basic_timed_lock()
{
    SPDLOG_INFO("[3.1] timed_mutex 基本 try_lock_for");

    std::timed_mutex mtx;
    int value = 0;

    std::thread t1([&]() {
        if (mtx.try_lock_for(100ms))
        {
            ++value;
            SPDLOG_INFO("  t1: 获取锁, value = {}", value);
            std::this_thread::sleep_for(50ms);
            mtx.unlock();
        }
        else
        {
            SPDLOG_WARN("  t1: 超时未获取到锁");
        }
    });

    std::thread t2([&]() {
        std::this_thread::sleep_for(10ms);
        if (mtx.try_lock_for(100ms))
        {
            ++value;
            SPDLOG_INFO("  t2: 获取锁, value = {}", value);
            mtx.unlock();
        }
        else
        {
            SPDLOG_WARN("  t2: 超时未获取到锁");
        }
    });

    t1.join();
    t2.join();
    SPDLOG_INFO("  最终 value = {}", value);
}

void demo_try_lock_until()
{
    SPDLOG_INFO("[3.2] timed_mutex::try_lock_until");

    std::timed_mutex mtx;
    int value = 0;

    std::thread t1([&]() {
        auto deadline = std::chrono::steady_clock::now() + 100ms;
        if (mtx.try_lock_until(deadline))
        {
            ++value;
            SPDLOG_INFO("  t1: 获取锁, value = {}", value);
            std::this_thread::sleep_for(50ms);
            mtx.unlock();
        }
        else
        {
            SPDLOG_WARN("  t1: 超时");
        }
    });

    std::thread t2([&]() {
        auto deadline = std::chrono::steady_clock::now() + 200ms;
        if (mtx.try_lock_until(deadline))
        {
            ++value;
            SPDLOG_INFO("  t2: 获取锁, value = {}", value);
            mtx.unlock();
        }
        else
        {
            SPDLOG_WARN("  t2: 超时");
        }
    });

    t1.join();
    t2.join();
    SPDLOG_INFO("  最终 value = {}", value);
}

void demo_lock_timeout()
{
    SPDLOG_INFO("[3.3] 超时场景演示");

    std::timed_mutex mtx;

    std::thread holder([&]() {
        std::lock_guard<std::timed_mutex> lock(mtx);
        SPDLOG_INFO("  holder: 持有锁 500ms");
        std::this_thread::sleep_for(500ms);
        SPDLOG_INFO("  holder: 释放锁");
    });

    std::thread waiter([&]() {
        std::this_thread::sleep_for(50ms);
        SPDLOG_INFO("  waiter: 尝试获取锁（100ms 超时）");
        if (mtx.try_lock_for(100ms))
        {
            SPDLOG_INFO("  waiter: 获取到锁");
            mtx.unlock();
        }
        else
        {
            SPDLOG_WARN("  waiter: 超时，未获取到锁");
        }
    });

    holder.join();
    waiter.join();
}

void demo_timed_lock_guard()
{
    SPDLOG_INFO("[3.4] std::lock_guard 与 timed_mutex");

    std::timed_mutex mtx;
    int value = 0;

    std::thread t1([&]() {
        std::lock_guard<std::timed_mutex> lock(mtx);
        ++value;
        SPDLOG_INFO("  t1: value = {}", value);
        std::this_thread::sleep_for(100ms);
    });

    std::thread t2([&]() {
        std::this_thread::sleep_for(50ms);
        if (mtx.try_lock_for(50ms))
        {
            ++value;
            SPDLOG_INFO("  t2: value = {}", value);
            mtx.unlock();
        }
        else
        {
            SPDLOG_WARN("  t2: 超时");
        }
    });

    t1.join();
    t2.join();
    SPDLOG_INFO("  最终 value = {}", value);
}

void demo_timed_mutex_contention()
{
    SPDLOG_INFO("[3.5] timed_mutex 竞争场景");

    std::timed_mutex mtx;
    int counter = 0;
    const int num_threads = 5;

    auto worker = [&](int id) {
        for (int i = 0; i < 3; ++i)
        {
            if (mtx.try_lock_for(200ms))
            {
                ++counter;
                SPDLOG_INFO("  线程 {} 获取锁, counter = {}", id, counter);
                std::this_thread::sleep_for(10ms);
                mtx.unlock();
            }
            else
            {
                SPDLOG_WARN("  线程 {} 超时", id);
            }
            std::this_thread::sleep_for(5ms);
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i)
        threads.emplace_back(worker, i);

    for (auto& t : threads)
        t.join();

    SPDLOG_INFO("  最终 counter = {}", counter);
}

void run_timed_mutex_demo()
{
    SPDLOG_INFO("=== std::timed_mutex 功能 ===\n");

    demo_basic_timed_lock();
    demo_try_lock_until();
    demo_lock_timeout();
    demo_timed_lock_guard();
    demo_timed_mutex_contention();

    SPDLOG_INFO("\n=== std::timed_mutex 演示完成 ===\n");
}
