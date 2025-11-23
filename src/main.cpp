#include <iostream>
#include <vector>
#include "itertools.hpp"

using itertools::iter;
using itertools::Count;
using itertools::Sum;
using itertools::Map;
using itertools::Filter;
using itertools::Unique;
using itertools::ForEach;
using itertools::Zip;
using itertools::Collect;

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
  return 0;
}
