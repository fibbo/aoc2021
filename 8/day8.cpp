#include <algorithm>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <climits>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <limits>
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

enum class Segment {
  top = 0x0000001,
  ul = 0x0000010,
  ur = 0x0000100,
  mid = 0x0001000,
  ll = 0x0010000,
  lr = 0x0100000,
  bot = 0x1000000,
};

void removeStringFromVector(std::vector<std::string> &vec, std::string str) {
  vec.erase(std::remove(vec.begin(), vec.end(), str), vec.end());
}

std::map<int, std::vector<std::string>>
getCharsByLength(std::vector<std::string> &strings) {
  std::map<int, std::vector<std::string>> charsByLength;
  for (const auto &str : strings) {
    charsByLength[str.size()].push_back(str);
  }
  return charsByLength;
}

int decipherLine(const std::string &line) {
  std::vector<std::string> entry;
  boost::algorithm::split(entry, line, boost::is_any_of("|"));
  std::for_each(entry.begin(), entry.end(),
                [](auto &s) { boost::algorithm::trim_all(s); });
  std::vector<std::string> allDigits;
  boost::algorithm::split(allDigits, entry[0], boost::is_any_of(" "),
                          boost::token_compress_on);
  auto charsByLength = getCharsByLength(allDigits);
  std::array<std::string, 10> stringsToNumbers;
  stringsToNumbers[1] = charsByLength[2][0];
  stringsToNumbers[4] = charsByLength[4][0];
  stringsToNumbers[7] = charsByLength[3][0];
  stringsToNumbers[8] = charsByLength[7][0];
  for (auto c : {2, 3, 4, 7}) {
    auto it = charsByLength.find(c);
    charsByLength.erase(it);
  }
  auto topChar = findDisjointChars(stringsToNumbers[1], stringsToNumbers[7])[0];

  // Find digit 9 and the bottom segment
  auto brokenNine = stringsToNumbers[4] + topChar;
  for (auto number : charsByLength[6]) {
    if (auto dif = findDisjointChars(number, brokenNine); dif.size() == 1) {
      stringsToNumbers[9] = number;
      removeStringFromVector(charsByLength[6], number);
      break;
    }
  }

  for (auto number : charsByLength[6]) { // 6, 0
    if (auto dif = findJointChars(stringsToNumbers[1], number);
        dif.size() == 2) {
      stringsToNumbers[0] = number; // This is the 0 digit
      removeStringFromVector(charsByLength[6], number);
      auto number_6 = charsByLength[6][0];
      stringsToNumbers[6] = number_6;
      charsByLength.erase(6);
    }
  }
  for (auto number : charsByLength[5]) {
    auto dif = findDisjointChars(number, stringsToNumbers[6]);
    if (dif.size() == 1) {
      // should be number 5
      stringsToNumbers[5] = number;
    } else {
      // should be number 2 or 3
      auto dif = findJointChars(stringsToNumbers[1], number);
      if (dif.size() == 2) {
        stringsToNumbers[3] = number;
      } else {
        stringsToNumbers[2] = number;
      }
    }
  }
  std::vector<std::string> digits;
  boost::algorithm::split(digits, entry[1], boost::is_any_of(" "),
                          boost::token_compress_on);
  std::for_each(stringsToNumbers.begin(), stringsToNumbers.end(),
                [&](auto &s) { std::sort(s.begin(), s.end()); });
  std::for_each(digits.begin(), digits.end(),
                [&](auto &s) { std::sort(s.begin(), s.end()); });
  std::string result;
  for (auto &digit : digits) {
    for (size_t index = 0; auto &number : stringsToNumbers) {
      if (findDisjointChars(digit, number).size() == 0) {
        result += std::to_string(index);
        break;
      }
      index++;
    }
  }
  return std::stoi(result);
}
std::vector<std::string> words;
void part2(const std::vector<std::string> &lines) {
  std::vector<std::string> entry;
  std::map<Segment, char> segmentToChar;
  int sum{0};
  for (const auto &line : lines) {
    sum += decipherLine(line);
  }
  std::cout << sum << std::endl;
}

std::map<int, int> lengthMap = {{7, 8}, {4, 4}, {2, 1}, {3, 7}};

void part1(const std::vector<std::string> &lines) {
  std::vector<int> frequencyCounter(9);
  std::vector<std::string> entry;
  for (auto &line : lines) {
    boost::algorithm::split(entry, line, boost::is_any_of("|"));
    std::vector<std::string> digits;
    boost::algorithm::split(digits, entry[1], boost::is_any_of(" "),
                            boost::token_compress_on);
    for (const auto &n : digits) {
      if (lengthMap.find(n.size()) != lengthMap.end()) {
        frequencyCounter[lengthMap[n.size()]]++;
      }
    }
  }
  uint32_t totalCount{0};
  for (const auto &n : frequencyCounter) {
    totalCount += n;
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
  // std::for_each(lines.begin(), lines.end(),
  // [](std::string &line) { boost::algorithm::trim_all(line); });
  part1(lines);
  part2(lines);
  return 0;
}