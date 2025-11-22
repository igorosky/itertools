#include <iostream>
#include <vector>
#include "itertools.hpp"

using itertools::iter;

struct Sum {
  template <typename T, typename Iter>
  auto operator()(itertools::Iterable<T, Iter> iter) const {
    std::remove_reference_t<typename itertools::Iterable<T, Iter>::Iterator::value_type> total = 0;
    for (const auto& x : iter) {
      total += x;
    }
    return total;
  }
};

struct Map {
  template <typename T, typename Iter, typename Func>
  auto operator()(itertools::Iterable<T, Iter> it, Func&& func) const {
    using new_value_type = std::invoke_result_t<Func, decltype(*it.begin())>;
    static_assert(!std::is_reference_v<new_value_type>,
      "Mapping function must not return a reference type");

    struct Mapper {
      decltype(it.begin()) iter;
      std::remove_reference_t<Func> func;

      auto operator()() {
        auto next_val = iter.next();
        if (next_val.has_value()) {
          return std::optional{ func(*next_val.value()) };
        }
        return std::optional<typename std::invoke_result_t<Func, std::remove_pointer_t<typename itertools::Iterable<T, Iter>::Iterator::value_type>>>{ };
      }
    };
    return itertools::Iterable<typename std::invoke_result_t<Mapper>::value_type, Mapper>{
      { it.begin(), std::move(func) }
    };
  }
};

struct Filter {
  template <typename T, typename Iter, typename Func>
  auto operator()(itertools::Iterable<T, Iter> it, Func&& func) const {
    struct Filterer {
      decltype(it.begin()) iter;
      std::remove_reference_t<Func> func;

      auto operator()() {
        auto next_val = iter.next();
        auto get_val = [](std::optional<typename itertools::Iterable<T, Iter>::Iterator::stored_type>&& opt) {
          if constexpr (std::is_lvalue_reference_v<T>) {
            return *opt.value();
          } else {
            return std::move(opt).value();
          }
        };
        while (next_val.has_value() && !func(get_val(std::move(next_val)))) {
          next_val = iter.next();
        }
        return next_val;
      }
    };
    return itertools::Iterable<T, Filterer>{
      { std::move(it.begin()), std::move(func) }
    };
  }
};

template <typename Target>
struct Collect {
  template <typename T, typename Iter>
  auto operator()(itertools::Iterable<T, Iter> iter) const {
    return Target(iter.begin(), iter.end());
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
  for (auto& x : iter(vec).to<Filter>([](const int& v) { return v % 2 == 1; })) {
    x += 1;
  }
  for (auto x : vec) {
    std::cout << x << '\n';
  }
  auto collected = iter(vec).to<Collect<std::vector<int>>>();
  for (auto x : collected) {
    std::cout << x << '\n';
  }
  return 0;
}
