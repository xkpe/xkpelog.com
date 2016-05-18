#include <algorithm>
#include <chrono>
#include <future>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <random>
#include <vector>

using namespace std;

constexpr auto NUMBERS_SIZE = size_t(1e7);
constexpr auto MAX_PART = 1u << 14;

template <typename Iterator> void print(Iterator start, Iterator end) {
  while (start < end) {
    cout << *start << ' ';
    ++start;
  }
  cout << endl;
}

template <typename Iterator> void merge_sort(Iterator start, Iterator end) {
  auto size = distance(start, end);
  if (size <= MAX_PART) {
    sort(start, end);
  } else {
    auto middle = start + size / 2;
    merge_sort(start, middle);
    merge_sort(middle, end);
    inplace_merge(start, middle, end);
  }
}

template <typename Iterator>
void async_merge_sort(Iterator start, Iterator end) {
  auto size = distance(start, end);
  if (size <= MAX_PART) {
    sort(start, end);
  } else {
    auto middle = start + size / 2;
    auto task1 =
        async(launch::async, async_merge_sort<Iterator>, start, middle);
    auto task2 = async(launch::async, async_merge_sort<Iterator>, middle, end);
    task1.wait();
    task2.wait();
    inplace_merge(start, middle, end);
  }
}

template <typename Iterator> void quick_sort(Iterator start, Iterator end) {
  auto size = distance(start, end);
  if (size < 2)
    return;
  auto pivot = start[size/2];
  auto head = start;
  auto tail = end - 1;

  while ((head < end) && (*head == pivot))
    ++head;
  if (head == end) {
    return;
  }
  if (*head < pivot) {
    pivot = *head;
    head = start;
  }
  while (head < tail) {
    while ((head < tail) && (*head <= pivot))
      ++head;
    while ((head < tail) && (*tail > pivot))
      --tail;
    if (head >= tail)
      break;
    iter_swap(head++, tail--);
  }
  if (*head <= pivot)
    ++head;

  quick_sort<Iterator>(start, head);
  quick_sort<Iterator>(head, end);
}

template <typename Iterator>
void async_quick_sort(Iterator start, Iterator end) {
  auto size = distance(start, end);
  if (size < 2)
    return;
  auto pivot = *start;
  auto head = start;
  auto tail = end - 1;

  while ((head < end) && (*head == pivot))
    ++head;
  if (head == end) {
    return;
  }
  if (*head < pivot) {
    iter_swap(head, start);
    head = start + 1;
    pivot = *start;
  }
  while (head < tail) {
    while ((head < tail) && (*head <= pivot))
      ++head;
    while ((head < tail) && (*tail > pivot))
      --tail;
    if (head >= tail)
      break;
    iter_swap(head++, tail--);
  }
  if (*head <= pivot)
    ++head;

  if (size <= MAX_PART) {
    quick_sort<Iterator>(start, head);
    quick_sort<Iterator>(head, end);
  } else {
    auto task1 = async(launch::async, async_quick_sort<Iterator>, start, head);
    auto task2 = async(launch::async, async_quick_sort<Iterator>, head, end);
    task1.wait();
    task2.wait();
  }
}

int main() {
  auto numbers = vector<int>(NUMBERS_SIZE);

  {
    uniform_int_distribution<> distribution(numeric_limits<int>::min(),
                                            numeric_limits<int>::max());
    //    uniform_int_distribution<> distribution(-10, 10);
    auto generator = mt19937(0);
    for (auto &&n : numbers) {
      n = distribution(generator);
    }
  }
  auto reference = numbers;
  sort(reference.begin(), reference.end());

  auto test = [&numbers, &reference](const string &name, auto sort_function) {
    auto copy = numbers;
    auto start = chrono::high_resolution_clock::now();

    sort_function(copy.begin(), copy.end());

    auto end = chrono::high_resolution_clock::now();
    auto seconds = chrono::duration<double>(end - start).count();
    cout << setw(20) << name << " " << seconds << "s" << endl;
    if (copy != reference) {
      cout << name << " sorting failed" << endl;
      std::copy(copy.begin(), copy.end(),
                std::ostream_iterator<int>(std::cout, " "));
      cout << endl;
      cout << "reference" << endl;
      std::copy(reference.begin(), reference.end(),
                std::ostream_iterator<int>(std::cout, " "));
      cout << endl;
    }
  };
  test("std::sort", sort<decltype(numbers.begin())>);
  test("merge_sort", merge_sort<decltype(numbers.begin())>);
  test("async_merge_sort", async_merge_sort<decltype(numbers.begin())>);
  test("quick_sort", quick_sort<decltype(numbers.begin())>);
  test("async_quick_sort", async_quick_sort<decltype(numbers.begin())>);

  return 0;
}
