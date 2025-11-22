#include <iostream>
#include <vector>
#include "itertools.hpp"

using itertools::iter;

struct Sum {
  template <typename Iter>
  int operator()(itertools::Iterable<Iter> iter) const {
    int total = 0;
    for (auto x : iter) {
      total += x;
    }
    return total;
  }
};

// template <typename Iter, typename Func>
// struct MapIterable : public itertools::Iterable<Iter, MapIterable<Iter, Func>> {
//   using value_type = decltype(std::declval<Func>()(std::declval<typename Iter::value_type>()));


  
//   auto get_generator() {
//     return []() mutable -> itertools::Optional<value_type> {

//     }
//   }
// };

// struct Map {
//   template <typename Iter, typename Func>
//   auto operator()(itertools::Iterable<Iter> iter, Func func) const {

//   }
// };

int main() {
  std::vector<int> vec = {1, 2, 3, 4, 5};
  auto x = iter(vec).to<Sum>();
  std::cout << "Sum: " << x << '\n';
  // for (auto x : iter(vec)) {
  //   std::cout << x << '\n';
  // }
  return 0;
}
