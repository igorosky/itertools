#pragma once

#include <optional>
#include <type_traits>
#include <utility>
#include "defer.hpp"
#include "optional.hpp"

namespace itertools {
struct DefaultEvaluator {
  template <typename Iter>
  void operator()(Iter& iter) const {
    ++iter;
  }
};

template <typename T, typename Generator>
class Iterable {
  using type = std::conditional_t<
    std::is_lvalue_reference_v<T>,
    std::remove_reference_t<T>*,
    std::remove_reference_t<T>
  >;
  static_assert(std::is_invocable_r_v<Optional<type>, Generator>,
    "Generator must be callable and return Optional<T>");

public:
  class Iterator {
  public:
    using value_type = T;

  private:
    friend class Iterable<T, Generator>;

    Optional<type> _value;
    Optional<Generator> _generator;


    Iterator() { }

    Iterator(Generator&& generator)
    : _value(generator()),
      _generator(std::move(generator)) { }

  public:
    void operator++() {
      _value = _generator.value()();
    }
    value_type operator*() {
      if constexpr (std::is_lvalue_reference_v<T>) {
        return *(_value.value());
      } else {
        return std::move(_value).value();
      }
    }
    bool operator!=(const Iterator& other) const {
      return _value.has_value() != other._value.has_value();
    }
    Optional<type> next() {
      IT_DEFER(([this, not_last = _value.has_value()]() {
        if (not_last) {
          ++(*this);
        }
      }));
      return Optional<type>(std::move(_value));
    }
  };

private:
  Iterator _begin;
  Iterator _end;

public:
  Iterable(Generator&& generator)
    : _begin(std::move(generator)) { }

  // Shall be called once
  Iterator begin() {
    return std::move(_begin);
  }
  // Shall be called once
  Iterator end() const {
    return std::move(_end);
  }

  template <typename E, typename ... Args>
  auto to(Args&& ... args) && {
    return E{}(std::move(*this), std::forward<Args>(args)...);
  }
};

// template <typename Iter>
// auto iter(const Iter& iter) {
//   using value_type = typename decltype(std::declval<Iter>().begin())::value_type;
//   return Iterable{ 
//     [begin = iter.begin(), end = iter.end()]() mutable
//       -> Optional<value_type>{
//         if (begin != end) {
//           IT_DEFER([&begin]() { ++begin; });
//           return Optional<value_type>(*begin);
//         }
//         return nullopt;
//       }
//    };
// }

template <typename Iter>
auto iter(Iter&& iter) {
  using value_type = typename decltype(std::declval<Iter>().begin())::value_type*;
  auto generator = [begin = iter.begin(), end = iter.end()]() mutable
    -> Optional<value_type> {
      if (begin != end) {
        IT_DEFER([&begin]() { ++begin; });
        return Optional<value_type>(&*begin);
      }
      return nullopt;
    };
  return Iterable<decltype(*std::declval<Iter>().begin()), decltype(generator)>{
    std::move(generator)
  };
}

// template <typename Iter>
// Iterable<Iter> iter(Iter&& begin, Iter&& end) {
//   using value_type = typename decltype(begin)::value_type*;
//   return Iterable{ 
//     [begin = std::move(begin), end = std::move(end)]() mutable {
//         if (begin != end) {
//           IT_DEFER([&begin]() { ++begin; });
//           return Optional<value_type>(&*begin);
//         }
//         return nullopt;
//       }
//    };
// }
}  // namespace itertools
