#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

void run_basic_mutex_demo();
void run_recursive_mutex_demo();
void run_timed_mutex_demo();
void run_recursive_timed_mutex_demo();
void run_shared_mutex_demo();
void run_lock_guards_demo();
void run_unique_lock_demo();
void run_lock_algorithms_demo();

void init_logger()
{
    auto logger = spdlog::stdout_color_mt("mutex_demo");
    logger->set_pattern("[%H:%M:%S.%f|%l|%t|%!] %v");
    spdlog::set_default_logger(logger);
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    init_logger();

    SPDLOG_INFO("=== std::mutex 完整功能演示 ===\n");

    run_basic_mutex_demo();
    run_recursive_mutex_demo();
    run_timed_mutex_demo();
    run_recursive_timed_mutex_demo();
    run_shared_mutex_demo();
    run_lock_guards_demo();
    run_unique_lock_demo();
    run_lock_algorithms_demo();

    SPDLOG_INFO("=== 全部演示完成 ===");
    SPDLOG_INFO("涵盖的 mutex 类型:");
    SPDLOG_INFO("  1. std::mutex - 基本互斥锁");
    SPDLOG_INFO("  2. std::recursive_mutex - 递归互斥锁");
    SPDLOG_INFO("  3. std::timed_mutex - 超时互斥锁");
    SPDLOG_INFO("  4. std::recursive_timed_mutex - 递归超时互斥锁");
    SPDLOG_INFO("  5. std::shared_mutex - 共享互斥锁（读写锁）");
    SPDLOG_INFO("  6. std::lock_guard / std::scoped_lock - RAII 锁");
    SPDLOG_INFO("  7. std::unique_lock - 灵活锁");
    SPDLOG_INFO("  8. std::lock / std::try_lock - 锁算法");

    return 0;
}
