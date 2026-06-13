#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <thread>
#include <vector>
#include <string>
#include <functional>
#include <numeric>
#include <chrono>

using namespace std::chrono_literals;

void init_logger()
{
    auto logger = spdlog::stdout_color_mt("main");
    logger->set_pattern("[%H:%M:%S.%f|%l|%t|%!] %v");
    spdlog::set_default_logger(logger);
}

// ============================================================
// 1. 基本线程创建
// ============================================================

void basic_function()
{
    SPDLOG_INFO("[1.1] 普通函数");
}

auto basic_lambda = []()
{
    SPDLOG_INFO("[1.2] Lambda");
};

void print_value(int value, const std::string& label)
{
    SPDLOG_INFO("[1.3] 带参数: {} = {}", label, value);
}

class FunctionObject
{
public:
    void operator()() const
    {
        SPDLOG_INFO("[1.4] 函数对象");
    }
};

class Worker
{
public:
    void do_work(int id)
    {
        SPDLOG_INFO("[1.5] 成员函数 - worker {}", id);
    }
    static void static_work()
    {
        SPDLOG_INFO("[1.5] 静态成员函数");
    }
};

// ============================================================
// 2. 参数传递方式
// ============================================================

void modify_value(int& value)
{
    value *= 2;
    SPDLOG_INFO("[2.1] 引用传递: 值变为 {}", value);
}

void by_value_copy(const std::string s)
{
    SPDLOG_INFO("[2.2] 值传递（拷贝）: {}", s);
}

// ============================================================
// 3. join 和 detach
// ============================================================

void long_task(const std::string& name, int seconds)
{
    SPDLOG_INFO("[3] {} 开始，耗时 {}s", name, seconds);
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
    SPDLOG_INFO("[3] {} 完成", name);
}

// ============================================================
// 4. 线程 ID 和 hardware_concurrency
// ============================================================

void print_thread_info(int index)
{
    SPDLOG_INFO("[4] 线程 {}  | hardware_concurrency: {}",
        index, std::thread::hardware_concurrency());
}

// ============================================================
// 5. 线程移动
// ============================================================

void movable_task(int value)
{
    SPDLOG_INFO("[5] 可移动线程 - value: {}", value);
}

// ============================================================
// 6. RAII 线程包装器
// ============================================================

class ScopedThread
{
public:
    explicit ScopedThread(std::thread t) : m_thread(std::move(t)) {}
    ~ScopedThread()
    {
        if (m_thread.joinable())
            m_thread.join();
    }
    ScopedThread(const ScopedThread&) = delete;
    ScopedThread& operator=(const ScopedThread&) = delete;
private:
    std::thread m_thread;
};

// ============================================================
// 7. 并行累加
// ============================================================

void parallel_accumulate_demo()
{
    SPDLOG_INFO("[7] 并行累加开始");

    const long long N = 1000000;
    std::vector<long long> data(N);
    std::iota(data.begin(), data.end(), 1LL);

    int num_threads = std::max(1u, std::thread::hardware_concurrency());
    long long block_size = N / num_threads;
    std::vector<long long> results(num_threads, 0);
    std::vector<std::thread> threads;

    for (int i = 0; i < num_threads; ++i)
    {
        long long start = i * block_size;
        long long end = (i == num_threads - 1) ? N : start + block_size;
        threads.emplace_back([&data, &results, i, start, end]() {
            results[i] = std::accumulate(data.begin() + start, data.begin() + end, 0LL);
        });
    }

    for (auto& t : threads)
        t.join();

    long long total = std::accumulate(results.begin(), results.end(), 0LL);
    long long expected = N * (N + 1) / 2;
    SPDLOG_INFO("[7] 并行累加结果: {} (预期: {})", total, expected);
}

// ============================================================
// 8. 异常处理
// ============================================================

void throwing_function()
{
    throw std::runtime_error("[8] 线程内抛出异常");
}

void exception_demo()
{
    SPDLOG_INFO("[8] 异常处理演示开始");

    std::thread t([]() {
        try
        {
            throwing_function();
        }
        catch (const std::exception& e)
        {
            SPDLOG_ERROR("[8] 捕获线程异常: {}", e.what());
        }
    });
    t.join();

    SPDLOG_INFO("[8] 异常处理演示结束");
}

// ============================================================
// 9. 数据竞争（不安全）
// ============================================================

void unsafe_increment(int& counter, int iterations)
{
    for (int i = 0; i < iterations; ++i)
        ++counter;
}

// ============================================================
// 10. 数据竞争（安全，使用 mutex）
// ============================================================

void safe_increment(int& counter, std::mutex& mtx, int iterations)
{
    for (int i = 0; i < iterations; ++i)
    {
        std::lock_guard<std::mutex> lock(mtx);
        ++counter;
    }
}

// ============================================================
// main
// ============================================================

int main()
{
#ifdef _WIN32
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);
#endif
    init_logger();

    SPDLOG_INFO("=== std::thread 完整接口演示 ===\n");

    // ----------------------------------------------------------
    // 1. 基本线程创建
    // ----------------------------------------------------------
    SPDLOG_INFO("--- 1. 基本线程创建 ---");

    std::thread t1(basic_function);
    std::thread t2(basic_lambda);
    std::thread t3(print_value, 42, "hello");
    std::thread t4(FunctionObject{});

    Worker worker;
    std::thread t5(&Worker::do_work, &worker, 1);
    std::thread t6(&Worker::static_work);

    t1.join(); t2.join(); t3.join(); t4.join(); t5.join(); t6.join();

    // ----------------------------------------------------------
    // 2. 参数传递
    // ----------------------------------------------------------
    SPDLOG_INFO("\n--- 2. 参数传递 ---");

    int value = 10;
    std::thread t7(modify_value, std::ref(value));
    t7.join();
    SPDLOG_INFO("  主线程中 value = {}", value);

    std::string msg = "hello world";
    std::thread t8(by_value_copy, msg);
    t8.join();
    SPDLOG_INFO("  原始字符串未变: {}", msg);

    // ----------------------------------------------------------
    // 3. join 和 detach
    // ----------------------------------------------------------
    SPDLOG_INFO("\n--- 3. join 和 detach ---");

    std::thread t9(long_task, "任务A", 1);
    std::thread t10(long_task, "任务B", 2);

    SPDLOG_INFO("  t9 joinable: {}", t9.joinable());
    SPDLOG_INFO("  t10 joinable: {}", t10.joinable());

    t9.join();
    SPDLOG_INFO("  t9 join 后 joinable: {}", t9.joinable());

    t10.detach();
    SPDLOG_INFO("  t10 detach 后 joinable: {}", t10.joinable());

    std::this_thread::sleep_for(3s);

    // ----------------------------------------------------------
    // 4. 线程 ID 和硬件并发
    // ----------------------------------------------------------
    SPDLOG_INFO("\n--- 4. 线程 ID 和硬件并发 ---");

    SPDLOG_INFO("  主线程 ");
    SPDLOG_INFO("  硬件并发数: {}", std::thread::hardware_concurrency());

    std::vector<std::thread> info_threads;
    for (int i = 0; i < 4; ++i)
        info_threads.emplace_back(print_thread_info, i);
    for (auto& t : info_threads)
        t.join();

    // ----------------------------------------------------------
    // 5. 线程移动
    // ----------------------------------------------------------
    SPDLOG_INFO("\n--- 5. 线程移动 ---");

    std::thread t11(movable_task, 100);
    std::thread t12 = std::move(t11);
    SPDLOG_INFO("  t11 move 后 joinable: {}", t11.joinable());
    SPDLOG_INFO("  t12 joinable: {}", t12.joinable());
    t12.join();

    // ----------------------------------------------------------
    // 6. RAII 线程包装器
    // ----------------------------------------------------------
    SPDLOG_INFO("\n--- 6. RAII ScopedThread ---");

    {
        ScopedThread st(std::thread([]() {
            SPDLOG_INFO("[6] ScopedThread 析构时自动 join");
        }));
    }
    SPDLOG_INFO("[6] ScopedThread 已析构");

    // ----------------------------------------------------------
    // 7. 并行累加
    // ----------------------------------------------------------
    SPDLOG_INFO("\n--- 7. 并行累加 ---");

    parallel_accumulate_demo();

    // ----------------------------------------------------------
    // 8. 异常处理
    // ----------------------------------------------------------
    SPDLOG_INFO("\n--- 8. 异常处理 ---");

    exception_demo();

    // ----------------------------------------------------------
    // 9. 数据竞争（不安全）
    // ----------------------------------------------------------
    SPDLOG_INFO("\n--- 9. 数据竞争（不安全）---");

    int unsafe_counter = 0;
    const int iterations = 100000;
    const int num_threads = 10;

    {
        std::vector<std::thread> threads;
        for (int i = 0; i < num_threads; ++i)
            threads.emplace_back(unsafe_increment, std::ref(unsafe_counter), iterations);
        for (auto& t : threads)
            t.join();
    }
    SPDLOG_WARN("  不安全计数器: {} (预期: {})", unsafe_counter, num_threads * iterations);

    // ----------------------------------------------------------
    // 10. 数据竞争（安全，使用 mutex）
    // ----------------------------------------------------------
    SPDLOG_INFO("\n--- 10. 数据竞争（安全，mutex）---");

    int safe_counter = 0;
    std::mutex counter_mutex;

    {
        std::vector<std::thread> threads;
        for (int i = 0; i < num_threads; ++i)
            threads.emplace_back(safe_increment, std::ref(safe_counter), std::ref(counter_mutex), iterations);
        for (auto& t : threads)
            t.join();
    }
    SPDLOG_INFO("  安全计数器: {} (预期: {})", safe_counter, num_threads * iterations);

    // ----------------------------------------------------------
    // 总结
    // ----------------------------------------------------------
    SPDLOG_INFO("\n=== 演示完成 ===");
    SPDLOG_INFO("std::thread 核心接口：");
    SPDLOG_INFO("  构造: function, lambda, functor, member function, static member");
    SPDLOG_INFO("  参数: 值传递, std::ref 引用传递");
    SPDLOG_INFO("  控制: join(), detach(), joinable()");
    SPDLOG_INFO("  信息: get_id(), hardware_concurrency()");
    SPDLOG_INFO("  移动: std::move() 转移所有权");
    SPDLOG_INFO("  线程安全: spdlog 内置线程安全");

    return 0;
}
