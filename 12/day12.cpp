#include <algorithm>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

struct Cave {
  void addNeighbor(Cave *neighbor) { adjacent.push_back(neighbor); }
  std::string name;
  bool isSmall;
  std::vector<Cave *> adjacent;
};

std::map<std::string, Cave> caveMap;

bool isLower(const std::string &s) {
  return std::all_of(s.begin(), s.end(),
                     [](char c) { return std::islower(c); });
}

Cave makeCave(const std::string &name) {
  Cave cave;
  cave.name = name;
  cave.isSmall = isLower(name);
  return cave;
}

struct Path {
  Path(Cave *cave) { path.push_back(cave); }
  Path() {}
  void addCave(Cave *cave) {
    if (cave->isSmall) {
      if (containsCave(cave)) {
        visitedSameSmallCaveTwice = true;
      }
    }
    path.push_back(cave);
  }
  bool containsCave(Cave *cave) const noexcept {
    return std::find(path.begin(), path.end(), cave) != path.end();
  }

  void printPath() const {
    for (auto cave : path) {
      std::cout << cave->name << " ";
    }
    std::cout << std::endl;
  }
  std::vector<Cave *> path;
  bool visitedSameSmallCaveTwice = false;
};

bool startOrEnd(Cave *cave) {
  return cave->name == "start" || cave->name == "end";
}
bool disallowPathPart1(Cave *currentCave, const Path &path) {
  if (currentCave->isSmall && path.containsCave(currentCave)) {
    return true;
  }
  return false;
}

bool disallowPathPart2(Cave *currentCave, const Path &path) {
  if (disallowPathPart1(currentCave, path) &&
      (startOrEnd(currentCave) || path.visitedSameSmallCaveTwice)) {
    return true;
  }
  return false;
}

template <typename PartChecker>
void findPath(PartChecker &&fn, Cave *currentCave, Cave *endCave, Path path,
              int &pathCount) {
  if (fn(currentCave, path)) {
    return;
  }
  Path prePath = path;
  path.addCave(currentCave);
  if (currentCave == endCave) {
    // path.printPath();
    pathCount++;
    return;
  }
  for (auto &cave : currentCave->adjacent) {
    findPath(fn, cave, endCave, path, pathCount);
  }
  return;
}

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
  boost::split(lines, ss.str(), boost::is_any_of("\n"));
  Cave *start = nullptr;
  Cave *end = nullptr;
  for (const auto &line : lines) {
    if (line.empty()) {
      continue;
    }
    std::vector<std::string> tokens;
    boost::split(tokens, line, boost::is_any_of("-"));
    for (const auto &token : tokens) {
      caveMap.insert(std::make_pair(token, makeCave(token)));
      if (token == "start") {
        start = &caveMap[token];
      }
      if (token == "end") {
        end = &caveMap[token];
      }
    }
    auto &caveA = caveMap[tokens[0]];
    auto &caveB = caveMap[tokens[1]];
    caveA.addNeighbor(&caveB);
    caveB.addNeighbor(&caveA);
  }
  int nPaths = 0;
  Path path;
  findPath(disallowPathPart1, start, end, path, nPaths);
  std::cout << nPaths << std::endl;
  findPath(disallowPathPart2, start, end, path, nPaths);
  std::cout << nPaths << std::endl;
  return 0;
}