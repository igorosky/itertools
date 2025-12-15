#include <gtest/gtest.h>
#include <itertools.hpp>
#include <vector>

TEST(RangeTest, BasicRange) {
  std::vector<int> expected = { 0, 1, 2, 3, 4 };
  size_t j = 0;
  for (int i : itertools::range(0, 5)) {
    EXPECT_EQ(i, expected[j]);
    ++j;
  }
  EXPECT_EQ(j, 5);
}

TEST(RangeTest, RangeWithStep) {
  std::vector<int> expected = { 0, 2, 4, 6, 8 };
  size_t j = 0;
  for (int i : itertools::range(0, 10, 2)) {
    EXPECT_EQ(i, expected[j]);
    ++j;
  }
  EXPECT_EQ(j, 5);
}

TEST(RangeTest, RangeWithNegativeStep) {
  std::vector<int> expected = { 10, 8, 6, 4, 2 };
  size_t j = 0;
  for (int i : itertools::range(10, 0, -2)) {
    EXPECT_EQ(i, expected[j]);
    ++j;
  }
  EXPECT_EQ(j, 5);
}

TEST(RangeTest, RangeStartEqualsEnd) {
  size_t count = 0;
  for (int i : itertools::range(5, 5)) {
    (void)i;
    ++count;
  }
  EXPECT_EQ(count, 0);
}

TEST(RangeTest, RangeStartGreaterThanEnd) {
  size_t count = 0;
  for (int i : itertools::range(10, 5)) {
    (void)i;
    ++count;
  }
  EXPECT_EQ(count, 0);
}

TEST(RangeTest, RangeLargeStep) {
  std::vector<int> expected = { 0, 10 };
  size_t j = 0;
  for (int i : itertools::range(0, 15, 10)) {
    EXPECT_EQ(i, expected[j]);
    ++j;
  }
  EXPECT_EQ(j, 2);
}

TEST(RangeTest, RangeStepLargerThanRange) {
  std::vector<int> expected = { 5 };
  size_t j = 0;
  for (int i : itertools::range(5, 10, 100)) {
    EXPECT_EQ(i, expected[j]);
    ++j;
  }
  EXPECT_EQ(j, 1);
}

TEST(RangeTest, RangeWithNegativeNumbers) {
  std::vector<int> expected = { -5, -4, -3, -2, -1 };
  size_t j = 0;
  for (int i : itertools::range(-5, 0)) {
    EXPECT_EQ(i, expected[j]);
    ++j;
  }
  EXPECT_EQ(j, 5);
}

TEST(RangeTest, RangeNegativeToPositive) {
  std::vector<int> expected = { -2, -1, 0, 1, 2 };
  size_t j = 0;
  for (int i : itertools::range(-2, 3)) {
    EXPECT_EQ(i, expected[j]);
    ++j;
  }
  EXPECT_EQ(j, 5);
}

TEST(RangeInfTest, InfiniteRangeStartZero) {
  int j = 0;
  for (int i : itertools::rangeInf(0)) {
    EXPECT_EQ(i, j);
    ++j;
    if (j >= 10) break;
  }
  EXPECT_EQ(j, 10);
}

TEST(RangeInfTest, InfiniteRangeStartNonZero) {
  int j = 100;
  for (int i : itertools::rangeInf(100)) {
    EXPECT_EQ(i, j);
    ++j;
    if (j >= 110) break;
  }
  EXPECT_EQ(j, 110);
}

TEST(RangeInfTest, InfiniteRangeWithStep) {
  int j = 0;
  for (int i : itertools::rangeInf(0, 5)) {
    EXPECT_EQ(i, j);
    j += 5;
    if (j >= 50) break;
  }
  EXPECT_EQ(j, 50);
}

TEST(RangeInfTest, InfiniteRangeNegativeStart) {
  int j = -10;
  for (int i : itertools::rangeInf(-10)) {
    EXPECT_EQ(i, j);
    ++j;
    if (j >= 0) break;
  }
  EXPECT_EQ(j, 0);
}

TEST(RangeInfTest, InfiniteRangeNegativeStep) {
  int j = 100;
  for (int i : itertools::rangeInf(100, -5)) {
    EXPECT_EQ(i, j);
    j -= 5;
    if (j <= 50) break;
  }
  EXPECT_EQ(j, 50);
}

TEST(RangeTest, RangeUsedInCount) {
  size_t count = itertools::range(0, 100)
    .to<itertools::transformations::Count>();
  EXPECT_EQ(count, 100);
}

TEST(RangeTest, RangeUsedInSum) {
  int sum = itertools::range(1, 6)
    .to<itertools::transformations::Sum>();
  EXPECT_EQ(sum, 15); // 1+2+3+4+5
}

TEST(RangeTest, RangeOneElement) {
  std::vector<int> expected = { 0 };
  size_t j = 0;
  for (int i : itertools::range(0, 1)) {
    EXPECT_EQ(i, expected[j]);
    ++j;
  }
  EXPECT_EQ(j, 1);
}

TEST(RangeTest, RangeLargeNumbers) {
  size_t count = 0;
  for (int i : itertools::range(1000, 2000, 100)) {
    EXPECT_EQ(i % 100, 0);
    ++count;
  }
  EXPECT_EQ(count, 10);
}
