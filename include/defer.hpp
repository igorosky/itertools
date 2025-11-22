#pragma once

#include <type_traits>

namespace itertools {
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
  [[nodiscard]] inline explicit Defer(Fn&& fn)
  : _fn(std::forward<Fn>(fn)) { }

  inline ~Defer() {
    _fn();
  }
};
}  // namespace itertools

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
