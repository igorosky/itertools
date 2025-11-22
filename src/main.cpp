#include <iostream>
#include <vector>
#include "itertools.hpp"

using itertools::iter;

struct Sum {
  template <typename Iter>
  int operator()(itertools::Iterable<Iter>&& iter) const {
    typename itertools::Iterable<Iter>::Iterator::value_type total = 0;
    for (auto x : iter) {
      total += x;
    }
    return total;
  }
};

struct Map {
  template <typename Iter, typename Func>
  auto operator()(itertools::Iterable<Iter>&& iter, Func&& func) const {
    return itertools::Iterable{
      [iter = std::move(iter.begin()), func = std::move(func)]() mutable {
        auto next_val = iter.next();
        if (next_val.has_value()) {
          return itertools::Optional{ func(next_val.value()) };
        }
        return itertools::Optional<typename std::invoke_result_t<Func, typename itertools::Iterable<Iter>::Iterator::value_type>>{ };
      }
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
  return 0;
}
