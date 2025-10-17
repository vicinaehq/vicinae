#pragma once
#include <filesystem>
#include <google/protobuf/struct.pb.h>
#include <iterator>
#include <qdatetime.h>
#include <qjsonvalue.h>
#include <qmimetype.h>
#include <qstring.h>
#include <string_view>

template <template <typename...> class Template, typename T> struct is_specialization : std::false_type {};

template <template <typename...> class Template, typename... Args>
struct is_specialization<Template, Template<Args...>> : std::true_type {};

// Custom implementation of ranges::to
template <template <typename...> class Container, typename Range> auto ranges_to(Range &&range) {
  using ValueType = std::decay_t<decltype(*std::begin(range))>;

  if constexpr (std::is_same_v<Container<ValueType>, std::set<ValueType>> ||
                std::is_same_v<Container<ValueType>, std::multiset<ValueType>>) {
    // For associative containers
    return Container<ValueType>(std::begin(range), std::end(range));
  } else {
    // For sequence containers
    Container<ValueType> result;

    // Try to reserve if possible (for vector, deque, etc.)
    if constexpr (requires { result.reserve(0); }) {
      if constexpr (requires { std::size(range); }) { result.reserve(std::size(range)); }
    }

    std::copy(std::begin(range), std::end(range), std::back_inserter(result));
    return result;
  }
}

// we can't yet use std::ranges::to
template <typename U> U ranges_to(const auto &ct) {
  U target;
  std::copy(ct.begin(), ct.end(), std::back_inserter(target));

  return target;
}

/**
 * Attempts to compress the path as much as possible to make it better
 * fit in when not a lot of space is available.
 * Typically, if the path starts with $HOME, it is replaced by the '~' symbol.
 */
std::filesystem::path compressPath(const std::filesystem::path &);
std::filesystem::path expandPath(const std::filesystem::path &);
std::filesystem::path homeDir();

QString getRelativeTimeString(const QDateTime &pastTime);

QString qStringFromStdView(std::string_view view);

/**
 * Whether the path points to a file that is considered to be hidden.
 * A file or directory is considered hidden when itself or one of its ancestor's name
 * starts with a dot ('.').
 */
bool isHiddenPath(const std::filesystem::path &path);

bool isInHomeDirectory(const std::filesystem::path &path);

std::filesystem::path stripPathComponents(const std::filesystem::path &path, int n);

QString formatCount(int count);

std::filesystem::path downloadsFolder();
std::filesystem::path documentsFolder();

/**
 * The list of directories found at the root of the current user's home directory.
 */
std::vector<std::filesystem::path> homeRootDirectories();

std::string getLastPathComponent(const std::filesystem::path &path);

google::protobuf::Value transformJsonValueToProto(const QJsonValue &value);
QJsonValue protoToJsonValue(const google::protobuf::Value &value);

QString formatSize(size_t bytes);

QString slugify(const QString &input, const QString &separator = "-");

namespace Utils {
inline std::vector<QString> toQStringVec(const std::vector<std::string> &strings) {
  std::vector<QString> list;
  list.reserve(strings.size());
  for (const auto &str : strings) {
    list.emplace_back(QString::fromStdString(str));
  }
  return list;
}

inline std::vector<std::string> toStdStringVec(const std::vector<QString> &strings) {
  std::vector<std::string> list;
  list.reserve(strings.size());
  for (const auto &str : strings) {
    list.emplace_back(str.toStdString());
  }
  return list;
}

/**
 * Strips the encoding part in a mime name, typically
 * in 'text/plain;charset-utf8
 */
QString normalizeMimeName(const QString &name);

bool isX11TextTarget(const QString &text);

bool isTextMimeType(const QMimeType &mime);

/**
 * Returns whether the mime name refers to a text mime type.
 * This will also return true if `mimeName` is one of the X11 clipboard
 * text targets (UTF8_STRING, STRING, TEXT...)
 * Content described by such a mime type should be safe to render as plain text.
 */
bool isTextMimeType(const QString &mimeName);

QColor colorFromString(const QString &str);

QString rgbaFromColor(const QColor &color);

std::string slurp(std::istream &ifs);
}; // namespace Utils
