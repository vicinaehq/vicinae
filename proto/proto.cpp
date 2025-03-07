#include "proto.hpp"
#include <cstdint>
#include <iostream>
#include <string>
#include <variant>

using namespace Proto;

bool Variant::isArray() const { return std::holds_alternative<Array>(_value); }
bool Variant::isDict() const { return std::holds_alternative<Dict>(_value); }
bool Variant::isBool() const { return std::holds_alternative<Boolean>(_value); }
bool Variant::isString() const { return std::holds_alternative<String>(_value); }
bool Variant::isInt() const { return std::holds_alternative<Int>(_value); }

const Array &Variant::asArray(const Array &dflt) const { return isArray() ? std::get<Array>(_value) : dflt; }
const Dict &Variant::asDict(const Dict &dict) const { return isDict() ? std::get<Dict>(_value) : dict; }
bool Variant::asBool(bool dflt) const { return isBool() ? std::get<Boolean>(_value) : dflt; }
const std::string &Variant::asString(const std::string &dflt) const {
  return isString() ? std::get<String>(_value) : dflt;
}
Int Variant::asInt(int dflt) const { return isInt() ? std::get<Int>(_value) : dflt; }

const Array *Variant::tryArray() const { return isArray() ? &std::get<Array>(_value) : nullptr; }
const Dict *Variant::tryDict() const { return isDict() ? &std::get<Dict>(_value) : nullptr; }
const bool *Variant::tryBool() const { return isBool() ? &std::get<Boolean>(_value) : nullptr; }
const std::string *Variant::tryString() const { return isString() ? &std::get<String>(_value) : nullptr; }
const Int *Variant::tryInt() const { return isInt() ? &std::get<Int>(_value) : nullptr; }

Variant::Variant(const std::vector<uint8_t> &s) { *this = String{s.begin(), s.end()}; }
Variant::Variant(Int n) { _value = n; }
Variant::Variant(const char *s) { *this = std::string(s); }
Variant::Variant(const Array &arr) { _value = arr; }
Variant::Variant(const Dict &dict) { _value = dict; }
Variant::Variant(unsigned int n) { *this = static_cast<Int>(n); }
Variant::Variant(const std::string &string) { _value = string; }
Variant::Variant(bool val) { _value = val; }
Variant::Variant() { *this = 0; }

void Marshaler::marshal(std::vector<uint8_t> &packet, const Variant &arg) const noexcept {
  if (auto s = arg.tryString()) {
    marshal(packet, s->size());
    packet.insert(packet.end(), s->begin(), s->end());
  } else if (auto n = arg.tryInt()) {
    marshal(packet, *n);
  } else if (auto n = arg.tryBool()) {
    packet.push_back(*n);
  } else if (auto array = arg.tryArray()) {
    marshal(packet, *array);
  } else if (auto dict = arg.tryDict()) {
    marshal(packet, *dict);
  }
}

void Marshaler::marshal(std::vector<uint8_t> &packet, const Array &array) const noexcept {
  packet.push_back(std::min(array.size(), (size_t)UINT8_MAX));

  for (const auto &arg : array) {
    packet.push_back(arg.tag());
    marshal(packet, arg);
  }
}

void Marshaler::marshal(std::vector<uint8_t> &packet, uint32_t value) const noexcept {
  int no = htonl(value);
  uint8_t *p = (uint8_t *)(&no);

  packet.push_back(p[0]);
  packet.push_back(p[1]);
  packet.push_back(p[2]);
  packet.push_back(p[3]);
}

void Marshaler::marshal(std::vector<uint8_t> &packet, const Dict &dict) const noexcept {
  packet.push_back(std::min(dict.size(), (size_t)UINT8_MAX));

  for (const auto &[key, value] : dict) {
    packet.push_back(Variant::tagForType<std::string>());
    marshal(packet, static_cast<uint32_t>(key.size()));
    packet.insert(packet.end(), key.begin(), key.end());
    packet.push_back(value.tag());
    marshal(packet, value);
  }
}

std::optional<Marshaler::Error> Marshaler::unmarshal(const std::vector<uint8_t> &packet, size_t &idx,
                                                     Dict &dict) const noexcept {
  uint8_t argc = packet.at(idx++);

  dict.reserve(argc);

  for (size_t i = 0; i < argc && idx < packet.size(); ++i) {
    Variant key, value;

    if (auto err = unmarshal(packet, idx, key)) { return err; }
    if (auto err = unmarshal(packet, idx, value)) { return err; }
    if (auto buf = key.tryString()) { dict.insert({*buf, value}); }
  }

  return std::nullopt;
}

std::optional<Marshaler::Error> Marshaler::unmarshal(const std::vector<uint8_t> &packet, size_t &idx,
                                                     Array &arr) const noexcept {
  uint8_t argc = packet.at(idx++);

  arr.reserve(argc);

  for (size_t i = 0; i < argc && idx < packet.size(); ++i) {
    Variant arg;

    if (auto err = unmarshal(packet, idx, arg)) { return err; }
    arr.push_back(arg);
  }

  return std::nullopt;
}

std::optional<Marshaler::Error> Marshaler::unmarshal(const std::vector<uint8_t> &packet, size_t &idx,
                                                     Variant &arg) const noexcept {
  uint8_t tag = packet[idx++];

  if (tag == Variant::tagForType<Int>()) {
    if (packet.size() - idx < sizeof(Int)) { return Error("Failed to unmarshal number: overflow detected"); }

    arg = ntohl(*reinterpret_cast<const Int *>(&packet[idx]));
    idx += sizeof(Int);

    return std::nullopt;
  }

  if (tag == Variant::tagForType<std::string>()) {
    if (packet.size() - idx < sizeof(uint32_t)) {
      return Error("Failed to unmarshal string length: overflow detected");
    }

    uint32_t n = ntohl(*reinterpret_cast<const uint32_t *>(&packet[idx]));

    idx += sizeof(uint32_t);

    if (packet.size() - idx < n) { return Error("Failed to unmarshal string: overflow detected"); }

    arg = std::string(packet.begin() + idx, packet.begin() + idx + n);
    idx += n;

    return std::nullopt;
  }

  if (tag == Variant::tagForType<Boolean>()) {
    if (idx >= packet.size()) { return Error("Failed to unmarshal boolean: overflow detected"); }

    arg = static_cast<bool>(packet[idx++]);

    return std::nullopt;
  }

  if (tag == Variant::tagForType<Array>()) {
    Array arr;

    if (auto err = unmarshal(packet, idx, arr)) { return err; }

    arg = arr;

    return std::nullopt;
  }

  if (tag == Variant::tagForType<Dict>()) {
    Dict dict;

    if (auto err = unmarshal(packet, idx, dict)) { return err; }
    arg = dict;

    return std::nullopt;
  }

  return Error("Failed to unmarshal unknown type " + std::to_string(tag));
}
