#pragma once
#include <filesystem>
#include <google/protobuf/struct.pb.h>
#include <qdatetime.h>
#include <qjsonvalue.h>
#include <qmimetype.h>
#include <qstring.h>
#include <string_view>

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

bool isTextMimeType(const QMimeType &mime);

/**
 * Whether the path points to a file that is considered to be hidden.
 * A file or directory is considered hidden when itself or one of its ancestor's name
 * starts with a dot ('.').
 */
bool isHiddenPath(const std::filesystem::path &path);

bool isInHomeDirectory(const std::filesystem::path &path);

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
