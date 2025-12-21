# itertools

A modern, **header-only** C++17 library for functional-style iteration and transformations, inspired by Python's itertools and Rust's iterators.

## Features

- **Header-only**: Just include `itertools.hpp` and you're ready to go - no linking required
- **Zero-cost abstractions**: Lazy evaluation with minimal overhead
- **Type-safe**: Leverages C++17 type system for compile-time safety
- **Composable**: Chain multiple transformations with a fluent API
- **STL-compatible**: Works seamlessly with standard containers and algorithms

## Quick Start

```cpp
#include "itertools.hpp"
using namespace itertools;

// Sum of squares of even numbers
auto result = range(0, 10)
    .to<transformations::Filter>([](int x) { return x % 2 == 0; })
    .to<transformations::Map>([](int x) { return x * x; })
    .to<transformations::Sum>();
// result = 120 (0² + 2² + 4² + 6² + 8²)

// Iterate over containers
std::vector<int> vec = {1, 2, 3, 4, 5};
for (auto x : iter(vec).to<transformations::Map>([](int x) { return x * 2; })) {
    std::cout << x << " "; // 2 4 6 8 10
}
```

## Installation

Since this is a **header-only library**, simply:

1. Copy `include/itertools.hpp` to your project
2. Include it: `#include "itertools.hpp"`
3. Compile with C++17 or later: `-std=c++17`

No build system configuration or linking required!

## Available Transformations

### Generators
- `range(start, end, step)` - Generate sequences of numbers
- `rangeInf(start, step)` - Infinite sequences
- `iter(container)` - Create iterables from containers

### Transformations
- `Map` - Transform each element
- `Filter` - Keep elements matching a predicate
- `Zip` - Combine two iterables element-wise
- `Chain` - Concatenate iterables
- `Unique` - Remove duplicates (unordered)
- `Dedup` - Remove consecutive duplicates
- `FirstN` - Take first N elements
- `SkipN` - Skip first N elements

### Terminal Operations
- `Sum` - Sum all elements
- `Count` - Count elements
- `ForEach` - Execute function on each element
- `Collect<Container>` - Collect into a container

## Examples

### Chaining Operations
```cpp
auto result = range(1, 100)
    .to<transformations::Filter>([](int x) { return x % 3 == 0; })
    .to<transformations::FirstN>(5)
    .to<transformations::Collect<std::vector<int>>>();
// result = {3, 6, 9, 12, 15}
```

### Working with Containers
```cpp
std::vector<int> data = {1, 2, 2, 3, 3, 3, 4};
auto unique_count = iter(data)
    .to<transformations::Dedup>()
    .to<transformations::Count>();
// unique_count = 4
```

### Zipping Iterables
```cpp
std::vector<int> numbers = {10, 20, 30};
for (auto [idx, val] : range(0, 10).to<transformations::Zip>(iter(numbers))) {
    std::cout << idx << ": " << val << "\n";
}
// Output: 0: 10, 1: 20, 2: 30
```

## Building the Demo and Tests

While the library itself is header-only, this repository includes a demo and test suite:

```bash
mkdir build && cd build
cmake -DTESTING=ON ..
cmake --build .
./tests      # Run tests
./demo       # Run demo
```

## Requirements

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.22+ (only for building tests/demo)
- GTest (only for running tests)

## License

This project is licensed under the Apache License 2.0 - see the [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Feel free to open issues or submit pull requests.
