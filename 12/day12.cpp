#include <algorithm>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

bool isLower(const std::string &s) {
  return std::all_of(s.begin(), s.end(),
                     [](char c) { return std::islower(c); });
}

struct Cave {
  void addNeighbor(Cave *neighbor) { adjacent.push_back(neighbor); }
  std::string name;
  bool isSmall;
  std::vector<Cave *> adjacent;
};

Cave makeCave(const std::string &name) {
  Cave cave;
  cave.name = name;
  cave.isSmall = isLower(name);
  return cave;
}

struct Path {
  void addCave(Cave *cave) {
    if (cave->isSmall) {
      if (containsCave(cave)) {
        visitedSameSmallCaveTwice = true;
        favoriteSmallCavern = cave->name;
      }
    }
    path.push_back(cave);
  }

  void clear() {
    path.clear();
    visitedSameSmallCaveTwice = false;
    favoriteSmallCavern = "";
  }

  bool containsCave(const Cave *cave) const {
    return std::find(path.begin(), path.end(), cave) != path.end();
  }

  void dropLastPath() {
    if (path.back()->name == favoriteSmallCavern) {
      visitedSameSmallCaveTwice = false;
      favoriteSmallCavern = "";
    }
    path.pop_back();
  }
  std::vector<Cave *> path;
  bool visitedSameSmallCaveTwice = false;
  std::string favoriteSmallCavern;
};

bool startOrEnd(const Cave *cave) noexcept {
  return cave->name == "start" || cave->name == "end";
}

inline bool disallowPathPart1(const Cave *currentCave, const Path &path) {
  return currentCave->isSmall && path.containsCave(currentCave);
}

inline bool disallowPathPart2(const Cave *currentCave, const Path &path) {
  return disallowPathPart1(currentCave, path) &&
         (startOrEnd(currentCave) || path.visitedSameSmallCaveTwice);
}

template <typename PathChecker>
int findPaths(PathChecker &&pathChecker, Cave *currentCave, Cave *endCave,
              Path &path) {
  int nPaths = 0;
  if (pathChecker(currentCave, path)) {
    return 0;
  }

  if (currentCave == endCave) {
    return 1;
  }

  path.addCave(currentCave);
  for (auto *cave : currentCave->adjacent) {
    nPaths += findPaths(pathChecker, cave, endCave, path);
  }
  path.dropLastPath();
  return nPaths;
}

int main(int /*argc*/, char *argv[]) {
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
  std::map<std::string, Cave> caveMap;
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

  Path path;
  std::cout << findPaths(disallowPathPart1, start, end, path) << std::endl;
  std::cout << findPaths(disallowPathPart2, start, end, path) << std::endl;
  return 0;
}
