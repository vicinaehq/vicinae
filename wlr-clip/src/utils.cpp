#include "utils.hpp"
#include <random>
#include <iomanip>

std::string generateFileId(int length) {
  std::random_device rd;
  std::mt19937 gen(rd());

  // Define the distribution for hexadecimal characters (0-15)
  std::uniform_int_distribution<> dis(0, 15);

  std::stringstream ss;

  // Generate the random hex string
  for (int i = 0; i < length; i++) {
    int randomValue = dis(gen);
    ss << std::hex << randomValue;
  }

  return ss.str();
}
