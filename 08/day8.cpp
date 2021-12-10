#include <algorithm>
#include <array>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

std::string findJointChars(std::string str1, std::string str2) {
  if (str1.size() > str2.size()) {
    std::swap(str1, str2);
  }
  std::string commonCharacters;
  std::copy_if(
      str1.begin(), str1.end(), std::back_inserter(commonCharacters),
      [&str2](const char &c) { return str2.find(c) != std::string::npos; });
  return commonCharacters;
}

std::string findDisjointChars(std::string str1, std::string str2) {
  if (str1.size() > str2.size()) {
    std::swap(str1, str2);
  }
  std::string missingCharacters;
  std::copy_if(
      str1.begin(), str1.end(), std::back_inserter(missingCharacters),
      [&str2](const char &c) { return str2.find(c) == std::string::npos; });
  missingCharacters += str2.substr(str1.size());
  return missingCharacters;
}

// Map encoded digits by length
std::map<int, std::vector<std::string>>
getCharsByLength(const std::vector<std::string> &strings) {
  std::map<int, std::vector<std::string>> charsByLength;
  for (const auto &str : strings) {
    charsByLength[str.size()].push_back(str);
  }
  return charsByLength;
}

uint64_t decipherLine(const std::vector<std::string> &allDigits,
                      const std::vector<std::string> &output) {
  auto charsByLength = getCharsByLength(allDigits);
  // Easy access for known digits
  std::array<std::string, 10> stringsToNumbers;
  // 1, 4, 7 and 8 are have a unique number of segments
  // and are known immediately
  stringsToNumbers[1] = charsByLength[2][0];
  stringsToNumbers[4] = charsByLength[4][0];
  stringsToNumbers[7] = charsByLength[3][0];
  stringsToNumbers[8] = charsByLength[7][0];
  const auto topChar =
      findDisjointChars(stringsToNumbers[1], stringsToNumbers[7])[0];

  // Infer 6, 9 and 0
  const auto brokenNine = stringsToNumbers[4] + topChar;
  for (const auto &number : charsByLength[6]) {
    if (auto dif = findDisjointChars(number, brokenNine); dif.size() == 1) {
      stringsToNumbers[9] = number;
    } else if (auto dif = findJointChars(stringsToNumbers[1], number);
               dif.size() == 2) {
      stringsToNumbers[0] = number; // This is the 0 digit
    } else {
      stringsToNumbers[6] = number;
    }
  }

  // Infer 5, 3 and 2
  for (const auto &number : charsByLength[5]) {
    const auto dif = findDisjointChars(number, stringsToNumbers[6]);
    if (dif.size() == 1) {
      // should be number 5
      stringsToNumbers[5] = number;
    } else {
      // should be number 2 or 3
      const auto dif = findJointChars(stringsToNumbers[1], number);
      if (dif.size() == 2) {
        stringsToNumbers[3] = number;
      } else {
        stringsToNumbers[2] = number;
      }
    }
  }

  uint64_t result{};
  size_t power = output.size() - 1;
  for (const auto &digit : output) {
    for (int index = 0; auto &number : stringsToNumbers) {
      if (findDisjointChars(digit, number).size() == 0) {
        result += index * std::pow(10, power--);
        break;
      }
      index++;
    }
  }
  return result;
}

void part2(const std::vector<std::string> &lines) {
  std::vector<std::string> entry;
  int sum{0};
  for (const auto &line : lines) {
    std::vector<std::string> entry;
    boost::algorithm::split(entry, line, boost::is_any_of("|"));
    std::for_each(entry.begin(), entry.end(),
                  [](auto &s) { boost::algorithm::trim_all(s); });
    std::vector<std::string> input;
    boost::algorithm::split(input, entry[0], boost::is_any_of(" "));
    std::vector<std::string> output;
    boost::algorithm::split(output, entry[1], boost::is_any_of(" "));
    sum += decipherLine(input, output);
  }
  std::cout << sum << std::endl;
}

std::array<int, 4> knownIntegerLengths = {2, 3, 4, 7};
void part1(const std::vector<std::string> &lines) {
  std::vector<std::string> entry;
  uint32_t totalCount{0};
  for (auto &line : lines) {
    boost::algorithm::split(entry, line, boost::is_any_of("|"));
    std::vector<std::string> digits;
    boost::algorithm::split(digits, entry[1], boost::is_any_of(" "));
    for (const auto &n : digits) {
      if (std::find(knownIntegerLengths.begin(), knownIntegerLengths.end(),
                    n.size()) != knownIntegerLengths.end()) {
        ++totalCount;
      }
    }
  }
  std::cout << totalCount << std::endl;
}

int main(int /*argc*/, char **argv) {
  std::stringstream ss;
  {
    std::ifstream file{argv[1]};
    if (!file.is_open()) {
      throw std::runtime_error("Could not open file");
    }
    ss << file.rdbuf();
    file.close();
  }
  std::vector<std::string> lines;
  boost::split(lines, ss.str(), boost::is_any_of("\n"));

  part1(lines);
  part2(lines);
  return 0;
}