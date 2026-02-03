#pragma once
#include <string>
#include <cstdint>

/**
 * Represents an entrypoint.
 * An entrypoint is always scoped under a provider.
 * extension = provider, command = entrypoint inside the extension
 */
struct EntrypointId {
  std::string provider;
  std::string entrypoint;

  operator std::string() const {
    std::string s;

    s.reserve(provider.size() + entrypoint.size() + 1);
    s.append(provider).append(":").append(entrypoint);

    return s;
  }

  static EntrypointId fromSerialized(std::string_view s) {
    auto pos = s.find(':');
    if (pos == std::string::npos) return {};
    return EntrypointId(std::string{s.substr(0, pos)}, std::string{s.substr(pos + 1)});
  }

  bool operator==(const EntrypointId &rhs) const {
    return provider == rhs.provider && entrypoint == rhs.entrypoint;
  }
};

namespace std {
template <> class hash<EntrypointId> {
public:
  std::uint64_t operator()(const EntrypointId &id) const { return std::hash<std::string>{}(id); }
};
}; // namespace std
