#include <iostream>
#include <vector>
#include "itertools.hpp"

using itertools::iter;

struct Count {
  template <typename T, typename Iter>
  auto operator()(itertools::Iterable<T, Iter> iter) const {
    size_t count = 0;
    auto it = iter.begin();
    while (it.next().has_value()) {
      ++count;
    }
    return count;
  }
};

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
        while (next_val.has_value() && !func(iter.get_val(std::move(next_val)))) {
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

struct ForEach {
  template <typename T, typename Iter, typename Func>
  void operator()(itertools::Iterable<T, Iter> iter, Func&& func) const {
    auto it = iter.begin();
    while (auto val = it.next()) {
      func(it.get_val(std::move(val)));
    }
  }
};

struct Zip {
  template <typename T, typename Iter, typename Y, typename OtherIter>
  auto operator()(itertools::Iterable<T, Iter> iter, itertools::Iterable<Y, OtherIter> other) const {
    using iter_type = decltype(iter.begin());
    using other_type = decltype(other.begin());
    using pair_type = std::pair<decltype(*std::declval<iter_type>()), decltype(*std::declval<other_type>())>;
    struct Zipper {
      iter_type iter1;
      other_type iter2;
      auto operator()() {
        auto val1 = iter1.next();
        auto val2 = iter2.next();
        if (val1.has_value() && val2.has_value()) {
          return std::optional<pair_type>{
            { iter1.get_val(std::move(val1)), iter2.get_val(std::move(val2)) }
          };
        }
        return std::optional<pair_type>{ };
      }
    };
    return itertools::Iterable<pair_type, Zipper>{
      { iter.begin(), other.begin() }
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
  std::cout << "Count: " << iter(vec).to<Count>() << '\n';
  iter(vec)
    .to<Map>([](const auto& v) { return static_cast<double>(v) * 3.7; })
    .to<Filter>([](const auto& v) { return v > 10.0; })
    .to<ForEach>([](const auto& v) { std::cout << "Value: " << v << '\n'; });
  for (auto [x, y] : iter(vec).to<Zip>(iter(collected))) {
    std::cout << "Pair: " << x << ", " << y << '\n';
  }
  return 0;
}
