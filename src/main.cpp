#include <iostream>
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

int main() {
  std::vector<int> vec = {1, 2, 3, 4, 5};
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
  for (auto& x : rangeInf(0, 2).to<FirstN>(5ul)) {
    std::cout << "RangeInf: " << x << '\n';
  }
  return 0;
}
