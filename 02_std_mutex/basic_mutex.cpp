#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>
#include <thread>
#include <mutex>
#include <vector>
#include <string>
#include <sstream>

void demo_basic_lock_unlock()
{
    SPDLOG_INFO("[1.1] 基本 lock/unlock");

    std::mutex mtx;
    int counter = 0;

    std::thread t1([&]() {
        mtx.lock();
        ++counter;
        SPDLOG_INFO("  t1: counter = {}", counter);
        mtx.unlock();
    });

    std::thread t2([&]() {
        mtx.lock();
        ++counter;
        SPDLOG_INFO("  t2: counter = {}", counter);
        mtx.unlock();
    });

    t1.join();
    t2.join();
    SPDLOG_INFO("  最终 counter = {}", counter);
}

void demo_try_lock()
{
    SPDLOG_INFO("[1.2] try_lock");

    std::mutex mtx;
    int value = 0;

    std::thread t1([&]() {
        if (mtx.try_lock())
        {
            ++value;
            SPDLOG_INFO("  t1: 获取锁, value = {}", value);
            mtx.unlock();
        }
        else
        {
            SPDLOG_WARN("  t1: 未获取到锁");
        }
    });

    std::thread t2([&]() {
        if (mtx.try_lock())
        {
            ++value;
            SPDLOG_INFO("  t2: 获取锁, value = {}", value);
            mtx.unlock();
        }
        else
        {
            SPDLOG_WARN("  t2: 未获取到锁");
        }
    });

    t1.join();
    t2.join();
    SPDLOG_INFO("  最终 value = {}", value);
}

void demo_owning_thread()
{
    SPDLOG_INFO("[1.3] mutex::mutex() 不可拷贝/移动");

    std::mutex mtx;
    // std::mutex m2(mtx);           // 编译错误：删除了拷贝构造
    // std::mutex m3 = std::move(mtx); // 编译错误：删除了移动构造
    SPDLOG_INFO("  std::mutex 不可拷贝、不可移动（编译期保证）");
}

void demo_shared_data()
{
    SPDLOG_INFO("[1.4] 保护共享数据");

    std::mutex mtx;
    std::vector<int> shared_vec;

    auto worker = [&](int start, int count) {
        for (int i = 0; i < count; ++i)
        {
            std::lock_guard<std::mutex> lock(mtx);
            shared_vec.push_back(start + i);
        }
    };

    std::vector<std::thread> threads;
    threads.emplace_back(worker, 0, 100);
    threads.emplace_back(worker, 100, 100);
    threads.emplace_back(worker, 200, 100);

    for (auto& t : threads)
        t.join();

    SPDLOG_INFO("  共享向量大小: {} (预期: 300)", shared_vec.size());
}

void demo_deadlock_example()
{
    SPDLOG_INFO("[1.5] 死锁演示（固定顺序加锁避免）");

    std::mutex mtx1, mtx2;
    int resource_a = 0, resource_b = 0;

    auto worker1 = [&]() {
        std::lock_guard<std::mutex> lock1(mtx1);
        std::lock_guard<std::mutex> lock2(mtx2);
        ++resource_a;
        ++resource_b;
        SPDLOG_INFO("  worker1: a={}, b={}", resource_a, resource_b);
    };

    auto worker2 = [&]() {
        std::lock_guard<std::mutex> lock1(mtx1);
        std::lock_guard<std::mutex> lock2(mtx2);
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

void run_basic_mutex_demo()
{
    SPDLOG_INFO("=== std::mutex 基本功能 ===\n");

    demo_basic_lock_unlock();
    demo_try_lock();
    demo_owning_thread();
    demo_shared_data();
    demo_deadlock_example();

    SPDLOG_INFO("\n=== std::mutex 演示完成 ===\n");
}
