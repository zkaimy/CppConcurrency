#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>
#include <thread>
#include <mutex>
#include <chrono>

using namespace std::chrono_literals;

void demo_basic_unique_lock()
{
    SPDLOG_INFO("[7.1] unique_lock 基本用法");

    std::mutex mtx;
    int value = 0;

    std::thread t([&]() {
        std::unique_lock<std::mutex> lock(mtx);
        ++value;
        SPDLOG_INFO("  unique_lock 持有锁, value = {}", value);
    });

    t.join();
    SPDLOG_INFO("  最终 value = {}", value);
}

void demo_unique_lock_defer_lock()
{
    SPDLOG_INFO("[7.2] unique_lock defer_lock（延迟加锁）");

    std::mutex mtx;
    int value = 0;

    std::thread t([&]() {
        std::unique_lock<std::mutex> lock(mtx, std::defer_lock);
        SPDLOG_INFO("  deferred: owns_lock = {}", lock.owns_lock());

        lock.lock();
        SPDLOG_INFO("  手动 lock: owns_lock = {}", lock.owns_lock());
        ++value;

        lock.unlock();
        SPDLOG_INFO("  unlock: owns_lock = {}", lock.owns_lock());
    });

    t.join();
    SPDLOG_INFO("  最终 value = {}", value);
}

void demo_unique_lock_try_to_lock()
{
    SPDLOG_INFO("[7.3] unique_lock try_to_lock");

    std::mutex mtx;
    int value = 0;

    std::thread t1([&]() {
        std::unique_lock<std::mutex> lock(mtx, std::try_to_lock);
        if (lock.owns_lock())
        {
            ++value;
            SPDLOG_INFO("  t1: 获取锁, value = {}", value);
            std::this_thread::sleep_for(50ms);
        }
        else
        {
            SPDLOG_WARN("  t1: 未获取到锁");
        }
    });

    std::thread t2([&]() {
        std::this_thread::sleep_for(10ms);
        std::unique_lock<std::mutex> lock(mtx, std::try_to_lock);
        if (lock.owns_lock())
        {
            ++value;
            SPDLOG_INFO("  t2: 获取锁, value = {}", value);
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

void demo_unique_lock_adopt_lock()
{
    SPDLOG_INFO("[7.4] unique_lock adopt_lock");

    std::mutex mtx;
    int value = 0;

    std::thread t([&]() {
        mtx.lock();
        std::unique_lock<std::mutex> lock(mtx, std::adopt_lock);
        SPDLOG_INFO("  adopt_lock: owns_lock = {}", lock.owns_lock());
        ++value;
        // lock 析构时自动 unlock
    });

    t.join();
    SPDLOG_INFO("  最终 value = {}", value);
}

void demo_unique_lock_move()
{
    SPDLOG_INFO("[7.5] unique_lock 所有权转移（move）");

    std::mutex mtx;
    int value = 0;

    std::thread t([&]() {
        std::unique_lock<std::mutex> lock1(mtx);
        ++value;
        SPDLOG_INFO("  lock1 持有锁, value = {}", value);

        std::unique_lock<std::mutex> lock2 = std::move(lock1);
        SPDLOG_INFO("  lock1 owns_lock = {}", lock1.owns_lock());
        SPDLOG_INFO("  lock2 owns_lock = {}", lock2.owns_lock());
        ++value;
        SPDLOG_INFO("  lock2: value = {}", value);
    });

    t.join();
    SPDLOG_INFO("  最终 value = {}", value);
}

void demo_unique_lock_manual_lock_unlock()
{
    SPDLOG_INFO("[7.6] unique_lock 手动 lock/unlock（灵活控制）");

    std::mutex mtx;
    int value = 0;

    std::thread t([&]() {
        std::unique_lock<std::mutex> lock(mtx);

        ++value;
        SPDLOG_INFO("  持锁操作: value = {}", value);

        lock.unlock();
        SPDLOG_INFO("  临时释放锁");

        // 可以执行不需要锁的操作
        std::this_thread::sleep_for(10ms);

        lock.lock();
        ++value;
        SPDLOG_INFO("  重新获取锁: value = {}", value);
    });

    t.join();
    SPDLOG_INFO("  最终 value = {}", value);
}

void demo_unique_lock_with_condition_variable()
{
    SPDLOG_INFO("[7.7] unique_lock 与 condition_variable 配合使用");

    std::mutex mtx;
    std::condition_variable cv;
    bool ready = false;
    int value = 0;

    std::thread notifier([&]() {
        std::this_thread::sleep_for(50ms);
        {
            std::lock_guard<std::mutex> lock(mtx);
            ready = true;
            value = 42;
        }
        cv.notify_one();
        SPDLOG_INFO("  notifier: 发送通知");
    });

    std::thread waiter([&]() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&]() { return ready; });
        SPDLOG_INFO("  waiter: 收到通知, value = {}", value);
    });

    notifier.join();
    waiter.join();
}

void demo_unique_lock_release()
{
    SPDLOG_INFO("[7.8] unique_lock::release() 释放所有权");

    std::mutex mtx;
    int value = 0;

    std::thread t([&]() {
        std::unique_lock<std::mutex> lock(mtx);
        ++value;
        SPDLOG_INFO("  持有锁: value = {}", value);

        std::mutex* released = lock.release();
        SPDLOG_INFO("  released mutex 指针有效: {}", released != nullptr);

        released->unlock();
        SPDLOG_INFO("  手动解锁");
    });

    t.join();
    SPDLOG_INFO("  最终 value = {}", value);
}

void demo_unique_lock_mutex()
{
    SPDLOG_INFO("[7.9] unique_lock::mutex() 获取关联 mutex");

    std::mutex mtx;
    int value = 0;

    std::thread t([&]() {
        std::unique_lock<std::mutex> lock(mtx);
        std::mutex* associated = lock.mutex();
        SPDLOG_INFO("  关联的 mutex 指针有效: {}", associated != nullptr);

        associated->lock();
        ++value;
        associated->unlock();
        SPDLOG_INFO("  通过 mutex() 操作: value = {}", value);
    });

    t.join();
    SPDLOG_INFO("  最终 value = {}", value);
}

void run_unique_lock_demo()
{
    SPDLOG_INFO("=== std::unique_lock 功能 ===\n");

    demo_basic_unique_lock();
    demo_unique_lock_defer_lock();
    demo_unique_lock_try_to_lock();
    demo_unique_lock_adopt_lock();
    demo_unique_lock_move();
    demo_unique_lock_manual_lock_unlock();
    demo_unique_lock_with_condition_variable();
    demo_unique_lock_release();
    demo_unique_lock_mutex();

    SPDLOG_INFO("\n=== std::unique_lock 演示完成 ===\n");
}
