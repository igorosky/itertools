#include <gtest/gtest.h>

#include <vector>

#include <itertools.hpp>

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

TEST(BasicTest, FilterTransformation) {
  std::vector<int> vec = { 1, 2, 3, 4, 5, 6 };
  std::vector<int> expected = { 2, 4, 6 };
  size_t j = 0;
  for (int i : itertools::iter(vec).to<itertools::transformations::Filter>(
      [](int x) { return x % 2 == 0; })) {
    EXPECT_EQ(i, expected[j]);
    ++j;
  }
}

TEST(BasicTest, CountTransformation) {
  std::vector<int> vec = { 1, 2, 3, 4, 5 };
  size_t count = itertools::iter(vec).to<itertools::transformations::Count>();
  EXPECT_EQ(count, 5);
}

TEST(BasicTest, EmptyContainer) {
  std::vector<int> vec;
  size_t count = itertools::iter(vec).to<itertools::transformations::Count>();
  EXPECT_EQ(count, 0);
}

TEST(BasicTest, SingleElement) {
  std::vector<int> vec = { 42 };
  int sum = itertools::iter(vec).to<itertools::transformations::Sum>();
  EXPECT_EQ(sum, 42);
}

TEST(BasicTest, ForEachTransformation) {
  std::vector<int> vec = { 1, 2, 3 };
  int sum = 0;
  itertools::iter(vec).to<itertools::transformations::ForEach>(
    [&sum](int x) { sum += x; });
  EXPECT_EQ(sum, 6);
}

TEST(BasicTest, CollectTransformation) {
  std::vector<int> vec = { 3, 1, 4, 1, 5 };
  auto collected = itertools::iter(vec)
    .to<itertools::transformations::Collect<std::vector<int>>>();
  ASSERT_EQ(collected.size(), 5);
  EXPECT_EQ(collected[0], 3);
  EXPECT_EQ(collected[1], 1);
  EXPECT_EQ(collected[2], 4);
}
