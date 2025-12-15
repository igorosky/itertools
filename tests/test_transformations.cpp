#include <gtest/gtest.h>
#include <itertools.hpp>
#include <vector>
#include <list>
#include <deque>
#include <string>

// Zip transformation tests
TEST(TransformationsTest, ZipEqualLength) {
  std::vector<int> vec1 = { 1, 2, 3 };
  std::vector<int> vec2 = { 10, 20, 30 };
  std::vector<std::pair<int, int>> expected = { {1, 10}, {2, 20}, {3, 30} };
  
  size_t j = 0;
  for (auto [a, b] : itertools::iter(vec1)
      .to<itertools::transformations::Zip>(itertools::iter(vec2))) {
    EXPECT_EQ(a, expected[j].first);
    EXPECT_EQ(b, expected[j].second);
    ++j;
  }
  EXPECT_EQ(j, 3);
}

TEST(TransformationsTest, ZipFirstShorter) {
  std::vector<int> vec1 = { 1, 2 };
  std::vector<int> vec2 = { 10, 20, 30, 40 };
  
  size_t count = 0;
  for (auto [a, b] : itertools::iter(vec1)
      .to<itertools::transformations::Zip>(itertools::iter(vec2))) {
    (void)a;
    (void)b;
    ++count;
  }
  EXPECT_EQ(count, 2);
}

TEST(TransformationsTest, ZipSecondShorter) {
  std::vector<int> vec1 = { 1, 2, 3, 4 };
  std::vector<int> vec2 = { 10, 20 };
  
  size_t count = 0;
  for (auto [a, b] : itertools::iter(vec1)
      .to<itertools::transformations::Zip>(itertools::iter(vec2))) {
    (void)a;
    (void)b;
    ++count;
  }
  EXPECT_EQ(count, 2);
}

TEST(TransformationsTest, ZipBothEmpty) {
  std::vector<int> vec1;
  std::vector<int> vec2;
  
  size_t count = 0;
  for (auto [a, b] : itertools::iter(vec1)
      .to<itertools::transformations::Zip>(itertools::iter(vec2))) {
    (void)a;
    (void)b;
    ++count;
  }
  EXPECT_EQ(count, 0);
}

TEST(TransformationsTest, ZipWithRanges) {
  std::vector<int> vec = { 10, 20, 30 };
  std::vector<std::pair<int, int>> expected = { {0, 10}, {1, 20}, {2, 30} };
  
  size_t j = 0;
  for (auto [idx, val] : itertools::range(0, 10)
      .to<itertools::transformations::Zip>(itertools::iter(vec))) {
    EXPECT_EQ(idx, expected[j].first);
    EXPECT_EQ(val, expected[j].second);
    ++j;
  }
  EXPECT_EQ(j, 3);
}

TEST(TransformationsTest, UniqueAllDifferent) {
  std::vector<int> vec = { 1, 2, 3, 4, 5 };
  std::vector<int> expected = { 1, 2, 3, 4, 5 };
  
  size_t j = 0;
  for (int i : itertools::iter(vec)
      .to<itertools::transformations::Unique>()) {
    EXPECT_EQ(i, expected[j]);
    ++j;
  }
  EXPECT_EQ(j, 5);
}

TEST(TransformationsTest, UniqueWithDuplicates) {
  std::vector<int> vec = { 1, 2, 2, 3, 1, 4, 3, 5 };
  std::unordered_set<int> seen;
  
  for (int i : itertools::iter(vec)
      .to<itertools::transformations::Unique>()) {
    EXPECT_FALSE(seen.count(i));
    seen.insert(i);
  }
  EXPECT_EQ(seen.size(), 5);
}

TEST(TransformationsTest, UniqueAllSame) {
  std::vector<int> vec = { 7, 7, 7, 7, 7 };
  
  size_t count = 0;
  for (int i : itertools::iter(vec)
      .to<itertools::transformations::Unique>()) {
    EXPECT_EQ(i, 7);
    ++count;
  }
  EXPECT_EQ(count, 1);
}

TEST(TransformationsTest, UniqueEmpty) {
  std::vector<int> vec;
  
  size_t count = 0;
  for (int i : itertools::iter(vec)
      .to<itertools::transformations::Unique>()) {
    (void)i;
    ++count;
  }
  EXPECT_EQ(count, 0);
}


// Chain transformation tests
TEST(TransformationsTest, ChainTwoVectors) {
  std::vector<int> vec1 = { 1, 2, 3 };
  std::vector<int> vec2 = { 4, 5, 6 };
  std::vector<int> expected = { 1, 2, 3, 4, 5, 6 };
  
  size_t j = 0;
  for (int i : itertools::iter(vec1)
      .to<itertools::transformations::Chain>(itertools::iter(vec2))) {
    EXPECT_EQ(i, expected[j]);
    ++j;
  }
  EXPECT_EQ(j, 6);
}

TEST(TransformationsTest, ChainFirstEmpty) {
  std::vector<int> vec1;
  std::vector<int> vec2 = { 1, 2, 3 };
  std::vector<int> expected = { 1, 2, 3 };
  
  size_t j = 0;
  for (int i : itertools::iter(vec1)
      .to<itertools::transformations::Chain>(itertools::iter(vec2))) {
    EXPECT_EQ(i, expected[j]);
    ++j;
  }
  EXPECT_EQ(j, 3);
}

TEST(TransformationsTest, ChainSecondEmpty) {
  std::vector<int> vec1 = { 1, 2, 3 };
  std::vector<int> vec2;
  std::vector<int> expected = { 1, 2, 3 };
  
  size_t j = 0;
  for (int i : itertools::iter(vec1)
      .to<itertools::transformations::Chain>(itertools::iter(vec2))) {
    EXPECT_EQ(i, expected[j]);
    ++j;
  }
  EXPECT_EQ(j, 3);
}

TEST(TransformationsTest, ChainBothEmpty) {
  std::vector<int> vec1;
  std::vector<int> vec2;
  
  size_t count = 0;
  for (int i : itertools::iter(vec1)
      .to<itertools::transformations::Chain>(itertools::iter(vec2))) {
    (void)i;
    ++count;
  }
  EXPECT_EQ(count, 0);
}

TEST(TransformationsTest, ChainWithRanges) {
  std::vector<int> expected = { 0, 1, 2, 10, 11, 12 };
  
  size_t j = 0;
  for (int i : itertools::range(0, 3)
      .to<itertools::transformations::Chain>(itertools::range(10, 13))) {
    EXPECT_EQ(i, expected[j]);
    ++j;
  }
  EXPECT_EQ(j, 6);
}

// FirstN transformation tests
TEST(TransformationsTest, FirstNBasic) {
  std::vector<int> vec = { 1, 2, 3, 4, 5 };
  std::vector<int> expected = { 1, 2, 3 };
  
  size_t j = 0;
  for (int i : itertools::iter(vec)
      .to<itertools::transformations::FirstN>(3ul)) {
    EXPECT_EQ(i, expected[j]);
    ++j;
  }
  EXPECT_EQ(j, 3);
}

TEST(TransformationsTest, FirstNZero) {
  std::vector<int> vec = { 1, 2, 3 };
  
  size_t count = 0;
  for (int i : itertools::iter(vec)
      .to<itertools::transformations::FirstN>(0ul)) {
    (void)i;
    ++count;
  }
  EXPECT_EQ(count, 0);
}

TEST(TransformationsTest, FirstNMoreThanAvailable) {
  std::vector<int> vec = { 1, 2, 3 };
  std::vector<int> expected = { 1, 2, 3 };
  
  size_t j = 0;
  for (int i : itertools::iter(vec)
      .to<itertools::transformations::FirstN>(10ul)) {
    EXPECT_EQ(i, expected[j]);
    ++j;
  }
  EXPECT_EQ(j, 3);
}

TEST(TransformationsTest, FirstNOne) {
  std::vector<int> vec = { 1, 2, 3, 4, 5 };
  
  size_t count = 0;
  int first = 0;
  for (int i : itertools::iter(vec)
      .to<itertools::transformations::FirstN>(1ul)) {
    first = i;
    ++count;
  }
  EXPECT_EQ(count, 1);
  EXPECT_EQ(first, 1);
}

TEST(TransformationsTest, FirstNWithRange) {
  std::vector<int> expected = { 0, 1, 2, 3, 4 };
  
  size_t j = 0;
  for (int i : itertools::range(0, 100)
      .to<itertools::transformations::FirstN>(5ul)) {
    EXPECT_EQ(i, expected[j]);
    ++j;
  }
  EXPECT_EQ(j, 5);
}

// SkipN transformation tests
TEST(TransformationsTest, SkipNBasic) {
  std::vector<int> vec = { 1, 2, 3, 4, 5 };
  std::vector<int> expected = { 4, 5 };
  
  size_t j = 0;
  for (int i : itertools::iter(vec)
      .to<itertools::transformations::SkipN<true>>(3ul)) {
    EXPECT_EQ(i, expected[j]);
    ++j;
  }
  EXPECT_EQ(j, 2);
}

TEST(TransformationsTest, SkipNZero) {
  std::vector<int> vec = { 1, 2, 3 };
  std::vector<int> expected = { 1, 2, 3 };
  
  size_t j = 0;
  for (int i : itertools::iter(vec)
      .to<itertools::transformations::SkipN<true>>(0ul)) {
    EXPECT_EQ(i, expected[j]);
    ++j;
  }
  EXPECT_EQ(j, 3);
}

TEST(TransformationsTest, SkipNAll) {
  std::vector<int> vec = { 1, 2, 3 };
  
  size_t count = 0;
  for (int i : itertools::iter(vec)
      .to<itertools::transformations::SkipN<true>>(3ul)) {
    (void)i;
    ++count;
  }
  EXPECT_EQ(count, 0);
}

TEST(TransformationsTest, SkipNMoreThanAvailable) {
  std::vector<int> vec = { 1, 2, 3 };
  
  size_t count = 0;
  for (int i : itertools::iter(vec)
      .to<itertools::transformations::SkipN<true>>(10ul)) {
    (void)i;
    ++count;
  }
  EXPECT_EQ(count, 0);
}

TEST(TransformationsTest, SkipNWithRange) {
  std::vector<int> expected = { 5, 6, 7, 8, 9 };
  
  size_t j = 0;
  for (int i : itertools::range(0, 10)
      .to<itertools::transformations::SkipN<true>>(5ul)) {
    EXPECT_EQ(i, expected[j]);
    ++j;
  }
  EXPECT_EQ(j, 5);
}

// Dedup transformation tests
TEST(TransformationsTest, DedupConsecutiveDuplicates) {
  std::vector<int> vec = { 1, 1, 2, 2, 2, 3, 1, 1, 4 };
  std::vector<int> expected = { 1, 2, 3, 1, 4 };
  
  size_t j = 0;
  for (int i : itertools::iter(vec)
      .to<itertools::transformations::Dedup>()) {
    EXPECT_EQ(i, expected[j]);
    ++j;
  }
  EXPECT_EQ(j, 5);
}

TEST(TransformationsTest, DedupNoDuplicates) {
  std::vector<int> vec = { 1, 2, 3, 4, 5 };
  std::vector<int> expected = { 1, 2, 3, 4, 5 };
  
  size_t j = 0;
  for (int i : itertools::iter(vec)
      .to<itertools::transformations::Dedup>()) {
    EXPECT_EQ(i, expected[j]);
    ++j;
  }
  EXPECT_EQ(j, 5);
}

TEST(TransformationsTest, DedupAllSame) {
  std::vector<int> vec = { 5, 5, 5, 5, 5 };
  
  size_t count = 0;
  for (int i : itertools::iter(vec)
      .to<itertools::transformations::Dedup>()) {
    EXPECT_EQ(i, 5);
    ++count;
  }
  EXPECT_EQ(count, 1);
}

TEST(TransformationsTest, DedupEmpty) {
  std::vector<int> vec;
  
  size_t count = 0;
  for (int i : itertools::iter(vec)
      .to<itertools::transformations::Dedup>()) {
    (void)i;
    ++count;
  }
  EXPECT_EQ(count, 0);
}

TEST(TransformationsTest, DedupSingleElement) {
  std::vector<int> vec = { 42 };
  
  size_t count = 0;
  int value = 0;
  for (int i : itertools::iter(vec)
      .to<itertools::transformations::Dedup>()) {
    value = i;
    ++count;
  }
  EXPECT_EQ(count, 1);
  EXPECT_EQ(value, 42);
}

// Map transformation tests
TEST(TransformationsTest, MapDoubling) {
  std::vector<int> vec = { 1, 2, 3, 4, 5 };
  std::vector<int> expected = { 2, 4, 6, 8, 10 };
  
  size_t j = 0;
  for (int i : itertools::iter(vec)
      .to<itertools::transformations::Map>([](int x) { return x * 2; })) {
    EXPECT_EQ(i, expected[j]);
    ++j;
  }
  EXPECT_EQ(j, 5);
}

TEST(TransformationsTest, MapTypeConversion) {
  std::vector<int> vec = { 1, 2, 3 };
  std::vector<double> expected = { 1.5, 2.5, 3.5 };
  
  size_t j = 0;
  for (double d : itertools::iter(vec)
      .to<itertools::transformations::Map>([](int x) { return x + 0.5; })) {
    EXPECT_DOUBLE_EQ(d, expected[j]);
    ++j;
  }
  EXPECT_EQ(j, 3);
}

TEST(TransformationsTest, MapEmpty) {
  std::vector<int> vec;
  
  size_t count = 0;
  for (int i : itertools::iter(vec)
      .to<itertools::transformations::Map>([](int x) { return x * 2; })) {
    (void)i;
    ++count;
  }
  EXPECT_EQ(count, 0);
}

TEST(TransformationsTest, MapToString) {
  std::vector<int> vec = { 1, 2, 3 };
  std::vector<std::string> expected = { "1", "2", "3" };
  
  size_t j = 0;
  for (std::string s : itertools::iter(vec)
      .to<itertools::transformations::Map>([](int x) { return std::to_string(x); })) {
    EXPECT_EQ(s, expected[j]);
    ++j;
  }
  EXPECT_EQ(j, 3);
}

// Filter transformation tests
TEST(TransformationsTest, FilterEvenNumbers) {
  std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8 };
  std::vector<int> expected = { 2, 4, 6, 8 };
  
  size_t j = 0;
  for (int i : itertools::iter(vec)
      .to<itertools::transformations::Filter>([](int x) { return x % 2 == 0; })) {
    EXPECT_EQ(i, expected[j]);
    ++j;
  }
  EXPECT_EQ(j, 4);
}

TEST(TransformationsTest, FilterOddNumbers) {
  std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8 };
  std::vector<int> expected = { 1, 3, 5, 7 };
  
  size_t j = 0;
  for (int i : itertools::iter(vec)
      .to<itertools::transformations::Filter>([](int x) { return x % 2 == 1; })) {
    EXPECT_EQ(i, expected[j]);
    ++j;
  }
  EXPECT_EQ(j, 4);
}

TEST(TransformationsTest, FilterNonePass) {
  std::vector<int> vec = { 1, 3, 5, 7 };
  
  size_t count = 0;
  for (int i : itertools::iter(vec)
      .to<itertools::transformations::Filter>([](int x) { return x % 2 == 0; })) {
    (void)i;
    ++count;
  }
  EXPECT_EQ(count, 0);
}

TEST(TransformationsTest, FilterAllPass) {
  std::vector<int> vec = { 2, 4, 6, 8 };
  std::vector<int> expected = { 2, 4, 6, 8 };
  
  size_t j = 0;
  for (int i : itertools::iter(vec)
      .to<itertools::transformations::Filter>([](int x) { return x % 2 == 0; })) {
    EXPECT_EQ(i, expected[j]);
    ++j;
  }
  EXPECT_EQ(j, 4);
}

TEST(TransformationsTest, FilterEmpty) {
  std::vector<int> vec;
  
  size_t count = 0;
  for (int i : itertools::iter(vec)
      .to<itertools::transformations::Filter>([](int x) { return x > 0; })) {
    (void)i;
    ++count;
  }
  EXPECT_EQ(count, 0);
}

TEST(TransformationsTest, FilterGreaterThan) {
  std::vector<int> vec = { 1, 5, 3, 8, 2, 9, 4 };
  std::vector<int> expected = { 5, 8, 9 };
  
  size_t j = 0;
  for (int i : itertools::iter(vec)
      .to<itertools::transformations::Filter>([](int x) { return x > 4; })) {
    EXPECT_EQ(i, expected[j]);
    ++j;
  }
  EXPECT_EQ(j, 3);
}

// Collect transformation tests
TEST(TransformationsTest, CollectToVector) {
  std::vector<int> vec = { 1, 2, 3, 4, 5 };
  auto collected = itertools::iter(vec)
    .to<itertools::transformations::Collect<std::vector<int>>>();
  
  ASSERT_EQ(collected.size(), 5);
  for (size_t i = 0; i < 5; ++i) {
    EXPECT_EQ(collected[i], vec[i]);
  }
}

TEST(TransformationsTest, CollectToList) {
  std::vector<int> vec = { 1, 2, 3 };
  auto collected = itertools::iter(vec)
    .to<itertools::transformations::Collect<std::list<int>>>();
  
  ASSERT_EQ(collected.size(), 3);
  auto it = collected.begin();
  for (int expected : vec) {
    EXPECT_EQ(*it, expected);
    ++it;
  }
}

TEST(TransformationsTest, CollectToDeque) {
  std::vector<int> vec = { 5, 4, 3, 2, 1 };
  auto collected = itertools::iter(vec)
    .to<itertools::transformations::Collect<std::deque<int>>>();
  
  ASSERT_EQ(collected.size(), 5);
  for (size_t i = 0; i < 5; ++i) {
    EXPECT_EQ(collected[i], vec[i]);
  }
}

TEST(TransformationsTest, CollectEmpty) {
  std::vector<int> vec;
  auto collected = itertools::iter(vec)
    .to<itertools::transformations::Collect<std::vector<int>>>();
  
  EXPECT_TRUE(collected.empty());
}

TEST(TransformationsTest, CollectFromRange) {
  auto collected = itertools::range(0, 5)
    .to<itertools::transformations::Collect<std::vector<int>>>();
  
  ASSERT_EQ(collected.size(), 5);
  for (size_t i = 0; i < 5; ++i) {
    EXPECT_EQ(collected[i], static_cast<int>(i));
  }
}

// Count transformation tests
TEST(TransformationsTest, CountBasic) {
  std::vector<int> vec = { 1, 2, 3, 4, 5 };
  size_t count = itertools::iter(vec)
    .to<itertools::transformations::Count>();
  EXPECT_EQ(count, 5);
}

TEST(TransformationsTest, CountEmpty) {
  std::vector<int> vec;
  size_t count = itertools::iter(vec)
    .to<itertools::transformations::Count>();
  EXPECT_EQ(count, 0);
}

TEST(TransformationsTest, CountSingle) {
  std::vector<int> vec = { 42 };
  size_t count = itertools::iter(vec)
    .to<itertools::transformations::Count>();
  EXPECT_EQ(count, 1);
}

TEST(TransformationsTest, CountLarge) {
  size_t count = itertools::range(0, 1000)
    .to<itertools::transformations::Count>();
  EXPECT_EQ(count, 1000);
}

// Sum transformation tests
TEST(TransformationsTest, SumBasic) {
  std::vector<int> vec = { 1, 2, 3, 4, 5 };
  int sum = itertools::iter(vec)
    .to<itertools::transformations::Sum>();
  EXPECT_EQ(sum, 15);
}

TEST(TransformationsTest, SumEmpty) {
  std::vector<int> vec;
  int sum = itertools::iter(vec)
    .to<itertools::transformations::Sum>();
  EXPECT_EQ(sum, 0);
}

TEST(TransformationsTest, SumSingle) {
  std::vector<int> vec = { 42 };
  int sum = itertools::iter(vec)
    .to<itertools::transformations::Sum>();
  EXPECT_EQ(sum, 42);
}

TEST(TransformationsTest, SumNegatives) {
  std::vector<int> vec = { -1, -2, -3, -4, -5 };
  int sum = itertools::iter(vec)
    .to<itertools::transformations::Sum>();
  EXPECT_EQ(sum, -15);
}

TEST(TransformationsTest, SumMixed) {
  std::vector<int> vec = { 10, -5, 3, -2, 7 };
  int sum = itertools::iter(vec)
    .to<itertools::transformations::Sum>();
  EXPECT_EQ(sum, 13);
}

TEST(TransformationsTest, SumDoubles) {
  std::vector<double> vec = { 1.5, 2.5, 3.5 };
  double sum = itertools::iter(vec)
    .to<itertools::transformations::Sum>();
  EXPECT_DOUBLE_EQ(sum, 7.5);
}

// ForEach transformation tests
TEST(TransformationsTest, ForEachAccumulate) {
  std::vector<int> vec = { 1, 2, 3, 4, 5 };
  int sum = 0;
  itertools::iter(vec)
    .to<itertools::transformations::ForEach>([&sum](int x) { sum += x; });
  EXPECT_EQ(sum, 15);
}

TEST(TransformationsTest, ForEachEmpty) {
  std::vector<int> vec;
  int count = 0;
  itertools::iter(vec)
    .to<itertools::transformations::ForEach>([&count](int x) { 
      (void)x;
      ++count; 
    });
  EXPECT_EQ(count, 0);
}

TEST(TransformationsTest, ForEachModifyExternal) {
  std::vector<int> vec = { 1, 2, 3 };
  std::vector<int> result;
  itertools::iter(vec)
    .to<itertools::transformations::ForEach>([&result](int x) { 
      result.push_back(x * 2); 
    });
  
  ASSERT_EQ(result.size(), 3);
  EXPECT_EQ(result[0], 2);
  EXPECT_EQ(result[1], 4);
  EXPECT_EQ(result[2], 6);
}

TEST(TransformationsTest, ForEachCount) {
  std::vector<int> vec = { 1, 2, 3, 4, 5 };
  size_t count = 0;
  itertools::iter(vec)
    .to<itertools::transformations::ForEach>([&count](int x) { 
      (void)x;
      ++count; 
    });
  EXPECT_EQ(count, 5);
}
