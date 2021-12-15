#include <algorithm>
#include <array>
#include <bitset>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

class BingoBoard {
public:
  BingoBoard(std::vector<int> &board) {
    for (size_t index = 0; const auto &number : board) {
      board_[index++] = number;
      remainingBoardScore_ += number;
    }
  }
  // Flip the corresponding bit in the bitboard
  // when a match has been found
  void tag(int n) {
    auto it = std::find(board_.begin(), board_.end(), n);
    if (it != board_.end()) {
      auto index = it - board_.begin();
      tagged_.flip(index);
      ++nTags_;
      remainingBoardScore_ -= n;
    }
  };

  // Returns true if the board has been solved the first time
  // Repeated calls to this method will return false
  bool solved() {
    if (nTags_ < 5 || alreadySolved_) {
      return false;
    }
    // check rows
    for (size_t index = 0; index < 25; index += 5) {
      if (tagged_[index] && tagged_[index + 1] && tagged_[index + 2] &&
          tagged_[index + 3] && tagged_[index + 4]) {
        alreadySolved_ = true;
        return true;
      }
    }
    // check columns
    for (size_t index = 0; index < 5; ++index) {
      if (tagged_[index] && tagged_[index + 5] && tagged_[index + 10] &&
          tagged_[index + 15] && tagged_[index + 20]) {
        alreadySolved_ = true;
        return true;
      }
    }
    return false;
  };

  int score() { return remainingBoardScore_; }

private:
  std::array<int, 25> board_{};
  std::bitset<25> tagged_{};
  uint32_t nTags_{0};
  int remainingBoardScore_{};
  bool alreadySolved_{false};
};

std::vector<std::string> readFile(const std::string &fileName) {
  std::ifstream file(fileName);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open file: " + fileName);
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::vector<std::string> lines;
  boost::algorithm::split(lines, buffer.str(), boost::is_any_of("\n"));
  std::erase_if(lines, [](const std::string &s) { return s.empty(); });
  std::for_each(lines.begin(), lines.end(),
                [](auto &line) { boost::algorithm::trim_all(line); });
  return lines;
}

std::vector<int> parseDrawnNumbers(const std::string &line) {
  std::vector<std::string> stringNumbers;
  boost::algorithm::split(stringNumbers, line, boost::is_any_of(","));
  std::vector<int> drawnNumbers;
  for (auto &s : stringNumbers) {
    drawnNumbers.push_back(std::stoi(s));
  }
  return drawnNumbers;
}

std::vector<BingoBoard> parseBoards(std::vector<std::string> &lines) {
  std::vector<BingoBoard> boards;
  // Start from 1 because the first line is the drawn numbers line
  for (size_t board = 1; board < lines.size(); board += 5) {
    std::vector<int> boardNumbers;
    for (size_t i = board; i < board + 5; i++) {
      std::vector<std::string> tmp;
      boost::algorithm::split(tmp, lines[i], boost::is_any_of(" "));
      for (auto n : tmp) {
        boardNumbers.push_back(std::stoi(n));
      }
    }
    boards.emplace_back(boardNumbers);
  }
  return boards;
}

int main(int /*argc*/, char *argv[]) {
  std::ifstream file(argv[1]);
  auto lines = readFile(argv[1]);
  auto drawnNumbers = parseDrawnNumbers(lines[0]);
  auto boards = parseBoards(lines);

  size_t solvedBoards = 0;
  for (auto drawnNumber : drawnNumbers) {
    for (auto &board : boards) {
      board.tag(drawnNumber);
      if (board.solved()) {
        solvedBoards++;
      }
      if (solvedBoards == boards.size()) {
        std::cout << "Draw number: " << drawnNumber << std::endl;
        std::cout << "Solved: " << drawnNumber * board.score() << std::endl;
        return 0;
      }
    }
  }

  return 0;
}