#pragma once
#include "../parser.hpp"
#include <filesystem>
#include <span>
#include <sstream>

enum class WireFormat {
  Json,
  Beve,
};

struct CodegenOptions {
  std::filesystem::path file;
  std::optional<std::string> generationNamespace;
  WireFormat wire = WireFormat::Json;
};

class AbstractCodeGenerator {
public:
  virtual ~AbstractCodeGenerator() = default;
  virtual std::string name() const = 0;
  virtual std::string generateClient(const Tree &ast, const CodegenOptions &opts = {}) = 0;
  virtual std::string generateServer(const Tree &ast, const CodegenOptions &opts = {}) = 0;
};

inline std::string glazeWirePrelude(WireFormat wire) {
  if (wire == WireFormat::Beve) {
    return "inline constexpr auto WIRE_FORMAT = glz::BEVE;\nusing raw_t = std::vector<std::uint8_t>;\n";
  }
  return "inline constexpr auto WIRE_FORMAT = glz::JSON;\nusing raw_t = glz::raw_json;\n";
}

inline std::string glazeBeveEnumSpecializations(std::string_view ns, std::span<const EnumValue> enums) {
  std::ostringstream oss;

  for (const auto &e : enums) {
    std::ostringstream names;
    names << "static constexpr auto names = std::array<std::string_view, " << e.values.size() << ">{";
    for (std::size_t i = 0; i != e.values.size(); ++i) {
      if (i > 0) names << ", ";
      names << '"' << e.values[i] << '"';
    }
    names << "};";

    oss << "namespace glz {\n";
    oss << "template <> struct to<BEVE, " << ns << "::" << e.name << "> {\n";
    oss << "\ttemplate <auto Opts, class... Args>\n";
    oss << "\tstatic void op(const " << ns << "::" << e.name
        << " &value, is_context auto &&ctx, Args &&...args) {\n";
    oss << "\t\t" << names.str() << "\n";
    oss << "\t\tconst auto index = static_cast<std::size_t>(value);\n";
    oss << "\t\tserialize<BEVE>::op<Opts>(index < names.size() ? names[index] : names[0], ctx, args...);\n";
    oss << "\t}\n};\n";
    oss << "template <> struct from<BEVE, " << ns << "::" << e.name << "> {\n";
    oss << "\ttemplate <auto Opts, class... Args>\n";
    oss << "\tstatic void op(" << ns << "::" << e.name
        << " &value, is_context auto &&ctx, Args &&...args) {\n";
    oss << "\t\t" << names.str() << "\n";
    oss << "\t\tstd::string s;\n";
    oss << "\t\tparse<BEVE>::op<Opts>(s, ctx, args...);\n";
    oss << "\t\tif (static_cast<bool>(ctx.error)) return;\n";
    oss << "\t\tfor (std::size_t i = 0; i != names.size(); ++i) {\n";
    oss << "\t\t\tif (names[i] == s) { value = static_cast<" << ns << "::" << e.name << ">(i); return; }\n";
    oss << "\t\t}\n";
    oss << "\t\tctx.error = error_code::unexpected_enum;\n";
    oss << "\t}\n};\n";
    oss << "}\n";
  }

  return oss.str();
}
