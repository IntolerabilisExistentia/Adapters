#include <gtest/gtest.h>
#include <vector>
#include <map>
#include <list>
#include <string>
#include <algorithm>
#include <iterator>

#include "../lib/Adapters.hpp"

int square(int x) {
  return x * x;
}

bool is_even(int x) {
  return x % 2 == 0;
}

TEST(TransformAdapterTest, TransformSquares) {
  std::vector<int> vec = {1, 2, 3, 4, 5};

  std::vector<int> expected = {1, 4, 9, 16, 25};

  int index = 0;
  for (auto value : vec | transform(square)) {
    EXPECT_EQ(value, expected[index++]);
  }
}

TEST(FilterAdapterTest, FilterEvens) {
  std::vector<int> vec = {1, 2, 3, 4, 5};

  std::vector<int> expected = {2, 4};

  int index = 0;
  for (auto value : vec | filter(is_even)) {
    EXPECT_EQ(value, expected[index++]);
  }
}

TEST(TakeAdapterTest, TakeNElements) {
  std::vector<int> vec = {1, 2, 3, 4, 5};

  std::vector<int> expected = {1, 2, 3};

  int index = 0;
  for (auto value : vec | take(3)) {
    EXPECT_EQ(value, expected[index++]);
  }
}

TEST(DropAdapterTest, DropNElements) {
  std::vector<int> vec = {1, 2, 3, 4, 5};

  std::vector<int> expected = {3, 4, 5};

  int index = 0;
  for (auto value : vec | drop(2)) {
    EXPECT_EQ(value, expected[index++]);
  }
}

TEST(ReverseAdapterTest, ReverseElements) {
  std::vector<int> vec = {1, 2, 3, 4, 5};

  std::vector<int> expected = {5, 4, 3, 2, 1};

  int index = 0;
  for (auto value : vec | reverse()) {
    EXPECT_EQ(value, expected[index++]);
  }
}

TEST(KeysAdapterTest, ExtractKeysFromMap) {
  std::map<int, std::string> my_map = {{1, "one"}, {2, "two"}, {3, "three"}};

  std::vector<int> expected = {1, 2, 3};

  int index = 0;
  for (auto value : my_map | keys()) {
    EXPECT_EQ(value, expected[index++]);
  }
}

TEST(ValuesAdapterTest, ExtractValuesFromMap) {
  std::map<int, std::string> my_map = {{1, "one"}, {2, "two"}, {3, "three"}};

  std::vector<std::string> expected = {"one", "two", "three"};

  int index = 0;
  for (auto value : my_map | values()) {
    EXPECT_EQ(value, expected[index++]);
  }
}

TEST(MixedAdapterTest, CombineAdapters1) {
  std::map<int, std::string> my_map = {{1, "one"}, {2, "two"}, {3, "three"}};

  std::vector<int> expected = {4, 9};

  int index = 0;
  for (auto value : my_map | keys() | transform(square) | drop(1)) {
    EXPECT_EQ(value, expected[index++]);
  }
}

TEST(MixedAdapterTest, CombineAdapters2) {
  std::map<int, std::string> my_map = {{1, "one"}, {2, "two"}, {3, "three"}};

  std::vector<int> expected = {2};

  int index = 0;
  for (auto value : my_map | keys() | filter(is_even)) {
    EXPECT_EQ(value, expected[index++]);
  }
}

TEST(MixedAdapterTest, CombineAdapters3) {
  std::vector<int> vec = {1, 2, 3, 4, 5};

  std::vector<int> expected = {5, 4, 3};

  int index = 0;
  for (auto value : vec | reverse() | take(3)) {
    EXPECT_EQ(value, expected[index++]);
  }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
