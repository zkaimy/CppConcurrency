#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>

using namespace std::chrono_literals;

void demo_basic_recursive_timed()
{
    SPDLOG_INFO("[4.1] recursive_timed_mutex 基本递归 + 超时");

    std::recursive_timed_mutex mtx;
    int value = 0;

    std::thread t([&]() {
        std::lock_guard<std::recursive_timed_mutex> lock1(mtx);
        ++value;
        SPDLOG_INFO("  第一次加锁, value = {}", value);

        if (mtx.try_lock_for(100ms))
        {
            ++value;
            SPDLOG_INFO("  try_lock_for 成功, value = {}", value);
            mtx.unlock();
        }
    });

    t.join();
    SPDLOG_INFO("  最终 value = {}", value);
}

void demo_try_lock_until_recursive()
{
    SPDLOG_INFO("[4.2] recursive_timed_mutex::try_lock_until 递归调用");

    std::recursive_timed_mutex mtx;
    int value = 0;

    std::thread t([&]() {
        auto deadline = std::chrono::steady_clock::now() + 200ms;
        if (mtx.try_lock_until(deadline))
        {
            ++value;
            SPDLOG_INFO("  第一次 try_lock_until, value = {}", value);

            deadline = std::chrono::steady_clock::now() + 200ms;
            if (mtx.try_lock_until(deadline))
            {
                ++value;
                SPDLOG_INFO("  第二次 try_lock_until, value = {}", value);
                mtx.unlock();
            }
            mtx.unlock();
        }
    });

    t.join();
    SPDLOG_INFO("  最终 value = {}", value);
}

void demo_nested_timeout()
{
    SPDLOG_INFO("[4.3] 嵌套超时场景");

    std::recursive_timed_mutex mtx;
    int value = 0;

    std::thread holder([&]() {
        std::lock_guard<std::recursive_timed_mutex> lock(mtx);
        ++value;
        SPDLOG_INFO("  holder: 持有锁 300ms, value = {}", value);
        std::this_thread::sleep_for(300ms);
    });

    std::thread waiter([&]() {
        std::this_thread::sleep_for(50ms);

        SPDLOG_INFO("  waiter: 尝试 try_lock_for(100ms)");
        if (mtx.try_lock_for(100ms))
        {
            SPDLOG_INFO("  waiter: 获取锁");
            mtx.unlock();
        }
        else
        {
            SPDLOG_WARN("  waiter: 超时");
        }

        std::this_thread::sleep_for(100ms);
        SPDLOG_INFO("  waiter: 再次尝试 try_lock_for(200ms)");
        if (mtx.try_lock_for(200ms))
        {
            SPDLOG_INFO("  waiter: 获取锁, value = {}", ++value);
            mtx.unlock();
        }
        else
        {
            SPDLOG_WARN("  waiter: 再次超时");
        }
    });

    holder.join();
    waiter.join();
    SPDLOG_INFO("  最终 value = {}", value);
}

void demo_multi_thread_recursive_timed()
{
    SPDLOG_INFO("[4.4] 多线程递归超时竞争");

    std::recursive_timed_mutex mtx;
    int counter = 0;

    auto worker = [&](int id) {
        for (int i = 0; i < 3; ++i)
        {
            if (mtx.try_lock_for(100ms))
            {
                ++counter;
                SPDLOG_INFO("  线程 {} 获取锁, counter = {}", id, counter);

                // 递归加锁
                if (mtx.try_lock_for(50ms))
                {
                    ++counter;
                    SPDLOG_INFO("  线程 {} 递归加锁, counter = {}", id, counter);
                    mtx.unlock();
                }

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
    for (int i = 0; i < 4; ++i)
        threads.emplace_back(worker, i);

    for (auto& t : threads)
        t.join();

    SPDLOG_INFO("  最终 counter = {}", counter);
}

void run_recursive_timed_mutex_demo()
{
    SPDLOG_INFO("=== std::recursive_timed_mutex 功能 ===\n");

    demo_basic_recursive_timed();
    demo_try_lock_until_recursive();
    demo_nested_timeout();
    demo_multi_thread_recursive_timed();

    SPDLOG_INFO("\n=== std::recursive_timed_mutex 演示完成 ===\n");
}
