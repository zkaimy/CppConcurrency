#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <functional>
#include <mutex>
#include <sstream>
#include <numeric>
#include <chrono>

using namespace std::chrono_literals;

static std::mutex g_print_mutex;

void safe_print(const std::string& msg)
{
    std::lock_guard<std::mutex> lock(g_print_mutex);
    std::cout << msg << "\n";
}

std::string tid()
{
    std::ostringstream oss;
    oss << std::this_thread::get_id();
    return oss.str();
}

// ============================================================
// 1. 基本线程创建
// ============================================================

void basic_function()
{
    safe_print("[1.1] 普通函数 - thread id: " + tid());
}

auto basic_lambda = []()
{
    safe_print("[1.2] Lambda - thread id: " + tid());
};

void print_value(int value, const std::string& label)
{
    safe_print("[1.3] 带参数: " + label + " = " + std::to_string(value));
}

class FunctionObject
{
public:
    void operator()() const
    {
        safe_print("[1.4] 函数对象 - thread id: " + tid());
    }
};

class Worker
{
public:
    void do_work(int id)
    {
        safe_print("[1.5] 成员函数 - worker " + std::to_string(id));
    }
    static void static_work()
    {
        safe_print("[1.5] 静态成员函数 - thread id: " + tid());
    }
};

// ============================================================
// 2. 参数传递方式
// ============================================================

void modify_value(int& value)
{
    value *= 2;
    safe_print("[2.1] 引用传递: 值变为 " + std::to_string(value));
}

void by_value_copy(const std::string s)
{
    safe_print("[2.2] 值传递（拷贝）: " + s);
}

// ============================================================
// 3. join 和 detach
// ============================================================

void long_task(const std::string& name, int seconds)
{
    safe_print("[3] " + name + " 开始，耗时 " + std::to_string(seconds) + "s");
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
    safe_print("[3] " + name + " 完成");
}

// ============================================================
// 4. 线程 ID 和 hardware_concurrency
// ============================================================

void print_thread_info(int index)
{
    std::ostringstream oss;
    oss << "[4] 线程 " << index
        << " | id: " << std::this_thread::get_id()
        << " | hardware_concurrency: " << std::thread::hardware_concurrency();
    safe_print(oss.str());
}

// ============================================================
// 5. 线程移动
// ============================================================

void movable_task(int value)
{
    safe_print("[5] 可移动线程 - value: " + std::to_string(value));
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
    safe_print("[7] 并行累加开始");

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
    safe_print("[7] 并行累加结果: " + std::to_string(total) + " (预期: " + std::to_string(expected) + ")");
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
    safe_print("[8] 异常处理演示开始");

    std::thread t([]() {
        try
        {
            throwing_function();
        }
        catch (const std::exception& e)
        {
            safe_print("[8] 捕获线程异常: " + std::string(e.what()));
        }
    });
    t.join();

    safe_print("[8] 异常处理演示结束");
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
    std::cout << "=== std::thread 完整接口演示 ===\n\n";

    // ----------------------------------------------------------
    // 1. 基本线程创建
    // ----------------------------------------------------------
    std::cout << "--- 1. 基本线程创建 ---\n";

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
    std::cout << "\n--- 2. 参数传递 ---\n";

    int value = 10;
    std::thread t7(modify_value, std::ref(value));
    t7.join();
    std::cout << "  主线程中 value = " << value << "\n";

    std::string msg = "hello world";
    std::thread t8(by_value_copy, msg);
    t8.join();
    std::cout << "  原始字符串未变: " << msg << "\n";

    // ----------------------------------------------------------
    // 3. join 和 detach
    // ----------------------------------------------------------
    std::cout << "\n--- 3. join 和 detach ---\n";

    std::thread t9(long_task, "任务A", 1);
    std::thread t10(long_task, "任务B", 2);

    std::cout << "  t9 joinable: " << std::boolalpha << t9.joinable() << "\n";
    std::cout << "  t10 joinable: " << t10.joinable() << "\n";

    t9.join();
    std::cout << "  t9 join 后 joinable: " << t9.joinable() << "\n";

    t10.detach();
    std::cout << "  t10 detach 后 joinable: " << t10.joinable() << "\n";

    std::this_thread::sleep_for(3s);

    // ----------------------------------------------------------
    // 4. 线程 ID 和硬件并发
    // ----------------------------------------------------------
    std::cout << "\n--- 4. 线程 ID 和硬件并发 ---\n";

    std::cout << "  主线程 id: " << std::this_thread::get_id() << "\n";
    std::cout << "  硬件并发数: " << std::thread::hardware_concurrency() << "\n";

    std::vector<std::thread> info_threads;
    for (int i = 0; i < 4; ++i)
        info_threads.emplace_back(print_thread_info, i);
    for (auto& t : info_threads)
        t.join();

    // ----------------------------------------------------------
    // 5. 线程移动
    // ----------------------------------------------------------
    std::cout << "\n--- 5. 线程移动 ---\n";

    std::thread t11(movable_task, 100);
    std::thread t12 = std::move(t11);
    std::cout << "  t11 move 后 joinable: " << t11.joinable() << "\n";
    std::cout << "  t12 joinable: " << t12.joinable() << "\n";
    t12.join();

    // ----------------------------------------------------------
    // 6. RAII 线程包装器
    // ----------------------------------------------------------
    std::cout << "\n--- 6. RAII ScopedThread ---\n";

    {
        ScopedThread st(std::thread([]() {
            safe_print("[6] ScopedThread 析构时自动 join");
        }));
    }
    safe_print("[6] ScopedThread 已析构");

    // ----------------------------------------------------------
    // 7. 并行累加
    // ----------------------------------------------------------
    std::cout << "\n--- 7. 并行累加 ---\n";

    parallel_accumulate_demo();

    // ----------------------------------------------------------
    // 8. 异常处理
    // ----------------------------------------------------------
    std::cout << "\n--- 8. 异常处理 ---\n";

    exception_demo();

    // ----------------------------------------------------------
    // 9. 数据竞争（不安全）
    // ----------------------------------------------------------
    std::cout << "\n--- 9. 数据竞争（不安全）---\n";

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
    std::cout << "  不安全计数器: " << unsafe_counter
              << " (预期: " << num_threads * iterations << ")\n";

    // ----------------------------------------------------------
    // 10. 数据竞争（安全，使用 mutex）
    // ----------------------------------------------------------
    std::cout << "\n--- 10. 数据竞争（安全，mutex）---\n";

    int safe_counter = 0;
    std::mutex counter_mutex;

    {
        std::vector<std::thread> threads;
        for (int i = 0; i < num_threads; ++i)
            threads.emplace_back(safe_increment, std::ref(safe_counter), std::ref(counter_mutex), iterations);
        for (auto& t : threads)
            t.join();
    }
    std::cout << "  安全计数器: " << safe_counter
              << " (预期: " << num_threads * iterations << ")\n";

    // ----------------------------------------------------------
    // 总结
    // ----------------------------------------------------------
    std::cout << "\n=== 演示完成 ===\n";
    std::cout << "std::thread 核心接口：\n";
    std::cout << "  构造: function, lambda, functor, member function, static member\n";
    std::cout << "  参数: 值传递, std::ref 引用传递\n";
    std::cout << "  控制: join(), detach(), joinable()\n";
    std::cout << "  信息: get_id(), hardware_concurrency()\n";
    std::cout << "  移动: std::move() 转移所有权\n";
    std::cout << "  线程安全: std::mutex + std::lock_guard\n";

    return 0;
}
