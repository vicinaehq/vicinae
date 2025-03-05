#pragma once

#include <cstdint>
#include <deque>
#include <map>
#include <memory>
#include <netinet/in.h>
#include <string>
#include <variant>
#include <vector>
#include <endian.h>

enum ArgumentType { Array, Object, String, Number, Dict };

using BufferType = std::vector<uint8_t>;
using NumberType = int32_t;

class ArgumentArray;

struct Argument {
  using ArgumentDict = std::map<std::string, Argument>;
  using ArgumentValue =
      std::variant<NumberType, BufferType, std::unique_ptr<ArgumentArray>, std::unique_ptr<ArgumentDict>>;

  ArgumentValue _value;

  template <typename T> const T *value() const {
    if (auto p = std::get_if<T>(&_value)) { return p; }

    return nullptr;
  }

  const ArgumentArray *array() const {
    auto arr = value<std::unique_ptr<ArgumentArray>>();
    if (arr) return arr->get();
    return nullptr;
  }

  std::string string(const std::string &dflt = "") const {
    if (auto n = std::get_if<BufferType>(&_value)) { return {n->begin(), n->end()}; }

    return dflt;
  }

  NumberType number(NumberType dflt = 0) const {
    if (auto n = std::get_if<NumberType>(&_value)) { return *n; }

    return dflt;
  }

  Argument &operator=(const Argument &arg) {
    if (auto n = std::get_if<NumberType>(&arg._value)) {
      _value = *n;
    } else if (auto buffer = std::get_if<BufferType>(&arg._value)) {
      _value = *buffer;
    } else if (auto array = std::get_if<std::unique_ptr<ArgumentArray>>(&arg._value)) {
      _value = std::make_unique<ArgumentArray>(*array->get());
    } else if (auto array = std::get_if<std::unique_ptr<ArgumentDict>>(&arg._value)) {
      _value = std::make_unique<ArgumentDict>(*array->get());
    }

    return *this;
  }

  Argument(const Argument &rhs) noexcept { *this = rhs; }

  Argument(std::unique_ptr<ArgumentArray> array) { _value = std::move(array); }
  Argument(std::unique_ptr<ArgumentDict> dict) { _value = std::move(dict); }
  Argument(const std::string &s) { _value = BufferType{s.begin(), s.end()}; }
  Argument(const char *s) { *this = std::string(s); }
  template <typename T> Argument(const T &init) { _value = init; }
  Argument() {}

  ~Argument() {}
};

namespace Marshaler {
void marshal(std::vector<uint8_t> &packet, uint32_t value);
void marshal(std::vector<uint8_t> &packet, const Argument &arg);
void marshal(std::vector<uint8_t> &packet, const ArgumentArray &array);
void marshalString(std::vector<uint8_t> &packet, const std::vector<uint8_t> &s);
void marshalDict(std::vector<uint8_t> &packet, const Argument::ArgumentDict &dict);

ArgumentArray unmarshalArray(const std::vector<uint8_t> &packet, size_t &idx);
Argument unmarshalArgument(const std::vector<uint8_t> &packet, size_t &idx);
Argument::ArgumentDict unmarshalDict(const std::vector<uint8_t> &packet, size_t &idx);

}; // namespace Marshaler

class ArgumentArray {
  std::deque<Argument> _args;

public:
  auto begin() const { return _args.begin(); }
  auto end() const { return _args.end(); }

private:
public:
  template <typename T> void append(const T &value) { _args.push_back(value); }

  void append(const std::string &value) { _args.push_back(BufferType(value.begin(), value.end())); }
  void append(const char *value) { append(std::string(value)); }

  void append(const int &value) { _args.push_back(static_cast<NumberType>(value)); }
  void append(const ArgumentArray &array) { _args.push_back(std::make_unique<ArgumentArray>(array)); }
  void append(const Argument::ArgumentDict &array) {
    _args.push_back(std::make_unique<Argument::ArgumentDict>(array));
  }

  template <typename T> void pop(T &value) {
    value = *_args.at(0).value<T>();
    _args.pop_front();
  }
  void pop(int &value) {
    value = *_args.at(0).value<NumberType>();
    _args.pop_front();
  }
  void pop(unsigned int &value) {
    value = *_args.at(0).value<NumberType>();
    _args.pop_front();
  }
  void pop(ArgumentArray &array) {
    array = *_args.at(0).value<std::unique_ptr<ArgumentArray>>()->get();
    _args.pop_front();
  }
  void pop(Argument::ArgumentDict &dict) {
    dict = *_args.at(0).value<std::unique_ptr<Argument::ArgumentDict>>()->get();
    _args.pop_front();
  }
  void pop(std::string &value) {
    std::vector<uint8_t> buf;

    pop(buf);
    value = std::string(buf.begin(), buf.end());
  }

  template <typename T> ArgumentArray &operator>>(T &value) {
    pop(value);
    return *this;
  }

  template <typename T> ArgumentArray &operator<<(const T &value) {
    append(value);
    return *this;
  }

  size_t size() const { return _args.size(); }

  ArgumentArray() {}

  ArgumentArray(const std::vector<uint8_t> &packet) {
    size_t idx = 0;

    _args = Marshaler::unmarshalArray(packet, idx)._args;
  }

  std::vector<uint8_t> build() const {
    std::vector<uint8_t> packet;

    Marshaler::marshal(packet, *this);

    return packet;
  }
};
