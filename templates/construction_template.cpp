#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>
#include "../library/random/rng.hpp"

int main() {
  int n; std::cin >> n; std::vector<int> answer(n); std::iota(answer.begin(), answer.end(), 0);
  // 1) greedy key  2) randomized tie-break  3) keep several starts for local search.
  ht::Xoshiro256StarStar rng(1); std::vector<std::uint64_t> tie(n);
  for (auto& x : tie) x = rng();
  std::stable_sort(answer.begin(), answer.end(), [&](int a, int b) {
    const int key_a = a; const int key_b = b;  // replace
    return key_a != key_b ? key_a < key_b : tie[a] < tie[b];
  });
  for (int x : answer) std::cout << x << ' ';
}
