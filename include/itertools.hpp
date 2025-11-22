#pragma once

#include <optional>
#include <type_traits>
#include <unordered_set>
#include <utility>

namespace itertools {

// Internal utility to implement defer semantics
namespace {
template<typename Fn>
class Defer {
  static_assert(std::is_invocable_v<Fn>, "Defer requires an invocable type");
  static_assert(std::is_same_v<std::invoke_result_t<Fn>, void>,
                "Defer function is required to return a void type");
  Fn _fn;

  // Disable copy and move semantics
  Defer(const Defer&) = delete;
  Defer& operator=(const Defer&) = delete;
  Defer(Defer&&) = delete;
  Defer& operator=(Defer&&) = delete;

public:
  [[nodiscard]] inline explicit Defer(Fn&& fn) noexcept(std::is_nothrow_move_constructible_v<Fn>)
  : _fn(std::move(fn)) { }

  inline ~Defer() noexcept(std::is_nothrow_invocable_v<Fn>) {
    _fn();
  }
};
}  // namespace

#ifdef __COUNTER__
#  define IT_COUNTER __COUNTER__
#else
#  define IT_COUNTER __LINE__
#endif

#define IT_CONCAT_HELPER(a, b) a ## b
#define IT_CONCAT(a, b) IT_CONCAT_HELPER(a, b)
#define IT_UNIQUE_NAME(prefix) IT_CONCAT(prefix, IT_COUNTER)
#define IT_DEFER(fn) \
  itertools::Defer IT_UNIQUE_NAME(it_defer)(fn);


// Iterable implementation

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

    explicit Iterator(Generator&& generator)
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
      return std::move(_value);
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
  explicit Iterable(Generator&& generator)
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


// Utility transformations


template <typename Iter>
auto iter(const Iter& iter) {
  using prev_iter = decltype(std::declval<Iter>().begin());
  using prev_value_type = decltype(*std::declval<prev_iter>());
  constexpr static bool is_lvalue_ref = std::is_lvalue_reference_v<prev_value_type>;
  using value_type = std::conditional_t<
      is_lvalue_ref,
      std::add_pointer_t<std::remove_reference_t<prev_value_type>>,
      std::remove_reference_t<prev_value_type>
    >;
  struct DefaultGenerator {
    prev_iter begin;
    prev_iter end;
    std::optional<value_type> operator()() {
      if (begin != end) {
        IT_DEFER([this]() { ++begin; });
        if constexpr (is_lvalue_ref) {
          return std::optional<value_type>(&*begin);
        } else {
          return std::optional<value_type>(*begin);
        }
      }
      return std::nullopt;
    }
  };
  return Iterable<prev_value_type, DefaultGenerator>{
    { iter.begin(), iter.end() }
  };
}

template <typename Iter>
auto iter(Iter&& iter) {
  using prev_iter = decltype(std::declval<Iter>().begin());
  using prev_value_type = decltype(*std::declval<prev_iter>());
  constexpr static bool is_lvalue_ref = std::is_lvalue_reference_v<prev_value_type>;
  using value_type = std::conditional_t<
      is_lvalue_ref,
      std::add_pointer_t<std::remove_reference_t<prev_value_type>>,
      std::remove_reference_t<prev_value_type>
    >;
  struct DefaultGenerator {
    prev_iter begin;
    prev_iter end;
    std::optional<value_type> operator()() {
      if (begin != end) {
        IT_DEFER([this]() { ++begin; });
        if constexpr (is_lvalue_ref) {
          return std::optional<value_type>(&*begin);
        } else {
          return std::optional<value_type>(*begin);
        }
      }
      return std::nullopt;
    }
  };
  return Iterable<prev_value_type, DefaultGenerator>{
    { iter.begin(), iter.end() }
  };
}

template <typename Iter>
auto iter(Iter begin, Iter end) {
  using prev_iter = decltype(begin);
  using prev_value_type = decltype(*std::declval<prev_iter>());
  constexpr static bool is_lvalue_ref = std::is_lvalue_reference_v<prev_value_type>;
  using value_type = std::conditional_t<
      is_lvalue_ref,
      std::add_pointer_t<std::remove_reference_t<prev_value_type>>,
      std::remove_reference_t<prev_value_type>
    >;
  struct DefaultGenerator {
    prev_iter begin;
    prev_iter end;
    std::optional<value_type> operator()() {
      if (begin != end) {
        IT_DEFER([this]() { ++begin; });
        if constexpr (is_lvalue_ref) {
          return std::optional<value_type>(&*begin);
        } else {
          return std::optional<value_type>(*begin);
        }
      }
      return std::nullopt;
    }
  };
  return Iterable<prev_value_type, DefaultGenerator>{
    { std::move(begin), std::move(end) }
  };
}


// Transformation implementations


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
    auto it = iter.begin();
    while (auto val = it.next()) {
      total += it.get_val(std::move(val));
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

struct Unique {
  template <typename T, typename Iter>
  auto operator()(itertools::Iterable<T, Iter> iter) const {
    using iter_type = decltype(iter.begin());

    struct Uniquifier {
      iter_type iter;
      std::unordered_set<std::remove_reference_t<T>> seen_values;

      auto operator()() {
        auto next_val = iter.next();
        while (next_val.has_value()) {
          auto val = iter.get_val(std::move(next_val));
          if (seen_values.insert(val).second) {
            return std::optional<T>{ val };
          }
          next_val = iter.next();
        }
        return std::optional<T>{ };
      }
    };
    return itertools::Iterable<T, Uniquifier>{
      { iter.begin(), { } }
    };
  }
};


}  // namespace itertools


// Cleanup internal macros
#undef IT_COUNTER
#undef IT_CONCAT_HELPER
#undef IT_CONCAT
#undef IT_UNIQUE_NAME
#undef IT_DEFER
