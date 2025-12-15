#include <gtest/gtest.h>
#include <itertools.hpp>
#include <vector>

TEST(BasicTest, RangeFunctionality) {
  int j = 0;
  for (int i : itertools::range(0, 5)) {
    EXPECT_EQ(i, j);
    ++j;
  }
}

TEST(BasicTest, RangeInfFunctionality) {
  int j = 10;
  for (int i : itertools::rangeInf(10)) {
    EXPECT_EQ(i, j);
    ++j;
    if (j >= 15) {
      break;
    }
  }
}

TEST(BasicTest, IteratingContainer) {
  std::vector<int> vec = { 5, 10, 15, 20 };
  size_t j = 0;
  for (int i : itertools::iter(vec)) {
    EXPECT_EQ(i, vec[j]);
    ++j;
  }
}

TEST(BasicTest, UncopiableUnmovable) {
  struct NoCopyNoMove {
    NoCopyNoMove() = default;
    NoCopyNoMove(const NoCopyNoMove&) = delete;
    NoCopyNoMove(NoCopyNoMove&&) = delete;
    NoCopyNoMove& operator=(const NoCopyNoMove&) = delete;
    NoCopyNoMove& operator=(NoCopyNoMove&&) = delete;
    int value;
  };
  std::vector<NoCopyNoMove> vec(3);
  vec[0].value = 1;
  vec[1].value = 2;
  vec[2].value = 3;
  size_t j = 0;
  for (auto& item : itertools::iter(vec)) {
    EXPECT_EQ(item.value, vec[j].value);
    ++j;
  }
}

TEST(BasicTest, InitializerList) {
  int j = 0;
  for (int i : itertools::iter({ 10, 20, 30, 40 })) {
    EXPECT_EQ(i, (j + 1) * 10);
    ++j;
  }
}

TEST(BasicTest, SumTransformation) {
  std::vector<int> vec = { 1, 2, 3, 4, 5 };
  int total = itertools::iter(vec).to<itertools::transformations::Sum>();
  EXPECT_EQ(total, 15);
}

TEST(BasicTest, MapTransformation) {
  std::vector<int> vec = { 1, 2, 3 };
  std::vector<int> expected = { 1, 4, 9 };
  size_t j = 0;
  for (int i : itertools::iter(vec).to<itertools::transformations::Map>(
        [](int x) { return x * x; })) {
    EXPECT_EQ(i, expected[j]);
    ++j;
  }
}
