#pragma once
#include "extension/model/model-parser.hpp"
#include <string_view>

ParsedRenderData parseRenderPayload(std::string_view json);
