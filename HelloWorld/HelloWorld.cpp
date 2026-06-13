// HelloWorld.cpp: 定义应用程序的入口点。
//

#include "HelloWorld.h"
#include "sequential_quick_sort.h"
#include <iostream>
#include <thread>
#include <format>
#include <print>
#include <vector>
#include <chrono>
#include <numeric>
#include <random>

void increment_value(int& i)
{
	i++;
}

void hello1(int *i)
{
	++(*i);
	std::println("Hello Concurrent World. Thread ID: {}\n", std::this_thread::get_id());
	std::println("{} Input int: {}\n", __FUNCTION__, (*i));
}

template<typename Iter, typename T>
struct accumulate_block
{
	void operator()(Iter first, Iter last, T& result)
	{
		result = std::accumulate(first, last, result);
	}
};

template<typename Iter, typename T>
T parallel_accumulate(Iter first, Iter last, T init)
{
	unsigned long const length = std::distance(first, last);

	if (!length)
		return init;

	unsigned long const min_per_thread = 25;
	unsigned long const max_threads =
		(length + min_per_thread - 1) / min_per_thread;

	unsigned long const hardware_threads =
		std::thread::hardware_concurrency();

	unsigned long const num_threads =
		std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);

	unsigned long const block_size = length / num_threads;

	std::vector<T> results(num_threads);
	std::vector<std::thread> threads(num_threads - 1);

	Iter block_start = first;
	for (unsigned long i = 0; i < (num_threads - 1); ++i)
	{
		Iter block_end = block_start;
		std::advance(block_end, block_size);
		threads[i] = std::thread(
			accumulate_block<Iter, T>(), block_start, block_end, std::ref(results[i]));
		block_start = block_end;
	}
	accumulate_block<Iter, T>()(block_start, last, results[num_threads - 1]);

	for (unsigned long i = 0; i < (num_threads - 1); ++i)
		threads[i].join();

	return std::accumulate(results.begin(), results.end(), init);
}

template<typename Func, typename... Args>
auto measure_time(Func&& func, Args&&... args)
{
	auto start = std::chrono::high_resolution_clock::now();
	auto result = std::forward<Func>(func)(std::forward<Args>(args)...);
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	
	std::println("Execution time: {} microseconds", duration.count());
	return result;
}

int main()
{
	std::println("Hello Main World. Thread ID: {}\n", std::this_thread::get_id());
	const size_t size = 20;
	std::list<int> num_list;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dist(0, static_cast<int>(size));
	for (size_t i = 0; i < size; i++)
	{
		num_list.emplace_back(dist(gen));
	}
	std::println("Before sort: {}", num_list);
	sequential_quick_sort_inplace_V2(num_list);
	#if 0
	auto pivot = num_list.front();
    auto divide_point = std::partition(num_list.begin(), num_list.end(),
        [&](int const& t) { return t < pivot; });
	std::println("Divide point: {} value: {} - {}", std::distance(num_list.begin(), divide_point), *divide_point, pivot);
	#endif
	std::println("After sort: {}", num_list);

	if(0)
	{
		int value = 0;
		std::vector<int> num_vec;
		num_vec.reserve(size);
		for (size_t i = 0; i < size; i++)
		{
			num_vec.emplace_back(i);
		}

		value = measure_time(parallel_accumulate<std::vector<int>::iterator, int>, num_vec.begin(), num_vec.end(), 0);
		std::println("Final value: {}\n", value);
	}

	if(0)
	{
		int value = 0;
		std::vector<int> num_vec;
		num_vec.reserve(size);
		for (size_t i = 0; i < size; i++)
		{
			num_vec.emplace_back(i);
		}

		value = measure_time([&](){
			int count = 0;
			// OpenMP并行for循环，使用reduction来累加结果
			#pragma omp parallel for reduction(+:count)
			for(int i = 0; i < num_vec.size(); i++)
			{
				count += num_vec[i];
			}
			return count;
		});
		std::println("Final value: {}\n", value);
	}
	return 0;
}
