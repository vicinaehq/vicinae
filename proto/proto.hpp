#pragma once

#include <cstdint>
#include <netinet/in.h>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <endian.h>

namespace Proto {
class Variant;

using String = std::string;
using Int = int32_t;
using Boolean = bool;
using Array = std::vector<Variant>;
using Dict = std::unordered_map<std::string, Variant>;

class Variant {
  union ArgumentUnion {
    Int integer;
    String string;
    Array array;
    Dict dict;
    Boolean boolean;

    ArgumentUnion() : integer(0) {}
    ~ArgumentUnion() {}
  };

  using VariantValue = std::variant<Int, String, Boolean, Array, Dict>;

  VariantValue _value;

public:
  // not amazing, we should try to find something that works at compile time
  template <typename T> static size_t tagForType() { return VariantValue(T{}).index(); }

  bool isArray() const;
  bool isDict() const;
  bool isBool() const;
  bool isString() const;
  bool isInt() const;

  const Array &asArray(const Array &dflt = {}) const;
  const Dict &asDict(const Dict &dict = {}) const;
  bool asBool(bool dflt = {}) const;
  const std::string &asString(const std::string &dflt = {}) const;
  Int asInt(int dflt = {}) const;

  const Array *tryArray() const;
  const Dict *tryDict() const;
  const bool *tryBool() const;
  const std::string *tryString() const;
  const Int *tryInt() const;

  size_t tag() const { return _value.index(); }

  Variant(const std::vector<uint8_t> &s);
  Variant(Int n);
  Variant(const Dict &dict);
  Variant(const Array &dict);
  Variant(bool val);
  Variant(const std::string &string);
  Variant(const char *s);
  Variant(unsigned int n);
  Variant();
};

class Marshaler {
public:
  struct Error {
    std::string message;

  public:
    Error(const std::string &m) : message(m) {}
  };

protected:
  void marshal(std::vector<uint8_t> &packet, uint32_t value) const noexcept;
  void marshal(std::vector<uint8_t> &packet, const Variant &arg) const noexcept;
  void marshal(std::vector<uint8_t> &packet, const Array &array) const noexcept;
  void marshal(std::vector<uint8_t> &packet, const Dict &dict) const noexcept;

  std::optional<Error> unmarshal(const std::vector<uint8_t> &packet, size_t &idx, Array &arr) const noexcept;
  std::optional<Error> unmarshal(const std::vector<uint8_t> &packet, size_t &idx,
                                 Variant &arg) const noexcept;
  std::optional<Error> unmarshal(const std::vector<uint8_t> &packet, size_t &idx, Dict &dict) const noexcept;

public:
  template <typename T> std::vector<uint8_t> marshal(const T &item) noexcept {
    std::vector<uint8_t> packet;

    marshal(packet, item);

    return packet;
  }

  template <typename T> std::variant<T, Error> unmarshal(const std::vector<uint8_t> &packet) {
    T item;
    size_t idx = 0;

    if (auto err = unmarshal(packet, idx, item)) { return *err; }

    return item;
  }
};
} // namespace Proto
