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

template <typename Generator>
class Iterable {
public:
  class Iterator {
  public:
    using value_type = typename decltype(std::declval<Generator>()())::value_type;

  private:
    friend class Iterable<Generator>;

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

template <typename Iter>
auto iter(const Iter& iter) {
  using value_type = typename decltype(std::declval<std::decay_t<Iter>>().begin())::value_type;
  return Iterable{ 
    [begin = iter.begin(), end = iter.end()]()
          mutable -> Optional<value_type> {
        if (begin != end) {
          IT_DEFER([&begin]() { ++begin; });
          return Optional<value_type>(*begin);
        }
        return { };
      }
   };
}

template <typename Iter>
auto iter(Iter&& iter) {
  using value_type = typename decltype(std::declval<std::decay_t<Iter>>().begin())::value_type;
  return Iterable{ 
    [begin = iter.begin(), end = iter.end()]()
          mutable -> Optional<value_type> {
        if (begin != end) {
          IT_DEFER([&begin]() { ++begin; });
          return Optional<value_type>(*begin);
        }
        return { };
      }
   };
}

template <typename Iter>
Iterable<Iter> iter(Iter&& begin, Iter&& end) {
  using value_type = typename decltype(begin)::value_type;
  return Iterable{ 
    [begin = std::move(begin), end = std::move(end)]()
          mutable -> Optional<value_type> {
        if (begin != end) {
          IT_DEFER([&begin]() { ++begin; });
          return Optional<value_type>(*begin);
        }
        return { };
      }
   };
}
}  // namespace itertools
