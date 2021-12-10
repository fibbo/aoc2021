#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

enum class Direction { forward, down, up };

static const std::map<std::string, Direction> directionMap = {
    {"forward", Direction::forward},
    {"up", Direction::up},
    {"down", Direction::down},
};

struct Command {
  Command() = default;
  Command(std::ifstream &file) {
    std::string line;
    std::getline(file, line);
    std::stringstream ss(line);
    std::string directionString;
    ss >> directionString;
    direction = directionMap.at(directionString);
    ss >> distance;
  }
  Direction direction;
  int distance;
};

struct Position {
  void calculate(const Command &command) {
    switch (command.direction) {
    case Direction::forward:
      horizontal += command.distance;
      depth += aim * command.distance;
      break;
    case Direction::up:
      aim -= command.distance;
      break;
    case Direction::down:
      aim += command.distance;
      break;
    }
  }
  int horizontal;
  int depth;
  int aim;
};

std::ifstream &operator>>(std::ifstream &file, Command &command) {
  command = Command(file);
  return file;
}

int main(int argc, char **argv) {
  std::ifstream file(argv[1]);
  Position position{};
  Command command;
  while (file.peek() != EOF) {
    file >> command;
    position.calculate(command);
  }
  std::cout << position.horizontal * position.depth << std::endl;
}