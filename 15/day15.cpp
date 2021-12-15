#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <array>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

struct Location
{
  Location() = default;
  Location(int x, int y, int risk)
      : x(x)
      , y(y)
      , risk(risk)
  {
  }
  int x;
  int y;
  int risk;
  bool operator==(const Location& other) const
  {
    return x == other.x && y == other.y;
  }
};

struct LocationPriority
{
  LocationPriority(Location* location, int priority)
      : location(location)
      , priority(priority)
  {
  }
  Location* location;
  int priority;
};

auto compare = [](LocationPriority lp1, LocationPriority lp2) -> bool
{ return lp1.priority > lp2.priority; };

static constexpr auto neighborIndices =
    std::array<std::tuple<int, int>, 4>{{{0, -1}, {0, 1}, {-1, 0}, {1, 0}}};
struct Grid
{
  Grid(std::vector<Location>&& grid, int o_w, int o_h, int factor)
      : grid{grid}
      , o_w{o_w}
      , o_h{o_h}
      , factor{factor}
  {
  }

  bool isValid(int x, int y) const
  {
    return x >= 0 && x < o_w * factor && y >= 0 && y < o_h * factor;
  }

  Location& get(int x, int y)
  {
    if (x >= o_w || y >= o_h)  // virtual grid
    {
      int risk = grid[(y % o_h) * o_w * factor + x % o_w]
                     .risk;  // Risk of the base grid
      risk += x / o_w;
      risk += y / o_h;
      risk = (risk - 1) / 9 >= 1 ? risk % 9 : risk;  // normalize risk
      Location& loc = grid[y * o_w * factor + x];
      loc.x = x;
      loc.y = y;
      loc.risk = risk;
    }
    return grid[y * o_w * factor + x];
  }

  std::vector<Location*> getNeighbors(Location* location)
  {
    std::vector<Location*> neighbors;
    for (const auto& t : neighborIndices)
    {
      if (isValid(location->x + std::get<0>(t), location->y + std::get<1>(t)))
      {
        neighbors.push_back(
            &get(location->x + std::get<0>(t), location->y + std::get<1>(t)));
      }
    }
    return neighbors;
  }

  std::vector<Location> grid;
  int o_w;
  int o_h;
  int factor;
};

std::unordered_map<Location*, Location*> aStar(
    Grid& g, Location& start, Location& end)
{
  std::unordered_map<Location*, Location*> cameFrom;
  std::unordered_map<Location*, int> costSoFar;
  std::priority_queue<
      LocationPriority,
      std::vector<LocationPriority>,
      decltype(compare)>
      frontier(compare);
  frontier.push({&start, 0});
  cameFrom[&start] = &start;
  costSoFar[&start] = 0;

  while (!frontier.empty())
  {
    auto current = frontier.top();
    frontier.pop();
    if (current.location == &end)
    {
      break;
    }
    for (auto* neighbor : g.getNeighbors(current.location))
    {
      auto newCost = costSoFar[current.location] + neighbor->risk;
      if (costSoFar.find(neighbor) == costSoFar.end() ||
          newCost < costSoFar[neighbor])
      {
        costSoFar[neighbor] = newCost;
        auto priority = newCost + neighbor->risk;
        frontier.emplace(neighbor, priority);
        cameFrom[neighbor] = current.location;
      }
    }
  }

  return cameFrom;
}

std::vector<Location*> reconstructPath(
    Location& start,
    Location& goal,
    std::unordered_map<Location*, Location*> cameFrom)
{
  std::vector<Location*> path;
  Location* current = &goal;
  while (*current != start)
  {
    path.push_back(current);
    current = cameFrom[current];
  }
  std::reverse(path.begin(), path.end());
  return path;
}

int main(int /*argc*/, char* argv[])
{
  std::stringstream ss;
  {
    std::ifstream file(argv[1]);
    if (!file.is_open())
    {
      throw std::runtime_error("Could not open file");
    }
    ss << file.rdbuf();
  }

  std::vector<std::string> lines;
  boost::split(lines, ss.str(), boost::is_any_of("\n"));
  int width = lines[0].size();
  int height = lines.size();
  static constexpr std::array<int, 2> factors{1, 5};
  for (const auto factor : factors)
  {
    std::vector<Location> gridVector(width * height * factor * factor);
    for (int j = 0; const auto& line : lines)
    {
      for (int i = 0; const auto& c : line)
      {
        gridVector[j * width * factor + i] = Location{i, j, c - '0'};
        ++i;
      }
      ++j;
    }
    Grid g{std::move(gridVector), width, height, factor};

    const auto cameFrom =
        aStar(g, g.get(0, 0), g.get(width * factor - 1, height * factor - 1));
    const auto path = reconstructPath(
        g.get(0, 0), g.get(width * factor - 1, height * factor - 1), cameFrom);

    int cost = 0;
    std::for_each(
        path.begin(), path.end(), [&](Location* l) { cost += l->risk; });
    std::cout << cost << std::endl;
  }
  return 0;
}
