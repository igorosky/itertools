#include <iostream>
#include <list>
#include <string>
#include <vector>
#include "itertools.hpp"

using itertools::iter;
using itertools::range;
using itertools::rangeInf;
using itertools::transformations::Count;
using itertools::transformations::Sum;
using itertools::transformations::Map;
using itertools::transformations::Filter;
using itertools::transformations::Unique;
using itertools::transformations::ForEach;
using itertools::transformations::Zip;
using itertools::transformations::Collect;
using itertools::transformations::Chain;
using itertools::transformations::FirstN;
using itertools::transformations::Dedup;

int test() {
  std::vector<int> vec = { 10, 20, 30 };
  std::vector<std::pair<int, int>> expected = { { 0, 10 }, { 1, 20 }, { 2, 30 } };

  size_t j = 0;
  for (auto [idx, val] : itertools::range(0, 10)
    .to<itertools::transformations::Zip>(itertools::iter(vec))) {
    if (idx != expected[j].first) {
      std::cerr << "Index mismatch: got " << idx << ", expected " << expected[j].first << '\n';
      return 1;
    }
    if (val != expected[j].second) {
      std::cerr << "Value mismatch: got " << val << ", expected " << expected[j].second << '\n';
      return 1;
    }
    ++j;
  }
  if (j != 3) {
    std::cerr << "Iteration count mismatch: got " << j << ", expected 3\n";
    return 1;
  }
  std::cout << "All tests passed.\n";
  return 0;
}

int main() {
  std::vector<int> vec = { 1, 2, 3, 4, 5 };
  auto x = iter(vec).to<Sum>();
  std::cout << "Sum: " << x << '\n';
  for (auto x : iter(vec).to<Map>([](int v) { return static_cast<double>(v * v); })) {
    std::cout << x << '\n';
  }
  std::cout <<
    iter(vec)
    .to<Map>([](const int& v) { return v * v; })
    .to<Sum>()
            << '\n';
  for (auto& x : iter(vec).to<Filter>([](const int& v) { return v % 2 == 1; })) {
    x += 1;
  }
  for (auto x : vec) {
    std::cout << x << '\n';
  }
  auto collected = iter(vec)
    .to<Map>([](int x) { return x * 10; })
    .to<Collect<std::vector<int>>>();
  for (auto x : collected) {
    std::cout << x << '\n';
  }
  std::cout << "Count: " << iter(vec).to<Count>() << '\n';
  iter(vec)
  .to<Map>([](const auto& v) { return static_cast<double>(v) * 3.7; })
  .to<Filter>([](const auto& v) { return v > 10.0; })
  .to<Unique>()
  .to<ForEach>([](const auto& v) { std::cout << "Value: " << v << '\n'; });
  for (auto [x, y] : iter(vec).to<Zip>(iter(collected))) {
    std::cout << "Pair: " << x << ", " << y << '\n';
  }
  for (decltype(auto) x : iter(vec).to<Map>([](auto v) { return v; })) {
    std::cout << "Direct iter: " << x << '\n';
  }
  for (auto& x : iter(vec).to<Chain>(iter(collected))) {
    std::cout << "Chained: " << x << '\n';
  }
  for (auto& x : iter(vec).to<FirstN>(3ul)) {
    std::cout << "First N: " << x << '\n';
  }
  for (auto x : rangeInf(0, 2).to<FirstN>(5ul)) {
    std::cout << "RangeInf: " << x << '\n';
  }
  for (auto& x : iter({ 1, 2, 2, 3, 3, 3, 2, 2, 2, 4, 1, 1, 1, 4, 4, 5, 5 }).to<Dedup>()) {
    std::cout << "Dedup: " << x << '\n';
  }
  std::list<std::string> l{ "hello", "world" };
  for (const auto& x : iter(l)) {
    std::cout << "From lvalue list: " << x << '\n';
  }
  return test();
}
