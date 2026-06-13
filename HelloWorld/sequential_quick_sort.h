#pragma once

#include <list>
#include <algorithm>

template<typename T>
std::list<T> sequential_quick_sort(std::list<T> input)
{
    if (input.empty())
    {
        return input;
    }

    std::list<T> result;
    // 将输入的第一个元素作为基准元素
    result.splice(result.begin(), input, input.begin());
    // 使用 const 引用，避免拷贝
    T const& pivot = result.front();

    // 分割输入列表，使得基准元素左边的元素都小于基准元素，右边的元素都大于基准元素
    auto divide_point = std::partition(input.begin(), input.end(), [&](T const& t){
        return t < pivot; // 如果元素小于基准元素，则返回 true，否则返回 false
    });

    std::list<T> lower_part;
    // 将分割后的小于基准元素的元素转移到 lower_part 列表中
    lower_part.splice(lower_part.end(), input, input.begin(), divide_point);
    auto sorted_lower_part = sequential_quick_sort(std::move(lower_part));

    std::list<T> upper_part;
    // 将分割后的大于等于基准元素的元素转移到 upper_part 列表中
    upper_part.splice(upper_part.end(), input, divide_point, input.end());
    auto sorted_upper_part = sequential_quick_sort(std::move(upper_part));

    // 将 lower_part 和 upper_part 合并到 result 列表中
    result.splice(result.end(), sorted_upper_part);
    result.splice(result.begin(), sorted_lower_part);
    return result;
}

template<typename T>
void sequential_quick_sort_inplace(std::list<T>& input)
{
    if (input.empty())
    {
        return;
    }

    // 使用 const 引用，避免拷贝
    T const& pivot = input.front();

    // 分割输入列表，使得基准元素左边的元素都小于基准元素，右边的元素都大于基准元素
    auto divide_point = std::partition(std::next(input.begin()), input.end(), [&](T const& t){
        return t < pivot; // 如果元素小于基准元素，则返回 true，否则返回 false
    });

    std::list<T> lower_part;
    lower_part.splice(lower_part.end(), input, std::next(input.begin()), divide_point);

    std::list<T> upper_part;
    upper_part.splice(upper_part.end(), input, divide_point, input.end());

    sequential_quick_sort_inplace(lower_part);
    sequential_quick_sort_inplace(upper_part);

    input.splice(input.begin(), lower_part);
    input.splice(input.end(), upper_part);
}

template<typename T>
void sequential_quick_sort_range(
    std::list<T>& input,
    typename std::list<T>::iterator first,
    typename std::list<T>::iterator last)
{
    if (std::distance(first, last) <= 1)
        return;

    T const& pivot = *first;
    auto divide_point = std::partition(first, last,
        [&](T const& t) { return t < pivot; });

    sequential_quick_sort_range(input, first, divide_point);
    auto pivot_pos = (pivot == *divide_point) ? std::next(divide_point) : divide_point;
    sequential_quick_sort_range(input, pivot_pos, last);
}

template<typename T>
void sequential_quick_sort_inplace_V2(std::list<T>& input)
{
    sequential_quick_sort_range(input, input.begin(), input.end());
}