#include "proto.hpp"
#include <iostream>

int main() {
  Proto::Marshaler marshaler;
  Proto::Array arr;

  arr.push_back(42);
  arr.push_back("Hello world");
  arr.push_back(Proto::Dict{{"type", "fire"}, {"json", true}});

  auto packet = marshaler.marshal(arr);
  auto result = marshaler.unmarshal<Proto::Array>(packet);

  if (auto err = std::get_if<Proto::Marshaler::Error>(&result)) {
    std::cout << "unmarshal failed" << err->message << std::endl;
    return 1;
  }

  auto newArr = std::get<Proto::Array>(result);

  if (newArr.size() == 3) {
    auto n = newArr.at(0).asInt();
    auto s = newArr.at(1).asString();
    auto opts = newArr.at(2).asDict();

    std::cout << n << s << std::endl;
    std::cout << "type=" << opts["type"].asString() << std::endl;
    std::cout << "json=" << opts["json"].asBool() << std::endl;
  }
}
