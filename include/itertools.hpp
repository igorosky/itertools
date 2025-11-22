#pragma once

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

template <typename Iter, typename CRTP = void>
class Iterable {
public:
  using value_type = typename Iter::value_type;

  template<typename Generator>
  class Iterator {
    friend class Iterable<Iter>;
    Optional<value_type> _value;
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
      return std::move(_value).value();
    }
    bool operator!=(const Iterator& other) const {
      return _value.has_value() != other._value.has_value();
    }
    Optional<value_type> next() {
      IT_DEFER(([this, not_last = _value.has_value()]() {
        if (not_last) {
          ++(*this);
        }
      }));
      return Optional<value_type>(std::move(_value));
    }
  };
protected:
  Iterable() = default;

private:
  Iter _begin;
  Iter _end;

  auto get_generator_or_def() {
    if constexpr (!std::is_same_v<CRTP, void>) {
      return static_cast<CRTP*>(this)->get_generator();
    } else {
      return [begin = std::move(_begin), end = std::move(_end)]()
          mutable -> Optional<value_type> {
        if (begin != end) {
          IT_DEFER([&begin]() { ++begin; });
          return Optional<value_type>(*begin);
        }
        return { };
      };
    }
  }

public:
  Iterable(Iter&& begin, Iter&& end)
      : _begin(std::move(begin)), _end(std::move(end)) { }

  // Shall be called once
  Iterator<decltype(std::declval<Iterable<Iter, CRTP>>().get_generator_or_def())> begin() {
    return { get_generator_or_def() };
  }
  Iterator<decltype(std::declval<Iterable<Iter, CRTP>>().get_generator_or_def())> end() const {
    return { };
  }

  template <typename E, typename ... Args>
  auto to(Args&& ... args) {
    return E{}(*this, std::forward<Args>(args)...);
  }
};

template <typename Iter>
Iterable<decltype(std::declval<Iter>().begin())> iter(const Iter& iter) {
  return { iter.begin(), iter.end() };
}

template <typename Iter>
Iterable<decltype(std::declval<Iter>().begin())> iter(Iter&& iter) {
  return { iter.begin(), iter.end() };
}

template <typename Iter>
Iterable<Iter> iter(Iter&& begin, Iter&& end) {
  return { std::forward<Iter>(begin), std::forward<Iter>(end) };
}
}  // namespace itertools
