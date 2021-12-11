#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

struct Position {
  int x;
  int y;
};

struct Octopus {
  explicit Octopus(Position pos, int energyLevel)
      : position{pos}, energy{energyLevel}, flashed{false} {}
  bool canFlash() const { return energy > 9 && !flashed; }
  bool justFlashed() const { return flashed; }
  void reset() {
    flashed = false;
    energy = 0;
  }
  Position position;
  int energy;
  bool flashed;
};

struct Grid {
  Grid(std::vector<Octopus> input, int width, int height)
      : grid{std::move(input)}, width{width}, height{height} {}

  std::vector<Octopus>::iterator begin() { return grid.begin(); }
  std::vector<Octopus>::iterator end() { return grid.end(); }

  void flashAtPosition(const Position &pos, int &flashCounter) {
    auto &flashingOctopus = grid[pos.y * width + pos.x];
    flashingOctopus.flashed = true;
    flashCounter++;

    // Increase neighbors
    for (int y = pos.y - 1; y <= pos.y + 1; y++) {
      for (int x = pos.x - 1; x <= pos.x + 1; x++) {
        if (x < 0 || y < 0 || x >= width || y >= height) {
          // discard values outside grid
          continue;
        }
        // Ignore self
        if (x == pos.x && y == pos.y) {
          continue;
        }
        // This is a neighbor of the flashing octopus
        auto &octopus = grid[y * width + x];
        ++octopus.energy;
        if (octopus.canFlash()) {
          flashAtPosition(octopus.position, flashCounter);
        }
      }
    }
  }
  std::vector<Octopus> grid;
  int width;
  int height;
};

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
  boost::algorithm::split(lines, ss.str(), boost::algorithm::is_any_of("\n"));
  int width = lines[0].size();
  int height = lines.size();

  std::vector<Octopus> octopi;
  for (int y = 0; const auto &line : lines) {
    for (int x = 0; const auto &c : line) {
      octopi.emplace_back(Octopus{Position{x, y}, c - '0'});
      ++x;
    }
    ++y;
  }
  Grid grid{std::move(octopi), width, height};

  int flashCount = 0;
  int oldFlashCount = 0;
  int iterations = 1;
  while (true) {
    for (auto &octopus : grid) {
      ++octopus.energy;
    }
    oldFlashCount = flashCount;
    for (auto &octopus : grid) {
      if (octopus.canFlash()) {
        grid.flashAtPosition(octopus.position, flashCount);
      }
    }
    if (flashCount - oldFlashCount == 100) {
      std::cout << "100 concurrent flashes in " << iterations << " iterations"
                << std::endl;
      break;
    }
    if (iterations == 100) {
      std::cout << "Flash count after 100 iterations: " << flashCount
                << std::endl;
    }

    for (auto &octopus : grid) {
      if (octopus.justFlashed()) {
        octopus.reset();
      }
    }
    iterations++;
  }
  return 0;
}
