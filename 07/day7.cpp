#include <algorithm>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

int fuelSpentForDestination(const std::vector<int> &crabs, int destintation) {
  int fuelCost = 0;
  for (auto position : crabs) {
    int n = std::abs(position - destintation);
    fuelCost += n * (n + 1) / 2;
  }
  return fuelCost;
}

int bestFuelCost(const std::vector<int> &crabs, int median) {
  int lower = fuelSpentForDestination(crabs, median - 1);
  int middle = fuelSpentForDestination(crabs, median);
  int higher = fuelSpentForDestination(crabs, median + 1);
  if (lower < middle) {
    return bestFuelCost(crabs, --median);
  }
  if (higher < middle) {
    return bestFuelCost(crabs, ++median);
  }
  return middle;
}

int main(int /*argc*/, char **argv) {
  std::stringstream ss;
  {
    std::ifstream file{argv[1]};
    if (!file.is_open()) {
      throw std::runtime_error("Could not open file");
    }
    ss << file.rdbuf();
    file.close();
  }
  std::vector<std::string> stringNumbers;
  std::vector<int> numbers;
  boost::algorithm::split(stringNumbers, ss.str(),
                          boost::algorithm::is_any_of(","));
  std::for_each(
      stringNumbers.begin(), stringNumbers.end(),
      [&numbers](const std::string &s) { numbers.push_back(std::stoi(s)); });
  std::sort(numbers.begin(), numbers.end());
  int median = numbers[numbers.size() / 2];
  uint64_t fuelCost{0};
  for (const auto p : numbers) {
    fuelCost += std::abs(p - median);
  }
  std::cout << "Fuel cost p1: " << fuelCost << std::endl;
  std::cout << "Fuel cost p2: " << bestFuelCost(numbers, median) << std::endl;
  return 0;
}