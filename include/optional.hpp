#pragma once

#include <new>
#include <type_traits>
#include <utility>

namespace itertools {
template <typename T>
  struct OptionalDestructor {
    ~OptionalDestructor() {
      // std::cout << "OptionalDestructor called for " << typeid(T).name() << std::endl;
      if (static_cast<T*>(this)->_initialized) {
        static_cast<T*>(this)->clear();
      }
    }
  };

struct NullOpt { };

[[maybe_unused]] static inline NullOpt nullopt{};

template <typename T>
class Optional 
  : private std::conditional_t<
    std::is_trivially_destructible_v<T>,
    NullOpt,
    OptionalDestructor<Optional<T>>>
  {
friend struct OptionalDestructor<Optional<T>>;

public:
  using value_type = T;

private:

  alignas(T) std::byte _value[sizeof(T)];
  bool _initialized;

  constexpr static bool is_triv_destr = std::is_trivially_destructible_v<T>;

  void clear() {
    if constexpr (!is_triv_destr) {
      reinterpret_cast<T*>(&*_value)->~T();
    }
  }

public:
  Optional(): _initialized(false) { }
  Optional(NullOpt): _initialized(false) { }
  Optional(T val): _initialized(true) {
    // std::cout << "Optional(T val) called\n" << typeid(T).name() << std::endl;
    new(&*_value) T(std::move(val));
  }

  Optional(const Optional<T>& other) {
    if (other.has_value()) {
      new(&*_value) T(other.value());
      _initialized = true;
    } else {
      _initialized = false;
    }
  }

  Optional(Optional<T>&& other) {
    // std::cout << "Optional(Optional<T>&& other) called\n" << typeid(T).name() << std::endl;
    if (other.has_value()) {
      new(&*_value) T(std::move(other).value());
      _initialized = true;
    } else {
      _initialized = false;
    }
  }

  Optional<T>& operator=(const Optional<T>& other) {
    // std::cout << "Optional<T>& operator=(const Optional<T>& other) called\n" << typeid(T).name() << std::endl;
    if (has_value()) {
      clear();
    }
    if (other.has_value()) {
      new(&*_value) T(other.value());
      _initialized = true;
    } else {
      _initialized = false;
    }
    return *this;
  }

  Optional<T>& operator=(Optional<T>&& other) {
    // std::cout << "Optional<T>& operator=(Optional<T>&& other) called\n" << typeid(T).name() << std::endl;
    if (_initialized) {
      clear();
    }
    if (other.has_value()) {
      new(&*_value) T(std::move(other).value());
      _initialized = true;
    } else {
      _initialized = false;
    }
    return *this;
  }

  operator bool() const noexcept {
    return _initialized;
  }

  bool has_value() const noexcept {
    return _initialized;
  }

  const T& value() const& {
    return *reinterpret_cast<const T*>(&*_value);
  }

  T& value() & {
    return *reinterpret_cast<T*>(&*_value);
  }

  T value() && {
    _initialized = false;
    if constexpr (is_triv_destr) {
      return std::move(*reinterpret_cast<T*>(&*_value));
    } else {
      IT_DEFER([this]() {
        clear();
      });
      return std::move(*reinterpret_cast<T*>(&*_value));
    }
  }
};
}  // namespace itertools
