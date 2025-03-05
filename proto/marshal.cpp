#include "proto.hpp"
#include <cstdint>
#include <memory>

void Marshaler::marshal(std::vector<uint8_t> &packet, const Argument &arg) {
  if (auto s = arg.value<BufferType>()) {
    packet.push_back(ArgumentType::String);
    marshal(packet, s->size());
    packet.insert(packet.end(), s->begin(), s->end());
  } else if (auto n = arg.value<NumberType>()) {
    packet.push_back(ArgumentType::Number);
    marshal(packet, *n);
  } else if (auto array = arg.array()) {
    packet.push_back(ArgumentType::Array);
    marshal(packet, *array);
  } else if (auto dict = arg.value<std::unique_ptr<Argument::ArgumentDict>>()) {
    packet.push_back(ArgumentType::Dict);
    marshalDict(packet, *dict->get());
  }
}

void Marshaler::marshal(std::vector<uint8_t> &packet, const ArgumentArray &array) {
  packet.push_back(std::min(array.size(), (size_t)UINT8_MAX));

  for (const auto &arg : array) {
    marshal(packet, arg);
  }
}

void Marshaler::marshal(std::vector<uint8_t> &packet, uint32_t value) {
  int no = htonl(value);
  uint8_t *p = (uint8_t *)(&no);

  packet.push_back(p[0]);
  packet.push_back(p[1]);
  packet.push_back(p[2]);
  packet.push_back(p[3]);
}

void Marshaler::marshalString(std::vector<uint8_t> &packet, const std::vector<uint8_t> &s) {}

void Marshaler::marshalDict(std::vector<uint8_t> &packet, const Argument::ArgumentDict &dict) {
  packet.push_back(static_cast<uint8_t>(dict.size()));

  for (const auto &[key, value] : dict) {
    packet.push_back(ArgumentType::String);
    marshal(packet, key.size());
    packet.insert(packet.end(), key.begin(), key.end());
    marshal(packet, value);
  }
}

Argument::ArgumentDict Marshaler::unmarshalDict(const std::vector<uint8_t> &packet, size_t &idx) {
  std::map<std::string, Argument> dict;
  uint8_t argc = packet.at(idx++);
  Argument::ArgumentDict args;

  for (size_t i = 0; i < argc && idx < packet.size(); ++i) {
    auto key = unmarshalArgument(packet, idx);
    auto value = unmarshalArgument(packet, idx);

    if (auto buf = key.value<BufferType>()) { args.insert({std::string(buf->begin(), buf->end()), value}); }
  }

  return args;
}

ArgumentArray Marshaler::unmarshalArray(const std::vector<uint8_t> &packet, size_t &idx) {
  uint8_t argc = packet.at(idx++);
  ArgumentArray args;

  for (size_t i = 0; i < argc && idx < packet.size(); ++i) {
    args << unmarshalArgument(packet, idx);
  }

  return args;
}

Argument Marshaler::unmarshalArgument(const std::vector<uint8_t> &packet, size_t &idx) {
  uint8_t type = packet[idx++];

  if (type == ArgumentType::Number) {
    NumberType n = ntohl(*(NumberType *)&packet[idx]);

    idx += sizeof(NumberType);

    return n;
  }

  if (type == ArgumentType::String) {
    uint32_t n = ntohl(*(uint32_t *)&packet[idx]);
    idx += sizeof(uint32_t);

    auto buf = std::vector<uint8_t>(packet.begin() + idx, packet.begin() + idx + n);
    idx += n;

    return buf;
  }

  if (type == ArgumentType::Array) { return std::make_unique<ArgumentArray>(unmarshalArray(packet, idx)); }

  if (type == ArgumentType::Dict) {
    return std::make_unique<Argument::ArgumentDict>(unmarshalDict(packet, idx));
  }

  throw std::runtime_error("unknown argument type");
}
