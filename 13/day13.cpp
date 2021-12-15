#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

struct Point {
  int x;
  int y;
  Point(int x, int y) : x(x), y(y) {}
  bool operator==(const Point &p) const { return p.x == x && p.y == y; }
  bool operator<(const Point &p) const {
    if (x != p.x)
      return x < p.x;
    return y < p.y;
  }
};

namespace std {
template <> struct hash<Point> {
  size_t operator()(const Point &p) const { return (53 + p.x) * 53 + p.y; }
};
} // namespace std
struct FoldInstruction {
  bool vertical = false;
  int foldLine = 0;
};

struct Grid {
  void addPoint(Point &&p, int value) { grid[p] = value; }
  void updateMinMax() {
    for (auto &p : grid) {
      if (p.second == 0) {
        continue;
      }
      if (p.first.x + 1 > width) {
        width = p.first.x + 1;
      }
      if (p.first.y + 1 > height) {
        height = p.first.y + 1;
      }
    }
  }

  void print() {
    std::vector<int> vectorGrid;
    updateMinMax();
    vectorGrid.resize(width * height);
    for (const auto &p : grid) {
      vectorGrid[p.first.x + p.first.y * width] = p.second;
    }
    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        if (vectorGrid[x + y * width] == 0) {
          std::cout << " ";
        } else {
          std::cout << vectorGrid[x + y * width];
        }
      }
      std::cout << std::endl;
    }
  }

  void fold(const FoldInstruction &instruction) {
    if (instruction.vertical) {
      auto it = grid.begin();
      while (it != grid.end()) {
        if (it->first.x > instruction.foldLine) {
          auto point = it->first;
          it = grid.erase(it);
          int oldX = point.x;
          point.x = oldX - 2 * (oldX - instruction.foldLine);
          grid[point] = 1;
        } else {
          ++it;
        }
      }
    } else {
      auto it = grid.begin();
      while (it != grid.end()) {
        if (it->first.y > instruction.foldLine) {
          auto point = it->first;
          it = grid.erase(it);
          int oldY = point.y;
          point.y = oldY - 2 * (oldY - instruction.foldLine);
          grid[point] = 1;
        } else {
          ++it;
        }
      }
    }
  }
  std::map<Point, int> grid;
  int width{};
  int height{};
};

int main(int /*argc*/, char *argv[]) {
  std::stringstream ss;
  {
    std::fstream file(argv[1]);
    if (!file.is_open()) {
      throw std::runtime_error("Could not open file");
    }
    ss << file.rdbuf();
  }
  std::vector<std::string> lines;
  boost::algorithm::split(lines, ss.str(), boost::is_any_of("\n"));
  size_t index = 0;
  Grid grid;
  for (const auto &line : lines) {
    if (line.empty()) {
      break;
    }
    ++index;
    std::vector<std::string> tokens;
    boost::algorithm::split(tokens, line, boost::is_any_of(","));
    grid.addPoint(Point(std::stoi(tokens[0]), std::stoi(tokens[1])), 1);
  }

  std::vector<FoldInstruction> instructions;
  for (size_t i = index + 1; i < lines.size(); ++i) {
    std::vector<std::string> tokens;
    boost::algorithm::split(tokens, lines[i], boost::is_any_of(" "));
    FoldInstruction instruction;
    boost::algorithm::split(tokens, tokens[2], boost::is_any_of("="));
    if (tokens[0] == "x") {
      instruction.vertical = true;
    } else {
      instruction.vertical = false;
    }
    instruction.foldLine = std::stoi(tokens[1]);
    instructions.emplace_back(instruction);
  }
  for (const auto &instruction : instructions) {
    grid.fold(instruction);
  }
  grid.print();
  return 0;
}