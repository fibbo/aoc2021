#include <algorithm>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

using PolymerInstructions = std::unordered_map<std::string, std::string>;

using PolymerPair = std::string;
using PolymerMap = std::unordered_map<PolymerPair, uint64_t>;

int main(int /*argc*/, char *argv[]) {
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
  std::string poly;
  poly.resize(2);
  for (int i = 0; i < 40; i++) {
    PolymerMap newPolyCount;
    for (auto &kv : polyCount) {
      auto c = insertionMap[kv.first];
      poly = {kv.first[0] + c};
      if (!newPolyCount.contains(poly)) {
        newPolyCount[poly] = 0;
      }
      newPolyCount[poly] += kv.second;
      poly = {c + kv.first[1]};
      if (!newPolyCount.contains(poly)) {
        newPolyCount[poly] = 0;
      }

      newPolyCount[poly] += kv.second;
    }
    polyCount = newPolyCount;
  }
  std::map<char, uint64_t> frequencyCount;
  std::vector<uint64_t> frequencies;
  for (size_t index = 0; const auto &kv : polyCount) {
    frequencyCount[kv.first[0]] += kv.second;
    ++index;
  }
  frequencyCount[polymer[polymer.size() - 1]]++;
  for (const auto &kv : frequencyCount) {
    frequencies.push_back(kv.second);
  }

  uint64_t max = *std::max_element(frequencies.begin(), frequencies.end());
  uint64_t min = *std::min_element(frequencies.begin(), frequencies.end());

  std::cout << max - min << std::endl;

  return 0;
}