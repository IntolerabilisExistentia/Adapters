#include <forward_list>
#include <map>
#include <set>
#include <iostream>
#include <vector>
#include <lib/Adapters.hpp>

int main (int argc, char *argv[]) {
  std::map<int, std::string> my_map({{1, "one"}, {2, "two"}, {3, "three"}});
  std::set<int> s = {1, 2, 3};

  std::vector<std::string> expected = {"one", "two", "three"};

  int index = 0;
  for (auto value : s | keys() | reverse()) {
    std::cout << value << ' ';
  }
  std::cout << '\n';

  return 0;
}
