#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

void part1(const std::string &fileName) {

  std::ifstream file(fileName);
  if (file.fail()) {
    throw std::runtime_error("File not found");
  }
  int old_depth, depth;
  file >> old_depth;
  int count = 0;
  while (file >> depth) {
    if (depth > old_depth)
      ++count;
    old_depth = depth;
  }

  std::cout << "Part 1:" << count << std::endl;
}

struct SlidingWindow {

  SlidingWindow() = default;

  SlidingWindow(std::ifstream &file) {
    file >> tail;
    file >> middle;
    file >> head;
    sum = head + middle + tail;
  }

  SlidingWindow(const SlidingWindow &old, std::ifstream &file) {
    tail = old.middle;
    middle = old.head;
    file >> head;
    sum = head + middle + tail;
  }

  int head;
  int middle;
  int tail;
  int sum;
};

std::istream &operator>>(std::ifstream &file, SlidingWindow &sw) {
  sw = SlidingWindow(sw, file);
  return file;
}

void part2(const std::string fileName) {
  std::ifstream file(fileName);
  SlidingWindow oldWindow(file);
  SlidingWindow window = oldWindow;
  int count = 0;
  while (file.peek() != EOF) {
    file >> window;
    if (window.sum > oldWindow.sum) {
      ++count;
    }
    oldWindow = window;
  }
  std::cout << "Part 2: " << count << std::endl;
}

int main(int /*argc*/, char *argv[]) {
  std::cout << argv[1] << std::endl;
  part1(argv[1]);
  part2(argv[1]);
}