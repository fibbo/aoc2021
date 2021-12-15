#include <algorithm>
#include <array>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

std::array<char, 4> openingBrackets = {'{', '[', '(', '<'};
std::map<char, int> scoreMapAutoComplete = {
    {'(', 1}, {'<', 4}, {'[', 2}, {'{', 3}};

void part2(const std::vector<std::string> &input) {
  std::vector<uint64_t> scores;
  for (const auto &line : input) {
    std::stack<char> lastOpen;
    for (const auto &c : line) {
      if (std::find(openingBrackets.begin(), openingBrackets.end(), c) !=
          openingBrackets.end()) {
        lastOpen.push(c);
      } else {
        // non corrupt sequences, this is valid
        lastOpen.pop();
      }
    }
    // lastTop contains now the open brackets which have to be closed
    // to make the sequence valid
    uint64_t score = 0;
    while (!lastOpen.empty()) {
      score = score * 5 + scoreMapAutoComplete[lastOpen.top()];
      lastOpen.pop();
    }
    scores.push_back(score);
  }
  std::sort(scores.begin(), scores.end());
  std::cout << "Part 2: " << scores[scores.size() / 2] << std::endl;
}

std::map<char, int> scoreMap = {{')', 3}, {'>', 25137}, {']', 57}, {'}', 1197}};
std::map<char, int> matchingBrace = {
    {')', '('}, {'>', '<'}, {']', '['}, {'}', '{'}};

void part1And2(const std::vector<std::string> &input) {
  int score = 0;
  std::vector<std::string> incomplete;
  for (const auto &line : input) {
    std::stack<char> lastOpen;
    bool corruptLine = false;
    for (const auto &c : line) {
      if (c == '{' || c == '[' || c == '(' || c == '<') {
        lastOpen.push(c);
      } else {
        if (lastOpen.top() != matchingBrace.at(c)) {
          score += scoreMap[c];
          corruptLine = true;
          break;
        }
        lastOpen.pop();
      }
    }
    if (!corruptLine) {
      incomplete.push_back(line);
    }
  }
  std::cout << "Part 1:" << score << std::endl;
  part2(incomplete);
}

int main(int /*argc*/, char *argv[]) {
  std::stringstream ss;
  {
    std::ifstream file(argv[1]);
    if (!file) {
      throw std::runtime_error("cannot open file");
    }
    ss << file.rdbuf();
  }
  std::vector<std::string> lines;
  boost::algorithm::split(lines, ss.str(), boost::algorithm::is_any_of("\n"));
  part1And2(lines);
  return 0;
}