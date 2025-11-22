#include <iostream>
#include <vector>
#include "itertools.hpp"

using itertools::iter;

struct Sum {
  template <typename T, typename Iter>
  int operator()(itertools::Iterable<T, Iter>&& iter) const {
    std::remove_reference_t<typename itertools::Iterable<T, Iter>::Iterator::value_type> total = 0;
    for (const auto& x : iter) {
      total += x;
    }
    return total;
  }
};

struct Map {
  template <typename T, typename Iter, typename Func>
  auto operator()(itertools::Iterable<T, Iter>&& iter, Func&& func) const {
    auto f = [iter = std::move(iter.begin()), func = std::move(func)]() mutable {
        auto next_val = iter.next();
        if (next_val.has_value()) {
          return itertools::Optional{ func(*next_val.value()) };
        }
        return itertools::Optional<typename std::invoke_result_t<Func, std::remove_pointer_t<typename itertools::Iterable<T, Iter>::Iterator::value_type>>>{ };
      };
    return itertools::Iterable<typename std::invoke_result_t<decltype(f)>::value_type, decltype(f)>{
      std::move(f)
    };
  }
};

int main() {
  std::vector<int> vec = {1, 2, 3, 4, 5};
  auto x = iter(vec).to<Sum>();
  std::cout << "Sum: " << x << '\n';
  for (auto x : iter(vec).to<Map>([](int v) { return static_cast<double>(v * v); })) {
    std::cout << x << '\n';
  }
  std::cout << 
    iter(vec)
      .to<Map>([](int v) { return v * v; })
      .to<Sum>()
    << '\n';
  for (auto& x : iter(vec)) {
    x += 1;
  }
  for (auto x : vec) {
    std::cout << x << '\n';
  }
  return 0;
}
