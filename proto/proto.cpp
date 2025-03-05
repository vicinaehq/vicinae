#include "proto.hpp"
#include <cstdint>
#include <iostream>

int main() {
  ArgumentArray arr;
  ArgumentArray arr2;

  arr2 << "fuck it" << "yolo";

  arr << 42 << "Hello world" << arr2 << Argument::ArgumentDict{{"json", 1}, {"ling", "pling"}};

  auto packet = arr.build();

  ArgumentArray parsed(packet);

  uint32_t n;
  std::string s;
  ArgumentArray subParsed;
  Argument::ArgumentDict dict;

  parsed >> n >> s >> subParsed >> dict;

  std::cout << n << s << std::endl;

  subParsed >> s;
  std::cout << s << std::endl;
  subParsed >> s;
  std::cout << s << std::endl;

  std::cout << "json" << dict["json"].number();
  std::cout << "ling" << " " << dict["ling"].string();
}
