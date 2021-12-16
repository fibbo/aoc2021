#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

std::unordered_map<char, std::string> hexToBinary = {
    {'0', "0000"},
    {'1', "0001"},
    {'2', "0010"},
    {'3', "0011"},
    {'4', "0100"},
    {'5', "0101"},
    {'6', "0110"},
    {'7', "0111"},
    {'8', "1000"},
    {'9', "1001"},
    {'A', "1010"},
    {'B', "1011"},
    {'C', "1100"},
    {'D', "1101"},
    {'E', "1110"},
    {'F', "1111"},
};

template <typename T>
uint64_t calculateDecimal(T binary)
{
  uint64_t value{0};
  size_t power = binary.size() - 1;
  for (auto c : binary)
  {
    value += static_cast<uint64_t>(c - '0') << power--;
  }
  assert(value >= 0);
  return value;
}

struct Literal
{
  Literal(std::string_view binary)
      : position{0}
      , value{0}
  {
    std::string literal;
    bool hasNext = true;
    while (hasNext)
    {
      hasNext = binary[position++] == '1';
      literal += binary.substr(position, 4);
      position += 4;
    }
    value = calculateDecimal(literal);
  }
  size_t position;
  uint64_t value;
};

struct Packet
{
  Packet(std::string_view binary)
      : position{0}
      , value{0}
  {
    version = calculateDecimal(binary.substr(position, 3));
    position += 3;
    type = calculateDecimal(binary.substr(position, 3));
    position += 3;
    if (type == 4)
    {
      Literal literal{binary.substr(position)};
      value = literal.value;
      position += literal.position;
    }
    else
    {
      auto opType = binary.substr(position, 1);
      ++position;
      if (opType == "0")
      {
        // Read 15 bits
        auto literal = binary.substr(position, 15);
        position += 15;
        // Convert to number
        uint64_t nBits = calculateDecimal(literal);
        // number = total length in bits of the packages
        uint64_t endBit = position + nBits;
        while (position < endBit)
        {
          Packet p{binary.substr(position)};
          position += p.position;
          subPackages.emplace_back(std::move(p));
        }
      }
      else
      {
        // Read 11 bits
        auto literal = binary.substr(position, 11);
        position += 11;
        // Convert to number
        int nPackages = calculateDecimal(literal);
        int count = 0;
        while (count < nPackages)
        {
          Packet p{binary.substr(position)};
          position += p.position;
          subPackages.emplace_back(std::move(p));
          count++;
        }
      }
    }
  }
  int getVersion() const
  {
    int versionSum = 0;
    for (const auto& p : subPackages)
    {
      versionSum += p.getVersion();
    }
    return version + versionSum;
  }

  uint64_t getValue() const
  {
    switch (type)
    {
    case 0:  // sum
    {
      uint64_t valueSum = 0;
      std::for_each(
          subPackages.begin(),
          subPackages.end(),
          [&valueSum](const Packet& p) { valueSum += p.getValue(); });
      return valueSum;
    }
    case 1:  // product
    {
      uint64_t valueProduct = 1;
      std::for_each(
          subPackages.begin(),
          subPackages.end(),
          [&valueProduct](const Packet& p) { valueProduct *= p.getValue(); });
      return valueProduct;
    }
    case 2:  // min
    {
      uint64_t valueMin = std::numeric_limits<uint64_t>::max();
      std::for_each(
          subPackages.begin(),
          subPackages.end(),
          [&valueMin](const Packet& p)
          { valueMin = std::min(valueMin, p.getValue()); });
      return valueMin;
    }
    case 3:  // max
    {
      uint64_t valueMax = std::numeric_limits<uint64_t>::min();
      std::for_each(
          subPackages.begin(),
          subPackages.end(),
          [&valueMax](const Packet& p)
          { valueMax = std::max(valueMax, p.getValue()); });
      return valueMax;
    }
    case 4:  // literal
    {
      return value;
    }
    case 5:  // greater-than
    {
      return subPackages[0].getValue() > subPackages[1].getValue() ? 1 : 0;
    }
    case 6:  // less-than
    {
      return subPackages[0].getValue() < subPackages[1].getValue() ? 1 : 0;
    }
    case 7:  // equal
    {
      return subPackages[0].getValue() == subPackages[1].getValue() ? 1 : 0;
    }
    default:
      throw std::runtime_error("Unknown operator");
    }
  }

  int type;
  int version;
  size_t position;
  uint64_t value;
  std::vector<Packet> subPackages;
};

int main(int /*argc*/, char* argv[])
{
  std::string arg{argv[1]};
  std::stringstream ss;
  if (arg.find(".txt") == std::string::npos)
  {
    ss << arg;
  }
  else
  {
    {
      std::ifstream file(argv[1]);
      if (!file.is_open())
        throw std::runtime_error("Could not open file");
      ss << file.rdbuf();
    }
  }
  std::string binaryMessage;
  for (const auto c : ss.str())
  {
    binaryMessage += hexToBinary[c];
  }
  Packet p{binaryMessage};
  int version = p.getVersion();
  uint64_t value = p.getValue();
  std::cout << version << std::endl;
  std::cout << value << std::endl;
  return 0;
}
