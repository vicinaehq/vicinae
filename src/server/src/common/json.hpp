#pragma once

#include <glaze/core/opts.hpp>

struct JsonWriteOptions : glz::opts {
  bool escape_control_characters = true;
};
