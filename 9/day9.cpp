#include <algorithm>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

struct GridSize {
  int width;
  int height;
};

struct Grid {
  Grid(std::vector<int> g, GridSize size) : grid{g}, size{size} {
    tags.resize(size.width * size.height);
  }
  void setVisited(int x, int y) noexcept { tags[y * size.width + x] = true; }

  bool visited(int x, int y) const { return tags[y * size.width + x]; }

  int get(int x, int y) const {
    if (x < 0 || y < 0 || x >= size.width || y >= size.height) {
      return 9;
    }
    return grid[y * size.width + x];
  }
  std::vector<int> grid;
  std::vector<bool> tags;
  GridSize size;
};

bool isMinimum(const Grid &v, int x, int y) {
  return v.get(x, y) < v.get(x - 1, y) && v.get(x, y) < v.get(x + 1, y) &&
         v.get(x, y) < v.get(x, y - 1) && v.get(x, y) < v.get(x, y + 1);
}

int exploreBasin(Grid &v, int x, int y) {
  v.setVisited(x, y);
  int basinSize{1};
  if (v.get(x - 1, y) < 9 && v.get(x - 1, y) > v.get(x, y) &&
      !v.visited(x - 1, y)) {
    basinSize += exploreBasin(v, x - 1, y);
  }
  if (v.get(x, y - 1) < 9 && v.get(x, y - 1) > v.get(x, y) &&
      !v.visited(x, y - 1)) {
    basinSize += exploreBasin(v, x, y - 1);
  }
  if (v.get(x + 1, y) < 9 && v.get(x + 1, y) > v.get(x, y) &&
      !v.visited(x + 1, y)) {
    basinSize += exploreBasin(v, x + 1, y);
  }
  if (v.get(x, y + 1) < 9 && v.get(x, y + 1) > v.get(x, y) &&
      !v.visited(x, y + 1)) {
    basinSize += exploreBasin(v, x, y + 1);
  }
  return basinSize;
}

void part2(const std::vector<int> &input, const GridSize &gridSize) {
  Grid grid{input, gridSize};
  std::vector<int> basins;
  for (int y = 0; y < gridSize.height; ++y) {
    for (int x = 0; x < gridSize.width; ++x) {
      if (isMinimum(grid, x, y)) {
        int basinSize = exploreBasin(grid, x, y);
        basins.push_back(basinSize);
      }
    }
  }
  std::sort(basins.begin(), basins.end());
  assert(basins.size() >= 3);
  std::cout << basins[basins.size() - 3] * basins[basins.size() - 2] *
                   basins[basins.size() - 1]
            << std::endl;
}

void part1(const std::vector<int> &input, const GridSize &gridSize) {
  Grid grid{input, gridSize};
  int riskLevel{0};
  for (int y = 0; y < gridSize.height; ++y) {
    for (int x = 0; x < gridSize.width; ++x) {
      if (isMinimum(grid, x, y)) {
        riskLevel += grid.get(x, y) + 1;
      }
    }
  }
  std::cout << riskLevel << std::endl;
}

int main(int /*argc*/, char **argv) {
  std::stringstream ss;
  {
    std::ifstream file(argv[1]);
    if (!file.is_open()) {
      throw std::runtime_error("Could not open file");
    }
    ss << file.rdbuf();
    file.close();
  }
  std::vector<std::string> lines;
  std::vector<int> grid;
  boost::algorithm::split(lines, ss.str(), boost::is_any_of("\n"));
  GridSize size{};
  size.width = lines[0].size();
  size.height = lines.size();
  for (const auto &line : lines) {
    for (auto c : line) {
      grid.push_back(c - '0');
    }
  }
  part1(grid, size);
  part2(grid, size);
  return 0;
}
