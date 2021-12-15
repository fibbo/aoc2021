#include <algorithm>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

struct Point {
  int x;
  int y;
  bool operator==(const Point &p) const { return p.x == x && p.y == y; }
};

struct Line {
  Point start;
  Point end;
  // So at least the x coordinate is ordered
  void normalize() {
    if (start.x > end.x) {
      std::swap(start.x, end.x);
      std::swap(start.y, end.y);
    }
  }
};

namespace std {
template <> struct hash<Point> {
  size_t operator()(const Point &p) const { return (53 + p.x) * 53 + p.y; }
};
} // namespace std

enum class Direction { horizontal, vertical, diagonal };

// @returns 1 if the line is horizontal, -1 if vertical and 0 if neither
Direction direction(const Point &p1, const Point &p2) {
  if (p1.x == p2.x) {
    return Direction::vertical;
  } else if (p1.y == p2.y) {
    return Direction::horizontal;
  } else {
    return Direction::diagonal;
  }
}
void getPoint(const std::string &stringPoint, Point &outPoint) {
  static std::vector<std::string> coordinates;
  boost::algorithm::split(coordinates, stringPoint, boost::is_any_of(","));
  outPoint.x = std::stoi(coordinates[0]);
  outPoint.y = std::stoi(coordinates[1]);
}

using Coordinates = Point;
std::unordered_map<Coordinates, int> fieldCount;

void fillArray(const std::vector<Line> &segments) {
  for (const auto &line : segments) {
    auto dir = direction(line.start, line.end);
    switch (dir) {
    case Direction::horizontal: // y's are the same
      for (int x = line.start.x; x <= line.end.x; ++x) {
        fieldCount[{x, line.start.y}]++;
      }
      break;
    case Direction::vertical: // x's are the same
      if (line.start.y > line.end.y) {
        for (int y = line.end.y; y <= line.start.y; ++y) {
          // p = {line.start.x, y};
          fieldCount[{line.start.x, y}]++;
        }
      } else {
        for (int y = line.start.y; y <= line.end.y; ++y) {
          fieldCount[{line.start.x, y}]++;
        }
      }
      break;
    case Direction::diagonal:
      if (line.start.y < line.end.y) {
        for (int x = line.start.x, y = line.start.y; x <= line.end.x;
             ++x, ++y) {
          fieldCount[{x, y}]++;
        }
      } else {
        for (int x = line.start.x, y = line.start.y; x <= line.end.x;
             ++x, --y) {
          fieldCount[{x, y}]++;
        }
      }
      break;
    }
  }
}

int main(int /*argc*/, char *argv[]) {
  std::ifstream file(argv[1]);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open file");
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::vector<std::string> lines;
  boost::algorithm::split(lines, buffer.str(), boost::is_any_of("\n"));
  std::for_each(lines.begin(), lines.end(),
                [](auto &line) { boost::algorithm::trim_all(line); });
  std::vector<Line> lineSegments;
  std::for_each(lines.begin(), lines.end(), [&](const std::string &line) {
    std::vector<std::string> points;
    boost::algorithm::split(points, line, boost::is_any_of(" -> "),
                            boost::token_compress_on);
    Point start{}, end{};
    getPoint(points[0], start);
    getPoint(points[1], end);
    Line segment{start, end};
    segment.normalize();
    lineSegments.emplace_back(std::move(segment));
  });
  fillArray(lineSegments);
  size_t count = 0;
  for (const auto &pair : fieldCount) {
    if (pair.second >= 2) {
      count++;
    }
  }
  std::cout << count << std::endl;
  return 0;
}