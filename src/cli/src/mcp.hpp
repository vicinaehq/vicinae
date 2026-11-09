#pragma once
#include <expected>
#include <string>

namespace cli {

std::expected<int, std::string> runMcpServer();

} // namespace cli
