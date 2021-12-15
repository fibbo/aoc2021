#include <algorithm>
#include <bitset>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#if defined(AOC_TEST)
constexpr size_t stringLength = 5;
#else
constexpr size_t stringLength = 12;
#endif

// Checks if for a given position of a bit the majority of the
// binary strings of a list are 1. If the majority is 1, the
// the final value is set to 1
// Example with stringLength 5 and 3 strings:
// 10101 : first string
// 11101 : second string
// 00101 : third string
// -----
// 21303 : intermediate result
// =====
// 10101 : returned value

std::vector<int> getBitMajority(const std::vector<std::string> &input) {
  std::vector<int> majority(stringLength);
  for (const auto &line : input) {
    for (size_t index = 0; auto c : line) {
      if (c == '1') {
        ++majority[index];
      }
      ++index;
    }
  }
  std::for_each(majority.begin(), majority.end(),
                [size = input.size()](int &i) {
                  i = (i >= static_cast<int>(size - i)) ? 1 : 0;
                });
  return majority;
}

// Reduces the input list to a single string
// according to the oxygen/CO2 rules. For oxygen
// strings with the majority bits are kept and
// for CO2 strings with the minority bits are kept
int calculateRating(std::vector<std::string> &input, bool isOxygenRating) {
  auto occurences = getBitMajority(input);
  auto binaryNumbers = input;
  std::vector<std::string> toVector;
  int index = 0;
  while (binaryNumbers.size() > 1) {
    occurences = getBitMajority(binaryNumbers);
    bool keepOn = occurences[index];
    keepOn = isOxygenRating ? keepOn : !keepOn;
    std::copy_if(binaryNumbers.begin(), binaryNumbers.end(),
                 std::back_inserter(toVector),
                 [keepOn, index](const std::string &s) {
                   if (keepOn) {
                     return (s[index] == '1');
                   } else {
                     return (s[index] == '0');
                   }
                 });
    ++index;
    binaryNumbers = std::move(toVector);
  }
  std::bitset<stringLength> rating(binaryNumbers[0]);
  return rating.to_ulong();
}

int calculatePower(const std::vector<std::string> &input) {
  const auto gamma = getBitMajority(input);
  std::string gammaString;
  for (auto n : gamma) {
    gammaString += std::to_string(n);
  }

  std::bitset<stringLength> gammaBits(gammaString);
  std::bitset<stringLength> epsilonBits = gammaBits;
  epsilonBits.flip();

  return gammaBits.to_ulong() * epsilonBits.to_ulong();
}

int main(int /*argc*/, char *argv[]) {
  std::ifstream fileName(argv[1]);
  std::vector<std::string> input;
  if (!fileName.is_open()) {
    std::cout << "File not found" << std::endl;
    return 1;
  }
  std::string line;
  while (getline(fileName, line)) {
    input.emplace_back(std::move(line));
  }
  std::cout << calculatePower(input) << std::endl;
  std::cout << calculateRating(input, true) * calculateRating(input, false)
            << std::endl;
}
