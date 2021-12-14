#include <algorithm>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using PolymerInstructions = std::map<std::string, std::string>;

using boost::multiprecision::uint128_t;
using PolymerPair = std::string;
using PolymerMap = std::map<PolymerPair, uint128_t>;

int main(int /*argc*/, char **argv) {
  std::stringstream ss;
  {
    std::ifstream file(argv[1]);
    if (!file.is_open()) {
      throw std::runtime_error("Could not open file");
    }
    ss << file.rdbuf();
  }
  std::vector<std::string> lines;
  boost::split(lines, ss.str(), boost::is_any_of("\n"));

  std::string polymer = lines[0];

  PolymerInstructions insertionMap;
  for (size_t i = 1; i < lines.size(); ++i) {
    if (lines[i].empty()) {
      continue;
    }
    std::vector<std::string> tokens;
    boost::split(tokens, lines[i], boost::is_any_of(" -> "),
                 boost::token_compress_on);
    insertionMap.emplace(tokens[0], tokens[1]);
  }

  PolymerMap polyCount;
  for (size_t i = 0; i < polymer.size() - 1; ++i) {
    std::string pair = polymer.substr(i, 2);
    if (polyCount.contains(pair)) {
      polyCount[pair]++;
    } else {
      polyCount.emplace(pair, 1);
    }
  }

  for (int i = 0; i < 40; i++) {
    PolymerMap newPolyCount;
    for (auto &kv : polyCount) {
      auto c = insertionMap[kv.first];
      std::string poly{kv.first[0] + c};
      if (!newPolyCount.contains(poly)) {
        newPolyCount[poly] = 0;
      }
      newPolyCount[poly] += polyCount[kv.first];
      poly = std::string{c + kv.first[1]};
      if (!newPolyCount.contains(poly)) {
        newPolyCount[poly] = 0;
      }
      newPolyCount[poly] += polyCount[kv.first];
    }
    polyCount = newPolyCount;
  }
  std::map<char, uint128_t> frequencyCount;
  std::vector<uint128_t> frequencies;
  for (size_t index = 0; const auto &kv : polyCount) {
    frequencyCount[kv.first[0]] += kv.second;
    ++index;
  }
  frequencyCount[polymer[polymer.size() - 1]]++;
  for (const auto &kv : frequencyCount) {
    frequencies.push_back(kv.second);
  }

  uint128_t max = *std::max_element(frequencies.begin(), frequencies.end());
  uint128_t min = *std::min_element(frequencies.begin(), frequencies.end());

  std::cout << max - min << std::endl;

  return 0;
}