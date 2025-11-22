#pragma once

#include <optional>
#include <type_traits>
#include <utility>
#include "defer.hpp"

namespace itertools {
template <typename T, typename Generator>
class Iterable {
public:
  class Iterator {
    struct Empty { };
  public:
    using stored_type = std::conditional_t<
      std::is_lvalue_reference_v<T>,
      std::add_pointer_t<std::remove_reference_t<T>>,
      std::remove_reference_t<T>
    >;

    using value_type = T;
    struct iterator_category : public
      std::input_iterator_tag,
      std::conditional_t<
        !std::is_const_v<T> && std::is_lvalue_reference_v<T>,
        std::output_iterator_tag,
        Empty
      >
    { };
    using difference_type = std::ptrdiff_t;
    using pointer = std::add_pointer_t<std::remove_reference_t<value_type>>;
    using reference = std::add_lvalue_reference_t<value_type>;

    static_assert(std::is_invocable_r_v<std::optional<stored_type>, Generator>,
      "Generator must be callable and return std::optional<T> - if T is a reference, it must return std::optional<T*>");

  private:
    friend class Iterable<T, Generator>;

    std::optional<stored_type> _value;
    std::optional<Generator> _generator;


    Iterator() { }

    Iterator(Generator&& generator)
    : _value(generator()),
      _generator(std::move(generator)) { }

  public:
    Iterator& operator++() {
      _value = _generator.value()();
      return *this;
    }
    Iterator operator++(int) {
      auto temp = *this;
      _value = _generator.value()();;
      return temp;
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
    std::optional<stored_type> next() {
      IT_DEFER(([this, not_last = _value.has_value()]() {
        if (not_last) {
          ++(*this);
        }
      }));
      return std::optional<stored_type>(std::move(_value));
    }
    static value_type get_val(std::optional<stored_type>&& opt) {
      if constexpr (std::is_lvalue_reference_v<T>) {
        return *opt.value();
      } else {
        return std::move(opt).value();
      }
    }
  };

private:
  Iterator _begin;
  Iterator _end;

public:
  Iterable(Generator&& generator)
    : _begin(std::move(generator)) { }

  Iterator begin() {
    return _begin;
  }
  Iterator end() const {
    return _end;
  }

  template <typename E, typename ... Args>
  auto to(Args&& ... args) const & {
    return E{}(*this, std::forward<Args>(args)...);
  }

  template <typename E, typename ... Args>
  auto to(Args&& ... args) && {
    return E{}(std::move(*this), std::forward<Args>(args)...);
  }
};

template <typename Iter>
auto iter(const Iter& iter) {
  using prev_iter = decltype(std::declval<Iter>().begin());
  using value_type = typename prev_iter::value_type*;
  struct DefaultGenerator {
    prev_iter begin;
    prev_iter end;
    std::optional<value_type> operator()() {
      if (begin != end) {
        IT_DEFER([this]() { ++begin; });
        return std::optional<value_type>(&*begin);
      }
      return std::nullopt;
    }
  };
  return Iterable<decltype(*std::declval<Iter>().begin()), DefaultGenerator>{
    { iter.begin(), iter.end() }
  };
}

template <typename Iter>
auto iter(Iter&& iter) {
  using prev_iter = decltype(std::declval<Iter>().begin());
  using value_type = typename prev_iter::value_type*;
  struct DefaultGenerator {
    prev_iter begin;
    prev_iter end;
    std::optional<value_type> operator()() {
      if (begin != end) {
        IT_DEFER([this]() { ++begin; });
        return std::optional<value_type>(&*begin);
      }
      return std::nullopt;
    }
  };
  return Iterable<decltype(*std::declval<Iter>().begin()), DefaultGenerator>{
    { iter.begin(), iter.end() }
  };
}

template <typename Iter>
auto iter(Iter&& begin, Iter&& end) {
  using prev_iter = decltype(std::declval<Iter>().begin());
  using value_type = typename prev_iter::value_type*;
  struct DefaultGenerator {
    prev_iter begin;
    prev_iter end;
    std::optional<value_type> operator()() {
      if (begin != end) {
        IT_DEFER([this]() { ++begin; });
        return std::optional<value_type>(&*begin);
      }
      return std::nullopt;
    }
  };
  return Iterable<decltype(*std::declval<Iter>().begin()), DefaultGenerator>{
    { begin, end }
  };
}
}  // namespace itertools
