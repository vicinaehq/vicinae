#pragma once
#include "extend/model-parser.hpp"
#include <string_view>

ParsedRenderData parseRenderPayload(std::string_view json);
