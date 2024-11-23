#include "calculator.hpp"
#include <functional>
#include <iostream>
#include <ostream>

static std::vector<OutputFormat> formats = {
    {OutputFormat::Type::Decimal, {"decimal"}, "decimal"},
    {OutputFormat::Type::Hex, {"hex", "hexa", "hexadecimal"}, "hex"},
    {OutputFormat::Type::Base64, {"b64", "base64"}, "base64"},
};

std::optional<std::reference_wrapper<OutputFormat>>
findFormatByName(std::string_view name) {
  auto pred = [name](OutputFormat &a) {
    std::cout << "candidate=" << "base64" << std::endl;
    return std::find(a.names.begin(), a.names.end(), name) != a.names.end();
  };

  if (auto it = std::find_if(formats.begin(), formats.end(), pred);
      it != formats.end())
    return *it;

  return std::nullopt;
}
