#include <algorithm>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

int main(int /*argc*/, char *argv[]) {
  std::stringstream ss;
  {
    std::ifstream file{argv[1]};
    if (!file.is_open()) {
      throw std::runtime_error{"Could not open file"};
    }

    ss << file.rdbuf();
    file.close();
  }
  std::vector<std::string> stringNumbers;
  boost::algorithm::split(stringNumbers, ss.str(), boost::is_any_of(","));

  using boost::multiprecision::uint128_t;
  std::array<uint128_t, 9> generationBins{};
  std::for_each(stringNumbers.begin(), stringNumbers.end(),
                [&](const std::string &s) { generationBins[std::stoi(s)]++; });

  for (int i = 0; i < 256; i++) {
    uint128_t nResets = 0;
    for (size_t j = 0; j < generationBins.size(); j++) {
      if (j == 0) {
        nResets = generationBins[j];
      } else {
        uint128_t currentSize = generationBins[j];
        generationBins[j - 1] = currentSize;
      }
    }
    generationBins[6] += nResets;
    generationBins[8] = nResets;
  }
  uint128_t populationCount{};
  for (const auto &n : generationBins) {
    populationCount += n;
  }
  std::cout << "The population count is: " << populationCount << std::endl;

  return 0;
}