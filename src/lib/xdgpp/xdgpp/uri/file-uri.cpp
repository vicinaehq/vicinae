#include "file-uri.hpp"
#include <cctype>
#include <string_view>

namespace xdgpp {

namespace {

constexpr std::string_view FILE_SCHEME = "file://";
constexpr std::string_view HEX = "0123456789ABCDEF";

bool isUnreservedPathChar(unsigned char c) {
  if (std::isalnum(c)) return true;
  return std::string_view("-._~/!$&'()*+,;=:@").find(static_cast<char>(c)) != std::string_view::npos;
}

int hexValue(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  return -1;
}

} // namespace

std::string toFileUri(const std::filesystem::path &path) {
  std::string uri(FILE_SCHEME);

  for (unsigned char c : path.string()) {
    if (isUnreservedPathChar(c)) {
      uri.push_back(static_cast<char>(c));
    } else {
      uri.push_back('%');
      uri.push_back(HEX[c >> 4]);
      uri.push_back(HEX[c & 0x0F]);
    }
  }

  return uri;
}

std::optional<std::filesystem::path> fromFileUri(std::string_view uri) {
  if (!uri.starts_with(FILE_SCHEME)) return std::nullopt;

  auto rest = uri.substr(FILE_SCHEME.size());
  auto slash = rest.find('/');
  if (slash == std::string_view::npos) return std::nullopt;

  auto host = rest.substr(0, slash);
  if (!host.empty() && host != "localhost") return std::nullopt;

  auto encoded = rest.substr(slash);
  std::string decoded;
  decoded.reserve(encoded.size());

  for (size_t i = 0; i < encoded.size(); ++i) {
    if (encoded[i] != '%') {
      decoded.push_back(encoded[i]);
      continue;
    }

    if (i + 2 >= encoded.size()) return std::nullopt;
    int hi = hexValue(encoded[i + 1]);
    int lo = hexValue(encoded[i + 2]);
    if (hi < 0 || lo < 0 || (hi == 0 && lo == 0)) return std::nullopt;
    decoded.push_back(static_cast<char>((hi << 4) | lo));
    i += 2;
  }

  return std::filesystem::path(std::move(decoded));
}

} // namespace xdgpp
